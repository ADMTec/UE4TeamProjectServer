#pragma once
#include "ClientStorage.hpp"


class UE4BaseServer;

class ImplUE4BaseServer
{
    friend class UE4BaseServer;
public:
    ImplUE4BaseServer(const ImplUE4BaseServer&) = delete;
    void operator=(const ImplUE4BaseServer&) = delete;
public:
    explicit ImplUE4BaseServer(UE4BaseServer& server);
    virtual ~ImplUE4BaseServer();

    void ActiveClient(NioSession& session);
    void CloseClient(const std::string& client_key);
    void ProcessPacket(NioSession& session, NioInPacket& in_packet);
    void BroadCastPacket(NioOutPacket& outpacket);
    void BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude);
public:
    std::shared_ptr<UE4Client> GetClient(const std::string& uuid) const;
protected:
    void SetNioServer(const std::shared_ptr<NioServer>& io_server);
    const std::shared_ptr<NioServer>& GetNIoServer() const;
private:
    UE4BaseServer& base_;
    ClientStorage storage_;
    std::shared_ptr<NioServer> server_;
};