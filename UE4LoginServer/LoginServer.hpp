#pragma once
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <unordered_set>
#include <mutex>
#include <memory>


class LoginServer : public UE4BaseServer, public TSingleton<LoginServer>
{
    constexpr static const int channel_id = 0;
    constexpr static const int lobby_channel_id = 1;
    LoginServer();
    friend class TSingleton<LoginServer>;
public:
    ~LoginServer();
    virtual void Initialize();
    virtual void Run();
    virtual void Stop();
    virtual void ConnectChannel();
    virtual void OnActiveClient(UE4Client& client);
    virtual void OnCloseClient(UE4Client& client);
    virtual void OnProcessPacket(const std::shared_ptr<UE4Client>& client, NioInPacket& in_packet);
private:
    void HandleCreateAccountRquest(UE4Client& client, NioInPacket& in_packet);
    void HandleLoginRequest(UE4Client& client, NioInPacket& in_packet);
private:
    TextFileLineReader reader_;
    std::unique_ptr<DatabaseDriver> database_;
    string_t odbc_;
    string_t db_id_;
    string_t db_pw_;
    
    std::mutex connected_id_set_garud_;
    std::unordered_set<std::string> connected_id_set_;
};