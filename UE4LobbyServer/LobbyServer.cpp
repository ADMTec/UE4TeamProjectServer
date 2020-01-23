#include "LobbyServer.hpp"
#include "LobbyEventHandler.hpp"
#include "PacketGenerateHelper.hpp"
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
    NioServerBuilder builder;
    builder.SetPort(*opt_port)
        .SetMaxConnection(*opt_max_conn)
        .SetNioThreadCount(*opt_worker_size)
        .SetNioInternalBufferSize(2048)
        .SetNioPacketCipher(std::shared_ptr<NioCipher>(new UE4PacketCipher()))
        .SetNioEventHandler(std::shared_ptr<NioEventHandler>(new LobbyEventHandler()));
    this->SetNioServer(builder.Build());

    GetNioServer()->GetChannel().MakeConnection(intermediate_server);
    GetNioServer()->GetChannel().GetConnection(intermediate_server).BindFunction(
        static_cast<int16_t>(IntermediateServerSendPacket::kNotifySessionAuthorityInfo),
        [this](NioSession& session, InputStream& input) {
            std::string login_server_uuid = input.ReadString();
            std::string user_uuid = input.ReadString();
            RemoteSessionInfo info;
            input >> info;

            {
                std::unique_lock lock(session_authority_guard_);
                authority_map_.emplace(info.GetId(), info);
            }
            GetNioServer()->CreateTimer(
                [this, info]() {
                    std::unique_lock lock(session_authority_guard_);
                    auto iter = authority_map_.find(info.GetId());
                    if (iter != authority_map_.end()) {
                        if (iter->second.GetClock().to_int64_t() == info.GetClock().to_int64_t()) {
                            authority_map_.erase(iter);
                        }
                    }
                }, std::chrono::milliseconds(3000));
            
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(
                IntermediateServerReceivePacket::kReactSessionAuthorityInfo));
            out << login_server_uuid;
            out << user_uuid;
            out.MakePacketHead();
            session.AsyncSend(out, false, false);
        }
    );
}

void LobbyServer::Run()
{
    auto nio = GetNioServer();
    if (nio) {
        GetNioServer()->Run();
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
                        nio->PrintCurrentSessionQueue();
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
    auto nio = GetNioServer();
    if (nio) {
        nio->Stop();
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
    auto conn = this->GetNioServer()->GetChannel().GetConnection(intermediate_server);
    if (conn.IsValid() && conn.IsOpen() == false) {
        if (conn.Connect(opt_ip->c_str(), *opt_port) == false) {
            std::stringstream ss;
            Clock clock;
            ss << '[' << Calendar::DateTime(clock) << "] connect to intermediate server(" << *opt_ip << ":" << *opt_port << ") fail \n";
            std::cout << ss.str();
        } else {
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kRegisterRemoteServer));
            out << this_info_;
            out.MakePacketHead();
            GetNioServer()->GetChannel().GetConnection(intermediate_server).Send(out);
        }
    }
}

void LobbyServer::OnActiveClient(UE4Client& client)
{
    client.SetState(ClientState::kNotConfirm);
    GetNioServer()->CreateTimer(
        [uuid = client.GetUUID(), this]() 
        {
            auto client = this->GetClient(uuid.ToString());
            if (client && client->GetState() == ClientState::kNotConfirm) {
                this->CloseClient(uuid.ToString());
            }
        }, std::chrono::milliseconds(1000));
}

void LobbyServer::OnCloseClient(UE4Client& client)
{
    // 이 경우에는 LoginServer로 Packet을 보내서 접속중인 연결이 아님을 통지
    if (client.GetState() == ClientState::kConfirm) {
        UE4OutPacket out;
        out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kNotifyUserLogout));
        out.WriteInt32(client.GetAccid());
        out.MakePacketHead();
        GetNioServer()->GetChannel().GetConnection(intermediate_server).Send(out);
    }
}

void LobbyServer::OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet)
{
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
        CloseClient(client->GetUUID().ToString());
    }
}

void LobbyServer::HandleConfirmRequest(UE4Client& client, NioInPacket& in_packet)
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
            result = true;
        }
    }
    if (result) {
        SendCharacterList(client);
    } else {
        // 실패 보내기
    }
    
}

void LobbyServer::HandleCharacterListRequest(UE4Client& client, NioInPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm)
        return;
    SendCharacterList(client);
}

void LobbyServer::HandleCharacterCreateRequest(UE4Client& client, NioInPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm)
        return;
}

void LobbyServer::HandleCharacterDeleteRequest(UE4Client& client, NioInPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm)
        return;
}

void LobbyServer::HandleCharacterSelectRequest(UE4Client& client, NioInPacket& in_packet)
{
    if (client.GetState() != ClientState::kConfirm)
        return;
}

void LobbyServer::SendCharacterList(UE4Client& client)
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
    if (rs->Next()) {
        cids_.push_back(cid);
    }
    rs->Close();
    ps->Close();

    UE4OutPacket out;
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kCharacterListNotify));
    out.WriteInt32(cids_.size());
    if (cids_.size() > 0) 
    {
        for (size_t i = 0; i < cids_.size(); ++i)
        {
            ps = con->GetPreparedStatement();
            ps->PrepareStatement(L"select characters.*, inventoryitems.itemid from characters, inventoryitems where characters.cid = inventoryitems.cid and inventoryitems.equipped = 1 and characters.cid = ?;");
            ps->SetInt32(1, &cids_[i]);
            
            rs = ps->Execute();

            PacketGenerateHelper::LobbyCharacterInfo info;

            int32_t db_cid;
            int32_t accid;
            int32_t itemid;
            rs->BindInt32(1, &db_cid);
            rs->BindInt32(2, &accid);
            rs->BindInt32(3, &info.slot);
            rs->BindString(4, info.name, 100);
            rs->BindInt32(5, &info.level);
            rs->BindInt32(6, &info.str);
            rs->BindInt32(7, &info.dex);
            rs->BindInt32(8, &info.intel);
            rs->BindInt32(9, &info.job);
            rs->BindInt32(10, &info.face);
            rs->BindInt32(11, &info.hair);
            rs->BindInt32(12, &info.gold);
            rs->BindInt32(13, &info.zone);
            rs->BindFloat32(14, &info.x);
            rs->BindFloat32(15, &info.y);
            rs->BindFloat32(16, &info.z);
            rs->BindInt32(17, &info.gender);
            rs->BindInt32(18, &itemid);
            int count = 0;
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
                    case 4:
                        info.sub_weapon_itemid = itemid;
                        break;
                }
            }
            PacketGenerateHelper::WriteLobbyCharacterInfo(out, info);
            rs->Close();
            ps->Close();
        }
    }
    out.MakePacketHead();
    client.GetSession()->AsyncSend(out, false, true);
}