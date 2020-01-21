#include "LobbyServer.hpp"
#include "LobbyEventHandler.hpp"
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
    auto opt_port = reader_.GetInt32("LOBBY_SERVER_PORT");
    auto opt_max_conn = reader_.GetInt32("LOBBY_SERVER_MAX_CONNECTION");
    auto opt_worker_size = reader_.GetInt32("LOBBY_SERVER_NUM_IO_WORKER");
    auto opt_channel_port = reader_.GetInt32("LOBBY_SERVER_CHANNEL_PORT");
    auto opt_channel_max_conn = reader_.GetInt32("LOBBY_SERVER_CHANNEL_MAX_CONNECTION");
    auto opt_lobby_ip = reader_.GetString("LOBBY_SERVER_IP");
    auto opt_lobby_port = reader_.GetInt32("LOBBY_SERVER_PORT");
    if (!opt_odbc || !opt_db_id || !opt_db_pw || !opt_port || !opt_max_conn ||
        !opt_worker_size || !opt_channel_port || *opt_channel_max_conn ||
        !opt_lobby_ip || !opt_lobby_port)
    {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
    database_ = std::make_unique<ODBCDriver>();
    database_->Initialize();

    GetNioServer()->GetChannel().MakeConnection(intermediate_server);
    GetNioServer()->GetChannel().GetConnection(intermediate_server).BindFunction(
        static_cast<int16_t>(IntermediateServerSendPacket::kNotifySessionAuthorityInfo),
        [this](NioSession& session, InputStream& input) {
            std::string login_server_uuid = input.ReadString();
            std::string user_uuid = input.ReadString();
            SessionAuthorityInfo info;
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
    auto opt_ip = reader_.GetString("INTERMEDIATE_SERVER_CHANNEL_IP");
    auto opt_port = reader_.GetInt32("INTERMEDIATE_SERVER_CHANNEL_PORT");
    if (!opt_ip || !opt_port) {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
    auto conn = this->GetNioServer()->GetChannel().GetConnection(intermediate_server_conn);
    if (conn.IsValid() && conn.IsOpen() == false) {
        if (conn.Connect(opt_ip->c_str(), *opt_port) == false) {
            std::stringstream ss;
            Clock clock;
            ss << '[' << Calendar::DateTime(clock) << "] connect to intermediate server(" << *opt_ip << ":" << *opt_port << ") fail \n";
            std::cout << ss.str();
        }
    }
}

void LobbyServer::OnActiveClient(UE4Client& client)
{
}

void LobbyServer::OnCloseClient(UE4Client& client)
{
}

void LobbyServer::OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet)
{
}
