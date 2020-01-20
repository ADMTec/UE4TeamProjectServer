#include "LoginServer.hpp"
#include "LoginServerEventHandler.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "../InterServerOpcode.hpp"
#include "../NetworkOpcode.h"
#include <sstream>
#include <iostream>


LoginServer::LoginServer()
{
}

LoginServer::~LoginServer()
{
}

void LoginServer::Initialize()
{
    reader_.Initialize("ServerSetting.ini");
    auto opt_odbc = reader_.GetString("ODBC");
    auto opt_db_id = reader_.GetString("DB_ID");
    auto opt_db_pw = reader_.GetString("DB_PW");
    auto opt_port = reader_.GetInt32("LOGIN_SERVER_PORT");
    auto opt_max_conn = reader_.GetInt32("LOGIN_SERVER_MAX_CONNECTION");
    auto opt_worker_size = reader_.GetInt32("LOGIN_SERVER_NUM_IO_WORKER");
    auto opt_channel_port = reader_.GetInt32("LOGIN_SERVER_CHANNEL_PORT");
    auto opt_channel_max_conn = reader_.GetInt32("LOGIN_SERVER_CHANNEL_MAX_CONNECTION");
    auto opt_lobby_ip = reader_.GetString("LOBBY_SERVER_IP");
    auto opt_lobby_port = reader_.GetInt32("LOBBY_SERVER_PORT");
    if(!opt_odbc || !opt_db_id || !opt_db_pw || !opt_port || !opt_max_conn || 
        !opt_worker_size || !opt_channel_port || *opt_channel_max_conn || 
        !opt_lobby_ip || !opt_lobby_port)
    {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
    database_ = std::make_unique<ODBCDriver>();
    database_->Initialize();
#ifdef _UNICODE
    std::string str1 = std::string(*opt_odbc);
    std::string str2 = std::string(*opt_db_id);
    std::string str3 = std::string(*opt_db_pw);
    odbc_ = std::wstring(str1.begin(), str1.end());
    db_id_ = std::wstring(str2.begin(), str2.end());
    db_pw_ = std::wstring(str3.begin(), str3.end());
#else
    odbc_ = *opt_odbc;
    db_id_ = *opt_db_id;
    db_pw_ = *opt_db_pw;
#endif
    NioServerBuilder builder;
    builder.SetPort(*opt_port)
        .SetMaxConnection(10)
        .SetNioThreadCount(2)
        .SetNioInternalBufferSize(2048)
        .SetNioPacketCipher(std::shared_ptr<NioCipher>(new UE4PacketCipher()))
        .SetNioEventHandler(std::shared_ptr<NioEventHandler>(new LoginServerEventHandler()));
    this->SetNioServer(builder.Build());

    this->GetNIoServer()->GetChannel().MakeReceiver(
        LoginServer::channel_id, 
        *opt_channel_port,
        *opt_channel_max_conn);
    this->GetNIoServer()->GetChannel().GetReceiver(LoginServer::channel_id).BindFunction(
        static_cast<int16_t>(ToLoginServer::UserCloseNotify),
        [this](NioSession& session, InputStream& input) {
            std::string id = input.ReadString();
            std::lock_guard lock(connected_id_set_garud_);
            connected_id_set_.erase(id);
        }
    );
    std::string lobby_ip(*opt_lobby_ip);
    uint16_t lobby_port = *opt_lobby_port;
    this->GetNIoServer()->GetChannel().MakeConnection(lobby_channel_id);
    GetNIoServer()->GetChannel().GetConnection(lobby_channel_id).BindFunction(
        static_cast<int16_t>(ToLoginServer::UserMigrationNotifyAck),
        [this, lobby_ip, lobby_port](NioSession& session, InputStream& input) {
            std::string client_uuid;
            input >> client_uuid;
            auto client = this->GetClient(client_uuid);
            if (client) {
                UE4OutPacket out;
                out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kLoginResult));
                out.WriteInt32(true);
                out.WriteString(lobby_ip);
                out.WriteInt16(lobby_port);
                out.MakePacketHead();
                client->GetSession()->AsyncSend(out, false, true);
            }
        });
}

