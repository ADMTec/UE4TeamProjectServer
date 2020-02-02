#pragma once
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <unordered_set>
#include <shared_mutex>
#include <memory>
#include <array>

class LobbyServer;
template<typename session>
struct ServerTemplateParameter
{
    using InPacket = NioInPacket;
    using Buffer = NioBuffer<InPacket>;
    using OutPacket = UE4OutPacket;

    using Cipher = UE4PacketCipher<session, Buffer, InPacket, OutPacket>;
    using Handler = UE4EventHandler<session, InPacket, LobbyServer>;

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


class LobbyServer : public TSingleton<LobbyServer>
{
    static constexpr int slot_size = 5;
    friend class TSingleton<LobbyServer>;
    LobbyServer();
public:
    enum ClientState {
        kNotConfirm,
        kConfirm,
        kReserveToMigrate,
    };
public:
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
    void HandleConfirmRequest(Client& client, Session::InPacket& in_packet);
    void HandleCharacterListRequest(Client& client, Session::InPacket& in_packet);
    void HandleCharacterCreateRequest(Client& client, Session::InPacket& in_packet);
    void HandleCharacterDeleteRequest(Client& client, Session::InPacket& in_packet);
    void HandleCharacterSelectRequest(Client& client, Session::InPacket& in_packet);
private:
    void SendCharacterList(Client& client);
private:
    std::optional<IoServer> io_server_;
    std::shared_ptr<IntermediateSession> session_;

    mutable std::shared_mutex client_storage_guard_;
    std::unordered_map<std::string, std::shared_ptr<Client>> client_storage_;

    TextFileLineReader reader_;
    RemoteServerInfo this_info_;

    std::shared_mutex session_authority_guard_;
    std::unordered_map<RemoteSessionInfo::id_t, RemoteSessionInfo> authority_map_;

    std::shared_mutex slot_info_lock;
    std::unordered_map<__UUID, std::array<int32_t, slot_size>> slot_info_;
};