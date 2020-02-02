#include "ZoneServer.hpp"
#include "ServerConstants.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "UE4DevelopmentLibrary/Server/UE4PacketCipher.hpp"
#include "UE4DevelopmentLibrary/Server/UE4OutPacket.hpp"
#include "UE4DevelopmentLibrary/Server/RemoteSessionInfo.hpp"
#include "UE4DevelopmentLibrary/Server/RemoteServerInfo.hpp"
#include "UE4DevelopmentLibrary/Server/UE4EventHandler.hpp"
#include "Model/Character/Character.hpp"
#include "../InterServerOpcode.hpp"
#include "../NetworkOpcode.h"
#include "GameConstants.hpp"
#include "System/ZoneSystem.hpp"
#include <iostream>
#include <sstream>


ZoneServer* ZoneServer::static_instance_ = nullptr;

ZoneServer::ZoneServer()
    : print_log_(true)
{
    static_instance_ = this;
}

ZoneServer::~ZoneServer()
{
}

void ZoneServer::Initialize()
{
    io_server_.emplace(ServerConstant.max_connection, ServerConstant.port, ServerConstant.num_io_worker_);
    io_server_->Initialize();
    session_ = std::make_shared<IntermediateSession>(io_server_->GetContext());

    IntermediateSession::Handler::BindFunction(
        static_cast<int16_t>(IntermediateServerSendPacket::kNotifySessionAuthorityInfo),
        [this](IntermediateSession& session, IntermediateSession::InPacket& input) {
            std::string lobby_server_uuid = input.ReadString();
            std::string user_uuid = input.ReadString();
            RemoteSessionInfo info;
            input >> info;

            {
                std::lock_guard lock(session_authority_guard_);
                authority_map_.emplace(info.GetId(), info);
            }
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(
                IntermediateServerReceivePacket::kReactSessionAuthorityInfo));
            out << lobby_server_uuid;
            out << user_uuid;
            out.MakePacketHead();
            session.Send(out, false, false);

            //nio_server_->CreateTimer(
            //    [this, info]() {
            //        std::lock_guard lock(session_authority_guard_);
            //        auto iter = authority_map_.find(info.GetId());
            //        if (iter != authority_map_.end()) {
            //            if (iter->second.GetClock().to_int64_t() == info.GetClock().to_int64_t()) {
            //                //authority_map_.erase(iter);
            //            }
            //        }
            //    }, std::chrono::milliseconds(3000));
        }
    );
    town_ = ZoneSystem::CreateNewInstance(100);
    town_->SetState(Zone::State::kActive);
}

void ZoneServer::Run()
{
    if (io_server_) {
        io_server_->Run();
        ConnectChannel();
        std::cout << "Input Command\n\
[1] : PrintServerState\n\
[2] : Retry connect IntermediateServer\n\
[q] : Server Stop\n";
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
                        ConnectChannel();
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

void ZoneServer::Stop()
{
    if (io_server_) {
        io_server_->Stop();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "nio server is nullptr");
    }
}

void ZoneServer::ConnectChannel()
{
    if (session_ && session_->IsOpen() == false) {
        if (session_->SyncConnect(ServerConstant.intermediate_server_ip.c_str(), ServerConstant.intermediate_server_port) == false) {
            std::stringstream ss;
            Clock clock;
            ss << '[' << Calendar::DateTime(clock) << "] connect to intermediate server(" 
                << ServerConstant.intermediate_server_ip << ":" << ServerConstant.intermediate_server_port << ") fail \n";
            std::cout << ss.str();
        } else {
            session_->OnConnect();

            RemoteServerInfo info;
            info.SetServerType(ServerType::kZoneServer);
            info.SetIP(ServerConstant.ip);
            info.SetPort(ServerConstant.port);
            info.SetCurrentConnection(0);
            info.SetMaxConnection(ServerConstant.max_connection);

            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kRegisterRemoteServer));
            out << info;
            out.MakePacketHead();
            session_->Send(out, false, false);
        }
    }
}

