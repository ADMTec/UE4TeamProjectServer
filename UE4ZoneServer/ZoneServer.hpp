#pragma once
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <mutex>

using std::unordered_map;

class ZoneServer : public UE4BaseServer, public TSingleton<ZoneServer>
{
    constexpr static const int intermediate_server = 0;
    ZoneServer();
    friend class TSingleton<ZoneServer>;
public:
    enum ClientState {
        kNotConfirm,
        kConfirm,
        kReserveToMigrate,
    };
    enum class ClientContextKey {
        kCharacter,
    };
public:
    ~ZoneServer();
    virtual void Initialize();
    virtual void Run();
    virtual void Stop();
    virtual void ConnectChannel();
    virtual void OnActiveClient(UE4Client& client);
    virtual void OnCloseClient(UE4Client& client);
    virtual void OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet);
private:
    void HandleConfirmRequest(const shared_ptr<UE4Client>& client, NioInPacket& in_packet);
private:
    std::mutex session_authority_guard_;
    unordered_map<RemoteSessionInfo::id_t, RemoteSessionInfo> authority_map_;

    std::shared_ptr<class Zone> town_;
};