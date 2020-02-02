#pragma once
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <memory>



class IntermediateServer;
template<typename session>
struct ServerTemplateParameter
{
    using InPacket = NioInPacket;
    using Buffer = NioBuffer<InPacket>;
    using OutPacket = UE4OutPacket;

    using Cipher = UE4PacketCipher<session, Buffer, InPacket, OutPacket>;
    using Handler = UE4EventHandler<session, InPacket, IntermediateServer>;

    constexpr static bool Strand = true;
    constexpr static int InternalBufferLength = 1024;
    constexpr static int RecvBufferLength = 1024;
};

using Session = TNioSession<ServerTemplateParameter>;
using IoServer = TNioServer<Session>;
using Client = UE4Client<Session>;


class IntermediateServer : public TSingleton<IntermediateServer>
{
    friend class TSingleton<IntermediateServer>;
    IntermediateServer();
public:
    void Initialize();
    void Run();
    void Stop();
    void ConnectChannel();
    void OnActive(Session& session);
    void OnClose(Session& session);
    void OnError(int ec, const char* msg);
    void ProcessPacket(Session& session, const std::shared_ptr<Session::InPacket>& in_packet);

    std::shared_ptr<Client> GetClient(const std::string& uuid) const;
private:
    void HandleRegisterServer(Client& client, Session::InPacket& packet);
    void UpdateServerConnection(Client& client, Session::InPacket& packet);
    void HandleRequestUserMigration(Client& client, Session::InPacket& packet);
    void HandleReactSessionAuthorityInfo(Client& client, Session::InPacket& packet);
    void HandleNotifiyUserLogout(Client& client, Session::InPacket& packet);
private:
    std::shared_ptr<Client> GetServerLoadBalance(ServerType type) const;
private:
    std::optional<IoServer> io_server_;

    mutable std::shared_mutex client_storage_guard_;
    std::unordered_map<std::string, std::shared_ptr<Client>> client_storage_;

    TextFileLineReader reader_;

    mutable std::shared_mutex server_map__guard_;
    std::unordered_map<std::string, RemoteServerInfo> server_map_;
};