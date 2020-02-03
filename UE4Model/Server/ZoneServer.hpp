#pragma once
#include "UE4DevelopmentLibrary/Nio.hpp"
#include <memory>
#include <shared_mutex>
#include "Alias.hpp"
#include <optional>


class ZoneServer
{
    constexpr static const int intermediate_server = 0;
public:
    enum ClientState {
        kNotConfirm,
        kConfirm,
        kReserveToMigrate,
    };
public:
    static ZoneServer& Instance() {
        return *static_instance_;
    }
public:
    ZoneServer();
    ~ZoneServer();
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
    void HandleChrPositionNotify(Client& client, Session::InPacket& in_packet);
    void HandleMonsterActionNotify(Client& client, Session::InPacket& in_packet);

    // Match
    void HandleMatchingRequest(Client& client, Session::InPacket& in_packet);
    void HandleMatchingComfirmRequest(Client& client, Session::InPacket& in_packet);

    // Character Attack
    void HandleCharacterAttackMotion(Client& client, Session::InPacket& in_packet);
    void HandleCharacterHitSuccess(Client& client, Session::InPacket& in_packet);

    // Monster Attack
    void HandleMonsterAttack(Client& client, Session::InPacket& in_packet);
private:
    bool print_log_;
    std::optional<IoServer> io_server_;
    std::shared_ptr<IntermediateSession> session_;

    mutable std::shared_mutex client_storage_guard_;
    std::unordered_map<std::string, std::shared_ptr<Client>> client_storage_;

    mutable std::mutex session_authority_guard_;
    std::unordered_map<RemoteSessionInfo::id_t, RemoteSessionInfo> authority_map_;
    static ZoneServer* static_instance_;
};