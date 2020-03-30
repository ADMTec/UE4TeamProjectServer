#include "IntermediateServer.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
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
    io_server_.emplace(*opt_max_conn, *opt_port, *opt_worker_size);
    io_server_->Initialize();
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
    if (io_server_) {
        io_server_->Run();
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
    if (io_server_) {
        io_server_->Stop();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "nio server is nullptr");
    }
}

void IntermediateServer::ConnectChannel()
{
}

void IntermediateServer::OnActive(Session& session)
{
    std::cout << "Server[" << session.GetRemoteAddress() << "] is Active\n";
    auto client = std::make_shared<Client>(session.shared_from_this());
    std::unique_lock lock(client_storage_guard_);
    client_storage_.emplace(client->GetUUID().ToString(), client);
    session.SetClientKey(client->GetUUID().ToString());
}

void IntermediateServer::OnClose(Session& session)
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
        client->Close();
        std::unique_lock lock(server_map__guard_);
        server_map_.erase(client->GetUUID().ToString());
    }
}

void IntermediateServer::OnError(int ec, const char* msg)
{
}

std::shared_ptr<Client> IntermediateServer::GetClient(const std::string& uuid) const
{
    std::shared_lock lock(client_storage_guard_);
    auto iter = client_storage_.find(uuid);
    if (iter != client_storage_.end()) {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<Client> IntermediateServer::GetServerLoadBalance(ServerType type) const
{
    double ratio = 1.1f;
    std::string uuid;
    std::shared_lock lock(server_map__guard_);
    for (const auto& pair : server_map_) {
        if (type == pair.second.GetServerType()) {
            double server_ratio = pair.second.GetCurrentCapacityRatio();
            if (ratio > server_ratio) {
                ratio = server_ratio;
                uuid = pair.first;
            }
        }
    }
    return GetClient(uuid);
}

void IntermediateServer::ProcessPacket(Session& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    auto client = this->GetClient(session.GetClientKey());
    if (!client) {
        throw StackTraceException(ExceptionType::kNullPointer, "client is nullptr");
    }
    in_packet->SetAccesOffset(2);
    try {
        int16_t opcode = in_packet->ReadInt16();
        switch (static_cast<IntermediateServerReceivePacket>(opcode)) {
            case IntermediateServerReceivePacket::kRegisterRemoteServer:
                HandleRegisterServer(*client, *in_packet);
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

void IntermediateServer::HandleRegisterServer(Client& client, Session::InPacket& packet)
{
    RemoteServerInfo info;
    packet >> info;
    
    std::unique_lock lock(server_map__guard_);
    server_map_.emplace(client.GetUUID().ToString(), info);
}

void IntermediateServer::UpdateServerConnection(Client& client, Session::InPacket& packet)
{
    int64_t current_connection = packet.ReadInt64();
    std::shared_lock lock(server_map__guard_);
    auto iter = server_map_.find(client.GetUUID().ToString());
    if (iter != server_map_.end()) {
        iter->second.SetCurrentConnection(current_connection);
    }
}

void IntermediateServer::HandleRequestUserMigration(Client& client, Session::InPacket& packet)
{
    int16_t type = packet.ReadInt16();
    std::string user_uuid = packet.ReadString();

    RemoteSessionInfo authority_info;
    packet >> authority_info;


    if (type < static_cast<int16_t>(ServerType::kLoginServer) || type >= static_cast<int16_t>(ServerType::kCount)) {
        return;
    }

    auto target_server = GetServerLoadBalance(static_cast<ServerType>(type));
    if (target_server)
    {
        UE4OutPacket out;
        out.WriteInt16(static_cast<int16_t>(
            IntermediateServerSendPacket::kNotifySessionAuthorityInfo));
        out << client.GetUUID().ToString();
        out << user_uuid;
        out << authority_info;
        out.MakePacketHead();
        target_server->GetSession()->Send(out, false, false);
    }
}

void IntermediateServer::HandleReactSessionAuthorityInfo(Client& client, Session::InPacket& packet)
{
    std::string server_uuid = packet.ReadString();
    std::string user_uuid = packet.ReadString();
    {
        std::shared_lock lock(server_map__guard_);
        auto origin = server_map_.find(server_uuid);
        auto dest = server_map_.find(client.GetUUID().ToString());
        if(origin != server_map_.end() && dest != server_map_.end()) {
            auto client = GetClient(origin->first);
            if (client) {
                UE4OutPacket out;
                out.WriteInt16(static_cast<int16_t>(IntermediateServerSendPacket::kReactUserMigation));
                out << user_uuid;
                out << dest->second.GetIP();
                out << dest->second.GetPort();
                out.MakePacketHead();
                client->GetSession()->Send(out, false, false);
            }
        }
    }
}

void IntermediateServer::HandleNotifiyUserLogout(Client& client, Session::InPacket& packet)
{
    int32_t accid = packet.ReadInt32();
    auto con = ODBCConnectionPool::Instance().GetConnection();
    auto ps = con->GetPreparedStatement();
    ps->PrepareStatement(TEXT("update accounts set loggedin = 0 where accid = ?"));
    ps->SetInt32(1, &accid);
    ps->ExecuteUpdate();
}