#pragma once
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <array>

using std::array;
using std::unordered_map;

class IntermediateServer : public UE4BaseServer, public TSingleton<IntermediateServer>
{
    friend class TSingleton<IntermediateServer>;
    IntermediateServer();
public:
    virtual void Initialize();
    virtual void Run();
    virtual void Stop();
    virtual void ConnectChannel();
    virtual void OnActiveClient(UE4Client& client);
    virtual void OnCloseClient(UE4Client& client);
    virtual void OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet);
private:
    void HandleRegisterServer(const shared_ptr<UE4Client>& client, NioInPacket& packet);
    void UpdateServerConnection(UE4Client& client, NioInPacket& packet);
    void HandleRequestUserMigration(UE4Client& client, NioInPacket& packet);
    void HandleReactSessionAuthorityInfo(UE4Client& client, NioInPacket& packet);
    void HandleNotifiyUserLogout(UE4Client& client, NioInPacket& packet);
private:
    TextFileLineReader reader_;

    std::shared_mutex data_guard_;
    unordered_map<std::string, RemoteServerInfo> server_map_;
};