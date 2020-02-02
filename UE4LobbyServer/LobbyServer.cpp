#include "LobbyServer.hpp"
#include "PacketGenerateHelper.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "../InterServerOpcode.hpp"
#include "../NetworkOpcode.h"
#include <iostream>
#include <sstream>


LobbyServer::LobbyServer()
{
}

void LobbyServer::Initialize()
{
    reader_.Initialize("ServerSetting.ini");
    auto opt_odbc = reader_.GetString("ODBC");
    auto opt_db_id = reader_.GetString("DB_ID");
    auto opt_db_pw = reader_.GetString("DB_PW");
    auto opt_ip = reader_.GetString("LOBBY_SERVER_IP");
    auto opt_port = reader_.GetInt32("LOBBY_SERVER_PORT");
    auto opt_max_conn = reader_.GetInt32("LOBBY_SERVER_MAX_CONNECTION");
    auto opt_worker_size = reader_.GetInt32("LOBBY_SERVER_NUM_IO_WORKER");
    if (!opt_odbc || !opt_db_id || !opt_db_pw || !opt_ip ||  !opt_port || !opt_max_conn || !opt_worker_size )
    {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
    this_info_.SetServerType(ServerType::kLobbyServer);
    this_info_.SetIP(*opt_ip);
    this_info_.SetPort(*opt_port);
    this_info_.SetCurrentConnection(0);
    this_info_.SetMaxConnection(*opt_max_conn);
#ifdef _UNICODE
    std::string str1 = std::string(*opt_odbc);
    std::string str2 = std::string(*opt_db_id);
    std::string str3 = std::string(*opt_db_pw);
    string_t odbc_ = std::wstring(str1.begin(), str1.end());
    string_t db_id_ = std::wstring(str2.begin(), str2.end());
    string_t db_pw_ = std::wstring(str3.begin(), str3.end());
    ODBCConnectionPool::Instance().Initialize(3, odbc_, db_id_, db_pw_);
#else
    ODBCConnectionPool::Instance().Initialize(3, *opt_odbc, *opt_db_id, *opt_db_pw);
#endif
    io_server_.emplace(*opt_max_conn, *opt_port, *opt_worker_size);
    io_server_->Initialize();
    session_ = std::make_shared<IntermediateSession>(io_server_->GetContext());

    IntermediateSession::Handler::BindFunction(
        static_cast<int16_t>(IntermediateServerSendPacket::kNotifySessionAuthorityInfo),
        [this](IntermediateSession& session, IntermediateSession::InPacket& input) {
            std::string login_server_uuid = input.ReadString();
            std::string user_uuid = input.ReadString();
            RemoteSessionInfo info;
            input >> info;

            {
                std::unique_lock lock(session_authority_guard_);
                authority_map_.emplace(info.GetId(), info);
            }
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(
                IntermediateServerReceivePacket::kReactSessionAuthorityInfo));
            out << login_server_uuid;
            out << user_uuid;
            out.MakePacketHead();
            session.Send(out, false, false);
            /*GetNioServer()->CreateTimer(
                [this, info]() {
                    std::unique_lock lock(session_authority_guard_);
                    auto iter = authority_map_.find(info.GetId());
                    if (iter != authority_map_.end()) {
                        if (iter->second.GetClock().to_int64_t() == info.GetClock().to_int64_t()) {
                            authority_map_.erase(iter);
                        }
                    }
                }, std::chrono::milliseconds(3000));*/
        }
    );
    IntermediateSession::Handler::BindFunction( // response from ZoneServer
        static_cast<int16_t>(IntermediateServerSendPacket::kReactUserMigation),
        [this](IntermediateSession& session, IntermediateSession::InPacket& input) {
            std::string user_uuid = input.ReadString();
            std::string zone_ip = input.ReadString();
            uint16_t zone_port = input.ReadInt16();

            auto client = this->GetClient(user_uuid);
            if (client) {
                UE4OutPacket out;
                out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kMigrateZoneNotify));
                out << zone_ip;
                out << zone_port;
                out.MakePacketHead();
                client->GetSession()->Send(out, false, false);
            }
        }
    );
}