void ZoneServer::OnActive(Session& session)
{
    std::shared_ptr<Client> client = std::make_shared<Client>(session.shared_from_this());
    const auto& uuid = client->GetUUID();
    {
        std::unique_lock lock(client_storage_guard_);
        client->SetState(ClientState::kNotConfirm);
        client_storage_.emplace(client->GetUUID().ToString(), client);
        session.SetClientKey(client->GetUUID().ToString());
    }
    if (print_log_) {
        std::stringstream ss;
        Clock clock;
        ss << "[" << Calendar::DateTime(clock) << "] "
            << client->GetSession()->GetRemoteAddress() << " connect...\n";
        std::cout << ss.str();
    }
    //GetNioServer()->CreateTimer([uuid = client.GetUUID(), this]() {
    //    auto client = this->GetClient(uuid.ToString());
    //    if (client && client->GetState() == ClientState::kNotConfirm) {
    //        this->CloseClient(uuid.ToString());
    //    }
    //}, std::chrono::milliseconds(1000));
}
\
void ZoneServer::OnClose(Session& session)
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
    if (client)
    {
        if (client->GetState() == ClientState::kConfirm) {
            auto chr = client->GetCharacter();
            if (chr) {
                std::shared_ptr<Zone> zone = chr->GetZone();
                if (zone) {
                    zone->RemoveCharacter(chr->GetObjectId());
                }
                chr->SetZone(nullptr);
                client->SetCharacter(nullptr);
            }
        } else if (client->GetState() == ClientState::kNotConfirm) {

        }
        if (print_log_) {
            std::stringstream ss;
            Clock clock;
            ss << "[" << Calendar::DateTime(clock) << "] "
                << client->GetSession()->GetRemoteAddress() << " close...\n";
            std::cout << ss.str();
        }
        /*UE4OutPacket out;
        out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kNotifyUserLogout));
        out.WriteInt32(client.GetAccid());
        out.MakePacketHead();
        GetNioServer()->GetChannel().GetConnection(intermediate_server).Send(out);*/
    }
}
void ZoneServer::OnError(int ec, const char* message)
{
}
void ZoneServer::ProcessPacket(Session& session, const std::shared_ptr<Session::InPacket>& in_packet)
{
    auto client = this->GetClient(session.GetClientKey());
    if (!client) {
        std::stringstream ss;
        ss << "ip[" << session.GetRemoteAddress() << "]'s clinet is nullptr";
        throw StackTraceException(ExceptionType::kNullPointer, ss.str().c_str());
    }
    if (print_log_) {
        std::stringstream ss;
        ss << "Packet Dump: " << in_packet->GetDebugString() << '\n';
        std::cout << ss.str();
    }
    in_packet->SetAccesOffset(2);
    try {
        int16_t opcode = in_packet->ReadInt16();
        switch (static_cast<ENetworkCSOpcode>(opcode)) {
            case ENetworkCSOpcode::kZoneConrifmRequest:
                HandleConfirmRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kNotifyCurrentChrPosition:
                HandleChrPositionNotify(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kNotifyMonsterAction:
                HandleMonsterActionNotify(*client, *in_packet);
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

std::shared_ptr<Client> ZoneServer::GetClient(const std::string& uuid) const
{
    std::shared_lock lock(client_storage_guard_);
    auto iter = client_storage_.find(uuid);
    if (iter != client_storage_.end()) {
        return iter->second;
    }
    return nullptr;
}

void ZoneServer::HandleConfirmRequest(Client& client, Session::InPacket& in_packet)
{
    std::string id = in_packet.ReadString();
    const auto& ip = client.GetSession()->GetRemoteAddress();
    {
        bool result{ false };
        RemoteSessionInfo info;
        {
            std::lock_guard lock(session_authority_guard_);
            auto iter = authority_map_.find(id);
            if (iter != authority_map_.end() && iter->second.GetIp() == ip) {
                result = true;
                info = iter->second;
                authority_map_.erase(iter);
            }
        }
        if (result) {
            client.SetState(ClientState::kConfirm);
            client.SetAccid(info.GetAccid());
            client.SetCid(info.GetCid());
        } else {
            client.Close();
            return;
        }
    }
    std::shared_ptr<Character> chr = std::make_shared<Character>(client.GetAccid(), client.GetCid());
    bool result{ false };
    try {
        chr->Initialize(ODBCConnectionPool::Instance().GetConnection());
        chr->SetWeakClient(client.shared_from_this());
        client.SetCharacter(chr);
        result = true;
    }
    catch (const std::exception & e) {
        std::cout << e.what();
    }
    if (result == false) {
        client.Close();
    } else {
        chr->SetZone(town_);
        Location spawn = town_->GetPlayerSpawn();
        chr->GetLocation().x = spawn.x;
        chr->GetLocation().y = spawn.y;
        chr->GetLocation().z = spawn.z;
        chr->GetRotation().x = 0.0;
        chr->GetRotation().y = 0.0;
        chr->GetRotation().z = 0.0;
        town_->SpawnCharacter(chr);
    }
}

void ZoneServer::HandleChrPositionNotify(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != kConfirm) {
        return;
    }
    auto chr = client.GetCharacter();
    if (chr) {
        int state = in_packet.ReadInt32();
        chr->GetLocation().Read(in_packet);
        chr->GetRotation().Read(in_packet);
        chr->GetZone()->UpdateCharacterPosition(*chr, state);
    }
}

void ZoneServer::HandleMonsterActionNotify(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != kConfirm) {
        return;
    }
    auto chr = client.GetCharacter();
    if (chr) {
        int64_t object_id = in_packet.ReadInt64();
        int state = in_packet.ReadInt32();
        Location lo; lo.Read(in_packet);
        Rotation ro; ro.Read(in_packet);
        chr->GetZone()->UpdateMonsterAction(chr->GetObjectId(), object_id, state, lo, ro);
    }
}
