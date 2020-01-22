#include "ZoneServer.hpp"
#include "ZoneEventHandler.hpp"
#include "Constants/ServerConstants.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "../InterServerOpcode.hpp"
#include "../NetworkOpcode.h"
#include "Game/Character/Character.hpp"
#include <iostream>
#include <sstream>


ZoneServer::ZoneServer()
{
}

ZoneServer::~ZoneServer()
{
}

void ZoneServer::Initialize()
{
    NioServerBuilder builder;
    builder.SetPort(ServerConstant.port)
        .SetMaxConnection(ServerConstant.max_connection)
        .SetNioThreadCount(ServerConstant.num_io_worker_)
        .SetNioInternalBufferSize(2048)
        .SetNioPacketCipher(std::shared_ptr<NioCipher>(new UE4PacketCipher()))
        .SetNioEventHandler(std::shared_ptr<NioEventHandler>(new ZoneServerEventHandler()));
    this->SetNioServer(builder.Build());

    GetNioServer()->GetChannel().MakeConnection(intermediate_server);
    GetNioServer()->GetChannel().GetConnection(intermediate_server).BindFunction(
        static_cast<int16_t>(IntermediateServerSendPacket::kReactUserMigation),
        [this](NioSession& session, InputStream& stream) {
            //std::string user_uuid = stream.ReadString();
            //std::string lobby_ip = stream.ReadString();
            //uint16_t looby_port = stream.ReadInt16();

            //auto client = this->GetClient(user_uuid);
            //if (client) {
            //    UE4OutPacket out;
            //    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kLoginResult));
            //    out.WriteInt32(true);
            //    out << lobby_ip;
            //    out << looby_port;
            //    out.MakePacketHead();
            //    client->GetSession()->AsyncSend(out, false, true);
            //}
        }
    );
    game_zone_.emplace("Town", std::make_shared<Zone>());
}

void ZoneServer::Run()
{
    auto nio = GetNioServer();
    if (nio) {
        GetNioServer()->Run();
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
                        nio->PrintCurrentSessionQueue();
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
    auto nio = GetNioServer();
    if (nio) {
        nio->Stop();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "nio server is nullptr");
    }
}

void ZoneServer::ConnectChannel()
{
    auto conn = this->GetNioServer()->GetChannel().GetConnection(intermediate_server);
    if (conn.IsValid() && conn.IsOpen() == false) {
        if (conn.Connect(ServerConstant.intermediate_server_ip.c_str(), ServerConstant.intermediate_server_port) == false) {
            std::stringstream ss;
            Clock clock;
            ss << '[' << Calendar::DateTime(clock) << "] connect to intermediate server(" 
                << ServerConstant.intermediate_server_ip << ":" << ServerConstant.intermediate_server_port << ") fail \n";
            std::cout << ss.str();
        }
    }
}

void ZoneServer::OnActiveClient(UE4Client& client)
{
    client.SetState(ClientState::kNotConfirm);
    GetNioServer()->CreateTimer([uuid = client.GetUUID(), this]() {
        auto client = this->GetClient(uuid.ToString());
        if (client && client->GetState() == ClientState::kNotConfirm) {
            this->CloseClient(uuid.ToString());
        }
    }, std::chrono::milliseconds(1000));
}

void ZoneServer::OnCloseClient(UE4Client& client)
{
    if (client.GetState() != ClientState::kConfirm) {
        UE4OutPacket out;
        out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kNotifyUserLogout));
        out.WriteInt32(client.GetAccid());
        out.MakePacketHead();
        GetNioServer()->GetChannel().GetConnection(intermediate_server).Send(out);
    }
}

void ZoneServer::OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet)
{
    try {
        int16_t opcode = in_packet->ReadInt16();
        switch (opcode) {
            case static_cast<int16_t>(ENetworkCSOpcode::kZoneConrifmRequest):
                HandleConfirmRequest(*client, *in_packet);
                break;
            case static_cast<int16_t>(ENetworkCSOpcode::kNotifyCurrentChrPosition):
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

void ZoneServer::HandleConfirmRequest(UE4Client& client, NioInPacket& in_packet)
{
    std::string id = in_packet.ReadString();
    const auto& ip = client.GetSession()->GetRemoteAddress();
    {
        std::shared_lock lock(session_authority_guard_);
        auto iter = authority_map_.find(id);
        if (iter == authority_map_.end())
            return;
        if (iter->second.GetIp() != ip)
            return;
        client.SetState(ClientState::kConfirm);
        client.SetAccid(iter->second.GetAccid());
        client.SetCid(iter->second.GetCid());
    }
    std::shared_ptr<Character> chr = std::make_shared<Character>(client.GetAccid(), client.GetCid());
    bool init = false;
    try {
        chr->Initialize();
        init = true;
    }
    catch (const std::exception & e) {
        std::cout << e.what();
    }
    if (init == false) {
        CloseClient(client.GetUUID().ToString());
    } else {
        game_zone_["Town"]->SpawnPlayer(chr);
    }
}