void LobbyServer::Run()
{
    if (io_server_) {
        io_server_->Run();
        ConnectChannel();
        std::cout << "Input Command\n[1] : PrintServerState\n[2] : Try Connect Other Server Channel\n[q] : Server Stop\n";;
        std::string line;
        while (true)
        {
            std::cin >> line;
            try {
                if (line._Equal("q")) {
                    break;
                }
                int command = std::atoi(line.c_str());
                switch (command)
                {
                    case 1:
                        break;
                    case 2:
                        this->ConnectChannel();
                        break;
                    default:
                        std::cout << "find command fail...\n";
                }
            } catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

void LobbyServer::Stop()
{
    if (io_server_) {
        io_server_->Stop();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "nio server is nullptr");
    }
}

void LobbyServer::ConnectChannel()
{
    auto opt_ip = reader_.GetString("INTERMEDIATE_SERVER_IP");
    auto opt_port = reader_.GetInt32("INTERMEDIATE_SERVER_PORT");
    if (!opt_ip || !opt_port) {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
    if (session_ && session_->IsOpen() == false) {
        if (session_->SyncConnect(opt_ip->c_str(), *opt_port) == false) {
            std::stringstream ss;
            Clock clock;
            ss << '[' << Calendar::DateTime(clock) << "] connect to intermediate server(" << *opt_ip << ":" << *opt_port << ") fail \n";
            std::cout << ss.str();
        } else {
            session_->OnConnect();
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kRegisterRemoteServer));
            out << this_info_;
            out.MakePacketHead();
            session_->Send(out, false, false);
        }
    }
}

void LobbyServer::OnActive(Session& session)
{
    auto client = std::make_shared<Client>(session.shared_from_this());
    std::unique_lock lock(client_storage_guard_);
    client_storage_.emplace(client->GetUUID().ToString(), client);
    session.SetClientKey(client->GetUUID().ToString());

    client->SetState(ClientState::kNotConfirm);
    //GetNioServer()->CreateTimer(
    //    [uuid = client->GetUUID(), this]() 
    //    {
    //        auto client = this->GetClient(uuid.ToString());
    //        if (client && client->GetState() == ClientState::kNotConfirm) {
    //            this->CloseClient(uuid.ToString());
    //        }
    //    }, std::chrono::milliseconds(1000));
}

void LobbyServer::OnClose(Session& session)
{
    std::shared_ptr<Client> client = nullptr;
    {
        std::unique_lock lock(client_storage_guard_);
        auto iter = client_storage_.find(session.GetClientKey());
        if (iter != client_storage_.end()) {
            client = iter->second;
            client_storage_.erase(iter);
        }
    }
    if (client) {
        // 이 경우에는 LoginServer로 Packet을 보내서 접속중인 연결이 아님을 통지
        if (client->GetState() == ClientState::kConfirm) {
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kNotifyUserLogout));
            out.WriteInt32(client->GetAccid());
            out.MakePacketHead();
            session_->Send(out, false, false);
        }
    }
}

void LobbyServer::OnError(int ec, const char* message)
{
}

void LobbyServer::ProcessPacket(Session& session, const std::shared_ptr<Session::InPacket>& in_packet)
{
    auto client = this->GetClient(session.GetClientKey());
    if (!client) {
        throw StackTraceException(ExceptionType::kNullPointer, "client is nullptr");
    }
    in_packet->SetAccesOffset(2);
    try {
        int16_t opcode = in_packet->ReadInt16();
        switch (static_cast<ENetworkCSOpcode>(opcode))
        {
            case ENetworkCSOpcode::kLobbyConfirmRequest:
                HandleConfirmRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kCharacterListRequest:
                HandleCharacterListRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kCharacterCreateRequest:
                HandleCharacterCreateRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kCharacterDeleteRequest:
                HandleCharacterDeleteRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kCharacterSelectionRequest:
                HandleCharacterSelectRequest(*client, *in_packet);
                break;
        }
    } catch (const std::exception & e) {
        std::stringstream ss;
        Clock clock;
        std::string date = Calendar::DateTime(clock);
        ss << "[" << date << "] Exception: " << e.what() << '\n';
        date = ss.str();
        std::cout << date;
        client->Close();
    }
}

std::shared_ptr<Client> LobbyServer::GetClient(const std::string& uuid) const
{
    std::shared_lock lock(client_storage_guard_);
    auto iter = client_storage_.find(uuid);
    if (iter != client_storage_.end()) {
        return iter->second;
    }
    return nullptr;
}

void LobbyServer::HandleConfirmRequest(Client& client, Session::InPacket& in_packet)
{
    bool result = false;
    std::string id = in_packet.ReadString();
    const auto& ip = client.GetSession()->GetRemoteAddress();
    {
        std::shared_lock lock(session_authority_guard_);
        auto iter = authority_map_.find(id);
        if (iter != authority_map_.end() && iter->second.GetIp() == ip) {
            client.SetState(LobbyServer::kConfirm);
            client.SetAccid(iter->second.GetAccid());
            client.SetAccount(id);
            result = true;
        }
    }
    if (result) {
        SendCharacterList(client);
    } else {
        // 실패 보내기
    }
}

