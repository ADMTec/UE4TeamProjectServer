#pragma once
#include "UE4Client.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <vector>
#include <atomic>
#include <concurrent_queue.h>

class ClientStorage
{
public:
    ClientStorage(const ClientStorage&) = delete;
    void operator=(const ClientStorage&) = delete;
    ClientStorage(ClientStorage&&) noexcept = delete;
    void operator=(ClientStorage&&) noexcept = delete;
public:
    ClientStorage() = default;
    ~ClientStorage() = default;

    bool Insert(const std::string& uuid, const std::shared_ptr<UE4Client>& client);
    size_t Erase(const std::string& uuid);
    std::shared_ptr<UE4Client> Find(const std::string& uuid) const;
    std::vector<std::shared_ptr<UE4Client>> GetCopy() const;
private:
    mutable std::shared_mutex storage_mutex_;
    std::unordered_map<std::string, std::shared_ptr<UE4Client>> storage_;
};

class TestServer : public TSingleton<TestServer>
{
    friend class TSingleton<TestServer>;
    TestServer();
public:
    virtual ~TestServer();

    void Initialize();
    void Run();
    void Stop();
    void ActiveClient(NioSession & session);
    void CloseClient(const std::string & client_key);
    void ProcessPacket(NioSession & session, const std::shared_ptr<NioInPacket>& in_packet);
    void BroadCastPacket(NioOutPacket & outpacket);
    void BroadCastPacket(NioOutPacket & outpacket, UE4Client* exclude);
    std::shared_ptr<UE4Client> GetClient(const std::string& uuid) const;
    std::vector<std::shared_ptr<UE4Client>> GetClientAll() const;
    void HandleSpawn(UE4Client& client, NioInPacket& input);
    void HandlePossess(UE4Client& client, NioInPacket& input);
    void HandleNotifiyPosition(UE4Client& client, NioInPacket& input);
protected:
    std::thread worker_;
    std::atomic<bool> eixt_flag_;
    ClientStorage storage_;
    std::shared_ptr<NioServer> io_server_;
};