void LoginServer::Run()
{
    GetNIoServer()->Run();
    ConnectChannel();
    std::cout << "Input Command\n[1] : PrintServerState \n[q] : Server Stop\n";;
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

void LoginServer::Stop()
{
    GetNIoServer()->Stop();
}

void LoginServer::ConnectChannel()
{
    auto opt_ip = reader_.GetString("LOBBY_SERVER_CHANNEL_IP");
    auto opt_port = reader_.GetInt32("LOBBY_SERVER_CHANNEL_PORT");
    if(!opt_ip || !opt_port) {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
    auto conn = this->GetNIoServer()->GetChannel().GetConnection(lobby_channel_id);
    if (conn.IsValid()) {
        int64_t reconn_time = 60 * 1000;
        Clock now;
        while (Clock() - now > reconn_time)
        {
            if (conn.Connect(opt_ip->c_str(), *opt_port)) {
                break;
            }
        }
    }
}

void LoginServer::OnActiveClient(UE4Client& client)
{
    std::stringstream ss;
    Clock clock;
    ss << "[" << Calendar::DateTime(clock) << "] " 
        << client.GetSession()->GetRemoteAddress() << " connect...\n";
    std::cout << ss.str();
}

void LoginServer::OnCloseClient(UE4Client& client)
{
    std::stringstream ss;
    Clock clock;
    ss << "[" << Calendar::DateTime(clock) << "] "
        << client.GetSession()->GetRemoteAddress() << " close...\n";
    std::cout << ss.str();
}

void LoginServer::OnProcessPacket(const std::shared_ptr<UE4Client>& client, NioInPacket& in_packet)
{
    try {
        std::stringstream ss;
        ss << "LoginServer InPacket: " << in_packet.GetDebugString() << '\n';
        std::cout << ss.str();

        int16_t opcode = in_packet.ReadInt16();
        switch (opcode)
        {
            case static_cast<int16_t>(ENetworkCSOpcode::kCreateAccountRequest):
                break;
            case static_cast<int16_t>(ENetworkCSOpcode::kLoginRequest):
                break;
        }
    }
    catch (const std::exception & e) {
        std::stringstream ss;
        Clock clock;
        std::string date = Calendar::DateTime(clock);
        ss << "[" << date << "] Exception: " << e.what() << '\n';
        date = ss.str();
        std::cout << date;
        CloseClient(client->GetUUID().ToString());
    }
}

void LoginServer::HandleCreateAccountRquest(UE4Client& client, NioInPacket& in_packet)
{
    std::string id, pw;
    in_packet >> id;
    in_packet >> pw;
    // 성공 1, 실패 0 - > 실패시 reason

        // DB 접속
    try {
        auto con = database_->Connect(odbc_, db_id_, db_pw_);
        auto ps = con->GetPreparedStatement();
        ps->PrepareStatement(TEXT("select pw from account where id = ?"));
        ps->SetString(1, id);
        auto rs = ps->Execute();

        char db_pw[100];
        rs->BindString(1, db_pw, 100);
        if (!rs->Next()) {
            rs->Close(); rs.reset();
            ps->Close();

            ps->PrepareStatement(TEXT("insert into account (id, pw) values (?, ?)"));
            ps->SetString(1, id);
            ps->SetString(2, pw);
            ps->ExecuteUpdate();
            ps->Close(); ps.reset();

        }
        if (rs) rs->Close();
        if (ps) ps->Close();
    } catch (const std::exception & e) {
        throw e;
    }
}

void LoginServer::HandleLoginRequest(UE4Client& client, NioInPacket& in_packet)
{
    std::string id, pw;
    in_packet >> id;
    in_packet >> pw;

    bool login_ok = false;

    // DB 접속
    try {
        auto con = database_->Connect(odbc_, db_id_, db_pw_);
        auto ps = con->GetPreparedStatement();
        ps->PrepareStatement(TEXT("select pw from account where id = ?"));
        ps->SetString(1, id);
        auto rs = ps->Execute();

        char db_pw[100];
        rs->BindString(1, db_pw, 100);
        if (rs->Next()) {
            if (!pw.compare(db_pw)) {
                login_ok = true;
            }
        }
        if (login_ok) {
            std::unique_lock lock(connected_id_set_garud_);
            auto iter = connected_id_set_.find(id);
            if (iter != connected_id_set_.end()) {
                lock.unlock();
                UE4OutPacket out;
                out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kLoginResult));
                out.WriteInt32(3);
                out.MakePacketHead();
                client.GetSession()->AsyncSend(out, false, true);        
            } else {
                connected_id_set_.emplace(id);
                lock.unlock();
                UE4OutPacket mig_noti;
                mig_noti.WriteInt16(static_cast<int16_t>(ToLobbyServer::UserMigrationNotify)); // opcode
                mig_noti.Write(SessionAuthorityInfo(client.GetSession()->GetRemoteAddress(), id));
                mig_noti << client.GetUUID();
                mig_noti.MakePacketHead();
                GetNIoServer()->GetChannel().GetConnection(lobby_channel_id).Send(mig_noti);
            }
        } else {
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kLoginResult));
            out.WriteInt32(login_ok);
            out.MakePacketHead();
            client.GetSession()->AsyncSend(out, false, true);
        }
    } catch (const std::exception & e) {
        throw e;
    }
}