void LobbyServer::HandleCharacterListRequest(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm)
        return;
    SendCharacterList(client);
}

void LobbyServer::HandleCharacterCreateRequest(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm) // log 추가
        return;
    int32_t accid = client.GetAccid();
    PacketGenerateHelper::LobbyCharacterInfo info;
    PacketGenerateHelper::ReadLobbyCharacterInfo(in_packet, info);
    if (info.slot < 0 || info.slot >= slot_size) { // log 추가
        client.Close();
        return;
    }
    {
        std::shared_lock lock(slot_info_lock);
        if (slot_info_[client.GetUUID()][info.slot] != -1) {
            client.Close();
            return;
        }
    }
    info.level = 1;
    //info.hair = 110;
    //info.face = 120;
    info.str = 10;
    info.dex = 10;
    info.intel = 10;
    info.zone = 100;
    info.x = 0.0f;
    info.y = 0.0f;
    info.z = 0.0f;
    info.gold = 0;
    info.armor_itemid = 3000001;
    info.hand_itemid = 3100001;
    info.shoes_itemid = 3200001;
    info.weapon_itemid = 3300001;

    std::string name = info.name;
    auto con = ODBCConnectionPool::Instance().GetConnection();
    auto ps = con->GetPreparedStatement();
    ps->PrepareStatement(L"insert into characters\
(accid, slot, name, level, str, dex, intel, job, face, hair, gold, zone, x, y, z, gender) values\
(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    ps->SetInt32(1, &accid);
    ps->SetInt32(2, &info.slot);
    ps->SetString(3, name);
    ps->SetInt32(4, &info.level);
    ps->SetInt32(5, &info.str);
    ps->SetInt32(6, &info.dex);
    ps->SetInt32(7, &info.intel);
    ps->SetInt32(8, &info.job);
    ps->SetInt32(9, &info.face);
    ps->SetInt32(10, &info.hair);
    ps->SetInt32(11, &info.gold);
    ps->SetInt32(12, &info.zone);
    ps->SetFloat32(13, &info.x);
    ps->SetFloat32(14, &info.y);
    ps->SetFloat32(15, &info.z);
    ps->SetInt32(16, &info.gender);
    ps->SetInt32(17, &info);
    ps->ExecuteUpdate();

    ps->Close();
    ps->PrepareStatement(L"select cid from characters where name = ?");
    ps->SetString(1, name);
    auto rs = ps->Execute();
    int32_t cid = -1;
    rs->BindInt32(1, &cid);

    UE4OutPacket out;
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kCharacterCreateNotify));
    if (rs->Next()) {
        rs->Close();
        static std::array<int32_t, 5> default_items = { 3000001 , 3100001 , 3200001 ,3300001 };
        info.armor_itemid = 3000001;
        info.hand_itemid = 3100001;
        info.shoes_itemid = 3200001;
        info.weapon_itemid = 3300001;
        std::shared_lock lock(slot_info_lock);
        slot_info_[client.GetUUID()][info.slot] = cid;
        for (int i = 0; i < default_items.size(); ++i)
        {
            ps->Close();
            ps->PrepareStatement(L"insert into inventoryitems\
(cid, accid, itemid, type, quantity, slot, atk, def, str, dex, intel, equipped) values\
(?, ?, ?, 3, 1, -1, 0, 0, 0, 0, 0, 1)");
            ps->SetInt32(1, &cid);
            ps->SetInt32(2, &accid);
            ps->SetInt32(3, &default_items[i]);
            ps->ExecuteUpdate();
        }
        PacketGenerateHelper::WriteLobbyCharacterInfo(out, info);
    } else {
        out.WriteInt32(-1);
    }
    ps->Close();
    out.MakePacketHead();
    client.GetSession()->Send(out, true, false);
    // 실패
}

void LobbyServer::HandleCharacterDeleteRequest(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm)
        return;

    int32_t slot = in_packet.ReadInt32();
    if (slot < 0 || slot > slot_size) { // log 추가
        client.Close();
        return;
    }
    int32_t cid = 0;
    {
        std::shared_lock lock(slot_info_lock);
        cid = slot_info_[client.GetUUID()][slot];
    }
    if (cid == -1) {
        client.Close();
        return;
    }
    auto con = ODBCConnectionPool::Instance().GetConnection();
    auto ps = con->GetPreparedStatement();
    ps->PrepareStatement(L"delete from characters where cid = ?");
    ps->SetInt32(1, &cid);
    ps->ExecuteUpdate();

    {
        std::unique_lock lock(slot_info_lock);
        slot_info_[client.GetUUID()][slot] = -1;
    }

    UE4OutPacket out;
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kCharacterDeleteNotify));
    out.WriteInt32(slot);
    out.MakePacketHead();
    client.GetSession()->Send(out, true, false);
}

