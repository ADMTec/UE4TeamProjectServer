#pragma once
#include "ClientStorage.hpp"


class UE4BaseServer;

class UE4BaseServer
{
    friend class UE4BaseServer;
public:
    UE4BaseServer(const UE4BaseServer&) = delete;
    void operator=(const UE4BaseServer&) = delete;
public:
    UE4BaseServer();
    virtual ~UE4BaseServer();

    void ActiveClient(NioSession& session);
    void CloseClient(const std::string& client_key);
    void ProcessPacket(NioSession& session, NioInPacket& in_packet);
    void BroadCastPacket(NioOutPacket& outpacket);
    void BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude);
public:
    virtual void Initialize();
    virtual void Run();
    virtual void Stop();
    virtual void ConnectChannel();
    virtual void OnActiveClient(UE4Client& client);
    virtual void OnCloseClient(UE4Client& client);
    virtual void OnProcessPacket(const std::shared_ptr<UE4Client>& client, NioInPacket& in_packet);
public:
    std::shared_ptr<UE4Client> GetClient(const std::string& uuid) const;
protected:
    void SetNioServer(const std::shared_ptr<NioServer>& io_server);
    const std::shared_ptr<NioServer>& GetNIoServer() const;
private:
    ClientStorage storage_;
    std::shared_ptr<NioServer> server_;
};