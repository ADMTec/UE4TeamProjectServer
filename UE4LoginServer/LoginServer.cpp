#include "LoginServer.hpp"
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
    auto opt_ip = reader_.GetString("LOGIN_SERVER_IP");
    auto opt_port = reader_.GetInt32("LOGIN_SERVER_PORT");
    auto opt_max_conn = reader_.GetInt32("LOGIN_SERVER_MAX_CONNECTION");
    auto opt_worker_size = reader_.GetInt32("LOGIN_SERVER_NUM_IO_WORKER");
    if(!opt_odbc || !opt_db_id || !opt_db_pw || !opt_ip || !opt_port || !opt_max_conn || !opt_worker_size)
    {
        throw StackTraceException(ExceptionType::kLogicError, "no data");
    }
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
        .SetNioEventHandler(std::shared_ptr<NioEventHandler>(new UE4EventHandler<LoginServer>()));
    this->SetNioServer(builder.Build());

    GetNioServer()->GetChannel().MakeConnection(intermediate_server);
    GetNioServer()->GetChannel().GetConnection(intermediate_server).BindFunction(
        static_cast<int16_t>(IntermediateServerSendPacket::kReactUserMigation),
        [this](NioSession& session, InputStream& stream) {
            std::string user_uuid = stream.ReadString();
            std::string lobby_ip = stream.ReadString();
            uint16_t looby_port = stream.ReadInt16();

            auto client = this->GetClient(user_uuid);
            if (client) {
                UE4OutPacket out;
                out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kLoginResult));
                out.WriteInt8(true);
                out << lobby_ip;
                out << looby_port;
                out.MakePacketHead();
                client->GetSession()->AsyncSend(out, false, true);
            }
        }
    );
}

void LoginServer::Run()
{
    auto nio = GetNioServer();
    if (nio) {
        GetNioServer()->Run();
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

void LoginServer::Stop()
{
    auto nio = GetNioServer();
    if (nio) {
        nio->Stop();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "nio server is nullptr");
    }
}

void LoginServer::ConnectChannel()
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

void LoginServer::OnActiveClient(UE4Client& client)
{
}

void LoginServer::OnCloseClient(UE4Client& client)
{
}

void LoginServer::OnProcessPacket(const std::shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet)
{
    try {
        int16_t opcode = in_packet->ReadInt16();
        switch (static_cast<ENetworkCSOpcode>(opcode)) {
            case ENetworkCSOpcode::kCreateAccountRequest:
                HandleCreateAccountRquest(client, *in_packet);
                break;
            case ENetworkCSOpcode::kLoginRequest:
                HandleLoginRequest(client, *in_packet);
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

void LoginServer::HandleCreateAccountRquest(const shared_ptr<UE4Client>& client, NioInPacket& in_packet)
{
    std::string id, pw;
    in_packet >> id;
    in_packet >> pw;
    // ���� 1, ���� 0 - > ���н� reason
    
    try {
        auto con = ODBCConnectionPool::Instance().GetConnection();
        auto ps = con->GetPreparedStatement();
        ps->PrepareStatement(TEXT("select pw from accounts where id = ?"));
        ps->SetString(1, id);
        auto rs = ps->Execute();

        char db_pw[100];
        rs->BindString(1, db_pw, 100);
        bool result = !rs->Next();
        if (result) {
            rs->Close();
            ps->Close();

            ps->PrepareStatement(TEXT("insert into accounts (id, pw) values (?, ?)"));
            ps->SetString(1, id);
            ps->SetString(2, pw);
            ps->ExecuteUpdate();
        }
        rs->Close();
        ps->Close();
        UE4OutPacket out;
        out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kCreateAccountResult));
        out.WriteInt8(result);
        out.MakePacketHead();
        client->GetSession()->AsyncSend(out, false, true);
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
}

void LoginServer::HandleLoginRequest(const shared_ptr<UE4Client>& client, NioInPacket& in_packet)
{
    std::string id, pw;
    in_packet >> id;
    in_packet >> pw;

    bool login_ok = false;

    // DB ����
    try {
        auto con = ODBCConnectionPool::Instance().GetConnection();
        auto ps = con->GetPreparedStatement();
        ps->PrepareStatement(TEXT("select accid, id, pw, loggedin from accounts where id = ?"));
        ps->SetString(1, id);
        auto rs = ps->Execute();

        int32_t accid;
        volatile char db_id[100] = {0, };
        volatile char db_pw[100] = { 0, };
        int32_t loggedin;
        rs->BindInt32(1, &accid);
        rs->BindString(2, (char*)db_id, 50);
        rs->BindString(3, (char*)db_pw, 50);
        rs->BindInt32(4, &loggedin);
        if (rs->Next()) {
            if (!pw.compare((char*)db_pw)) {
                login_ok = true;
            }
        }
        if (login_ok) {
            if (!loggedin) { // Success
                rs->Close();
                ps->Close();
                /*ps->PrepareStatement(TEXT("update accounts set loggedin = 1 where accid = ?"));
                ps->SetInt32(1, &accid);
                ps->ExecuteUpdate();
                ps->Close();*/

                RemoteSessionInfo info;
                info.SetAccid(accid);
                info.SetId((char*)db_id);
                info.SetIp(client->GetSession()->GetRemoteAddress()); // ���⼭ ����

                UE4OutPacket mig_noti;
                mig_noti.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kRequestUserMigration)); // opcode
                mig_noti.WriteInt16(static_cast<int16_t>(ServerType::kLobbyServer));
                mig_noti << client->GetUUID();
                mig_noti << info;
                mig_noti.MakePacketHead();
                GetNioServer()->GetChannel().GetConnection(intermediate_server).Send(mig_noti);
            } else { // current connected id
                UE4OutPacket out;
                out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kLoginResult));
                out.WriteInt8(2);
                out.MakePacketHead();
                client->GetSession()->AsyncSend(out, false, true);
            }
        } else { // wrong id or password
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kLoginResult));
            out.WriteInt8(login_ok);
            out.MakePacketHead();
            client->GetSession()->AsyncSend(out, false, true);
        }
    } catch (const std::exception & e) {
        throw e;
    }
}