void LobbyServer::HandleCharacterSelectRequest(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm)
        return;
    client.SetState(ClientState::kReserveToMigrate);

    int32_t slot = in_packet.ReadInt32();
    std::shared_lock lock(slot_info_lock);
    if (slot < 0 || slot >= slot_size || slot_info_[client.GetUUID()][slot] == -1) {
        Clock clock;
        std::stringstream ss;
        ss << "[" << Calendar::DateTime(clock) << "] Invalid Character Select Reqeust... slot: " << slot;
        throw StackTraceException(ExceptionType::kRunTimeError, ss.str().c_str());
    }

    RemoteSessionInfo info;
    info.SetAccid(client.GetAccid());
    info.SetCid(slot_info_[client.GetUUID()][slot]);
    info.SetId(client.GetAccount());
    info.SetIp(client.GetSession()->GetRemoteAddress());

    UE4OutPacket out;
    out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kRequestUserMigration)); // opcode
    out << static_cast<int16_t>(ServerType::kZoneServer);
    out << client.GetUUID();
    out << info;
    out.MakePacketHead();
    session_->Send(out, false, false);
}

void LobbyServer::SendCharacterList(Client& client)
{
    int accid = client.GetAccid();
    auto con = ODBCConnectionPool::Instance().GetConnection();

    auto ps = con->GetPreparedStatement();
    ps->PrepareStatement(L"select cid from characters where accid = ?");
    ps->SetInt32(1, &accid);
    auto rs = ps->Execute();

    std::vector<int> cids_;

    int cid;
    rs->BindInt32(1, &cid);
    while (rs->Next()) {
        cids_.push_back(cid);
    }
    rs->Close();
    ps->Close();

    UE4OutPacket out;
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kCharacterListNotify));
    out.WriteInt32(cids_.size());

    std::array<int32_t, slot_size> slot_cid;
    slot_cid.fill(-1);
    if (cids_.size() > 0) 
    {
        for (size_t i = 0; i < cids_.size(); ++i)
        {
            ps = con->GetPreparedStatement();
            ps->PrepareStatement(L"select slot, name, level, str, dex, intel, job, face, hair, gold, zone, x, y, z, gender from characters where cid = ?");
            ps->SetInt32(1, &cids_[i]);
            
            rs = ps->Execute();

            PacketGenerateHelper::LobbyCharacterInfo info;
            rs->BindInt32(1, &info.slot);
            rs->BindString(2, info.name, 100);
            rs->BindInt32(3, &info.level);
            rs->BindInt32(4, &info.str);
            rs->BindInt32(5, &info.dex);
            rs->BindInt32(6, &info.intel);
            rs->BindInt32(7, &info.job);
            rs->BindInt32(8, &info.face);
            rs->BindInt32(9, &info.hair);
            rs->BindInt32(10, &info.gold);
            rs->BindInt32(11, &info.zone);
            rs->BindFloat32(12, &info.x);
            rs->BindFloat32(13, &info.y);
            rs->BindFloat32(14, &info.z);
            rs->BindInt32(15, &info.gender);
            int count = 0;
            if (rs->Next()) {
                if (info.slot < 0 || info.slot >= slot_size) {
                    throw StackTraceException(ExceptionType::kSQLError, "stored slot index is wrong");
                }
                rs->Close();
                ps->Close();
                ps->PrepareStatement(L"select itemid from inventoryitems where cid = ? and equipped = 1");
                ps->SetInt32(1, &cids_[i]);
                rs = ps->Execute();
                int32_t itemid;
                rs->BindInt32(1, &itemid);
                while (rs->Next()) {
                    switch ((itemid / 100000) % 10) {
                        case 0:
                            info.armor_itemid = itemid;
                            break;
                        case 1:
                            info.hand_itemid = itemid;
                            break;
                        case 2:
                            info.shoes_itemid = itemid;
                            break;
                        case 3:
                            info.weapon_itemid = itemid;
                            break;
                    }
                }
                PacketGenerateHelper::WriteLobbyCharacterInfo(out, info);
                slot_cid[info.slot] = cids_[i];
            } else {
                // 실패 패킷 보내기
            }
            rs->Close();
            ps->Close();
        }
    }
    out.MakePacketHead();
    client.GetSession()->Send(out, true, false);

    std::unique_lock lock(slot_info_lock);
    slot_info_[client.GetUUID()] = slot_cid;
}