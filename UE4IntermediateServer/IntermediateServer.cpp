#include "IntermediateServer.hpp"
#include "IntermediateEventHandler.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "../InterServerOpcode.hpp"
#include <sstream>
#include <iostream>


IntermediateServer::IntermediateServer()
{
}

void IntermediateServer::Initialize()
{
    reader_.Initialize("ServerSetting.ini");
    auto opt_odbc = reader_.GetString("ODBC");
    auto opt_db_id = reader_.GetString("DB_ID");
    auto opt_db_pw = reader_.GetString("DB_PW");
    auto opt_port = reader_.GetInt32("INTERMEDIATE_SERVER_PORT");
    auto opt_max_conn = reader_.GetInt32("INTERMEDIATE_SERVER_MAX_CONNECTION");
    auto opt_worker_size = reader_.GetInt32("INTERMEDIATE_SERVER_NUM_IO_WORKER");
    if (!opt_odbc || !opt_db_id || !opt_db_pw || !opt_port || !opt_max_conn || !opt_worker_size) {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
    NioServerBuilder builder;
    builder.SetPort(*opt_port)
        .SetMaxConnection(*opt_max_conn)
        .SetNioThreadCount(*opt_worker_size)
        .SetNioInternalBufferSize(2048)
        .SetNioPacketCipher(std::shared_ptr<NioCipher>(new UE4PacketCipher()))
        .SetNioEventHandler(std::shared_ptr<NioEventHandler>(new IntermediateEventHandler()));
    this->SetNioServer(builder.Build());

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
}

void IntermediateServer::Run()
{
    auto nio = GetNioServer();
    if (nio) {
        GetNioServer()->Run();
        ConnectChannel();
        std::cout << "Input Command\n[1] : PrintServerState\n[q] : Server Stop\n";;
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

void IntermediateServer::Stop()
{
    auto nio = GetNioServer();
    if (nio) {
        nio->Stop();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "nio server is nullptr");
    }
}

void IntermediateServer::ConnectChannel()
{
}

void IntermediateServer::OnActiveClient(UE4Client& client)
{
}

void IntermediateServer::OnCloseClient(UE4Client& client)
{
    std::unique_lock lock(data_guard_);
    server_map_.erase(client.GetUUID().ToString());
}

void IntermediateServer::OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet)
{
    try {
        int16_t opcode = in_packet->ReadInt16();
        switch (static_cast<IntermediateServerReceivePacket>(opcode)) {
            case IntermediateServerReceivePacket::kRegisterRemoteServer:
                HandleRegisterServer(client, *in_packet);
                break;
            case IntermediateServerReceivePacket::kUpdateServerConnection:
                UpdateServerConnection(*client, *in_packet);
                break;
            case IntermediateServerReceivePacket::kRequestUserMigration:
                HandleRequestUserMigration(*client, *in_packet);
                break;
            case IntermediateServerReceivePacket::kReactSessionAuthorityInfo:
                HandleReactSessionAuthorityInfo(*client, *in_packet);
                break;
            case IntermediateServerReceivePacket::kNotifyUserLogout:
                HandleNotifiyUserLogout(*client, *in_packet);
                break;
        }
    } catch (const std::exception & e) {
        std::stringstream ss;
        Clock clock;
        std::string date = Calendar::DateTime(clock);
        ss << "[" << date << "] Exception: " << e.what() << '\n';
        date = ss.str();
        std::cout << date;
    }
}

void IntermediateServer::HandleRegisterServer(const shared_ptr<UE4Client>& client, NioInPacket& packet)
{
    RemoteServerInfo info;
    packet >> info;
    info.SetWeak(client);
    
    std::unique_lock lock(data_guard_);
    server_map_.emplace(client->GetUUID().ToString(), info);
}

void IntermediateServer::UpdateServerConnection(UE4Client& client, NioInPacket& packet)
{
    int64_t current_connection = packet.ReadInt64();
    std::shared_lock lock(data_guard_);
    auto iter = server_map_.find(client.GetUUID().ToString());
    if (iter != server_map_.end()) {
        iter->second.SetCurrentConnection(current_connection);
    }
}

void IntermediateServer::HandleRequestUserMigration(UE4Client& client, NioInPacket& packet)
{
    int16_t type = packet.ReadInt16();
    std::string user_uuid = packet.ReadString();

    RemoteSessionInfo authority_info;
    packet >> authority_info;

    std::vector<RemoteServerInfo> servers;
    {
        std::shared_lock lock(data_guard_);
        for (const auto& pair : server_map_) {
            if (type == static_cast<int16_t>(pair.second.GetServerType())) {
                servers.push_back(pair.second);
            }
        }
    } // ratio에 mutex를 써가면서 동기화할 필요는 없다고 봄
    
    std::sort(servers.begin(), servers.end(),
        [](const RemoteServerInfo& lhs, const RemoteServerInfo& rhs) {
            return lhs.GetCurrentCapacityRatio() < rhs.GetCurrentCapacityRatio();
        });

    for (size_t i = 0; i < servers.size(); ++i) {
        auto shared = servers[i].GetWeak().lock();
        if (shared) {
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(
                IntermediateServerSendPacket::kNotifySessionAuthorityInfo));
            out << client.GetUUID().ToString();
            out << user_uuid;
            out << authority_info;
            out.MakePacketHead();
            shared->GetSession()->AsyncSend(out, false, false);
            break;
        }
    }
}

void IntermediateServer::HandleReactSessionAuthorityInfo(UE4Client& client, NioInPacket& packet)
{
    std::string server_uuid = packet.ReadString();
    std::string user_uuid = packet.ReadString();
    {
        std::shared_lock lock(data_guard_);
        auto login = server_map_.find(server_uuid);
        auto lobby = server_map_.find(client.GetUUID().ToString());
        if(login != server_map_.end() && lobby != server_map_.end()) {
            auto client = login->second.GetWeak().lock();
            if (client) {
                UE4OutPacket out;
                out.WriteInt16(static_cast<int16_t>(IntermediateServerSendPacket::kReactUserMigation));
                out << user_uuid;
                out << lobby->second.GetIP();
                out << lobby->second.GetPort();
                out.MakePacketHead();
                client->GetSession()->AsyncSend(out, false, false);
            }
        }
    }
}

void IntermediateServer::HandleNotifiyUserLogout(UE4Client& client, NioInPacket& packet)
{
    int32_t accid = packet.ReadInt32();
    auto con = ODBCConnectionPool::Instance().GetConnection();
    auto ps = con->GetPreparedStatement();
    ps->PrepareStatement(TEXT("update accounts set loggedin = 0 where accid = ?"));
    ps->SetInt32(1, &accid);
    ps->ExecuteUpdate();
}