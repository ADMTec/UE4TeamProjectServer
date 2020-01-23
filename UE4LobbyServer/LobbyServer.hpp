#pragma once
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <unordered_set>
#include <shared_mutex>
#include <memory>

using std::shared_ptr;
using std::unique_ptr;

class LobbyServer : public UE4BaseServer, public TSingleton<LobbyServer>
{
    static constexpr int intermediate_server = 0;
    friend class TSingleton<LobbyServer>;
    LobbyServer();
public:
    enum ClientState {
        kNotConfirm,
        kConfirm,
        kReserveToMigrate,
    };
public:
    virtual void Initialize();
    virtual void Run();
    virtual void Stop();
    virtual void ConnectChannel();
    virtual void OnActiveClient(UE4Client& client);
    virtual void OnCloseClient(UE4Client& client);
    virtual void OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet);
private:
    void HandleConfirmRequest(UE4Client& client, NioInPacket& in_packet);
    void HandleCharacterListRequest(UE4Client& client, NioInPacket& in_packet);
    void HandleCharacterCreateRequest(UE4Client& client, NioInPacket& in_packet);
    void HandleCharacterDeleteRequest(UE4Client& client, NioInPacket& in_packet);
    void HandleCharacterSelectRequest(UE4Client& client, NioInPacket& in_packet);
private:
    void SendCharacterList(UE4Client& client);
private:
    TextFileLineReader reader_;
    RemoteServerInfo this_info_;
    string_t odbc_;
    string_t db_id_;
    string_t db_pw_;

    std::shared_mutex session_authority_guard_;
    std::unordered_map<RemoteSessionInfo::id_t, RemoteSessionInfo> authority_map_;
};