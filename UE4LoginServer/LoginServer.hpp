#pragma once
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <unordered_set>
#include <shared_mutex>
#include <memory>
#include <optional>

class LoginServer;
template<typename session>
struct ServerTemplateParameter
{
    using InPacket = NioInPacket;
    using Buffer = NioBuffer<InPacket>;
    using OutPacket = UE4OutPacket;

    using Cipher = UE4PacketCipher<session, Buffer, InPacket, OutPacket>;
    using Handler = UE4EventHandler<session, InPacket, LoginServer>;

    constexpr static bool Strand = true;
    constexpr static int InternalBufferLength = 1024;
    constexpr static int RecvBufferLength = 1024;
};

using Session = TNioSession<ServerTemplateParameter>;
using IoServer = TNioServer<Session>;
using Client = UE4Client<Session>;


template<typename Session>
struct IntermediateTemplateParameter
{
    using InPacket = NioInPacket;
    using Buffer = NioBuffer<InPacket>;
    using OutPacket = UE4OutPacket;

    using Cipher = UE4PacketCipher<Session, Buffer, InPacket, OutPacket>;
    using Handler = UE4IntermediateHandler<Session, InPacket>;

    constexpr static bool Strand = true;
    constexpr static int InternalBufferLength = 1024;
    constexpr static int RecvBufferLength = 1024;
};
using IntermediateSession = TNioSession<IntermediateTemplateParameter>;


class LoginServer :  public TSingleton<LoginServer>
{
    constexpr static const int intermediate_server = 0;
    LoginServer();
    friend class TSingleton<LoginServer>;
public:
    ~LoginServer();
    void Initialize();
    void Run();
    void Stop();
    void ConnectChannel();
    void OnActive(Session& session);
    void OnClose(Session& session);
    void OnError(int ec, const char* message);
    void ProcessPacket(Session& session, const std::shared_ptr<Session::InPacket>& in_packet);

    std::shared_ptr<Client> GetClient(const std::string& uuid) const;
private:
    void HandleCreateAccountRquest(const Client& client, Session::InPacket& in_packet);
    void HandleLoginRequest(const Client& client, Session::InPacket& in_packet);
private:
    bool print_log_;
    std::optional<IoServer> io_server_;
    std::shared_ptr<IntermediateSession> session_;

    mutable std::shared_mutex client_storage_guard_;
    std::unordered_map<std::string, std::shared_ptr<Client>> client_storage_;

    TextFileLineReader reader_;
    RemoteServerInfo this_info_;
};