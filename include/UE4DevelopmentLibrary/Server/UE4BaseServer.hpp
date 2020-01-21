#pragma once
#include "ClientStorage.hpp"


class UE4BaseServer;

using std::shared_ptr;

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
    void ProcessPacket(NioSession& session, const shared_ptr<NioInPacket>& in_packet);
    void BroadCastPacket(NioOutPacket& outpacket);
    void BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude);
public:
    virtual void Initialize() = 0;
    virtual void Run() = 0;
    virtual void Stop() = 0;
    virtual void ConnectChannel() = 0;
    virtual void OnActiveClient(UE4Client& client) = 0;
    virtual void OnCloseClient(UE4Client& client) = 0;
    virtual void OnProcessPacket(const shared_ptr<UE4Client>& client, const shared_ptr<NioInPacket>& in_packet) = 0;
public:
    void SetPrintLogState(bool value);
    bool GetPrintState() const;
    std::shared_ptr<UE4Client> GetClient(const std::string& uuid) const;
    const std::shared_ptr<NioServer>& GetNioServer() const;
protected:
    void SetNioServer(const shared_ptr<NioServer>& io_server);
private:
    ClientStorage storage_;
    std::shared_ptr<NioServer> server_;
    bool print_log_;
};