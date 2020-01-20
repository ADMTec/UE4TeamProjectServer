#pragma once
#include "Base.hpp"
#include <vector>
#include <memory>
#include "UE4DevelopmentLibrary/Nio.hpp"
#include "UE4DevelopmentLibrary/Utility.hpp"


class UE4Client;
UE4_DLLEXPORT template class UE4_DLL_CLASS std::shared_ptr<UE4Client>;
UE4_DLLEXPORT template class UE4_DLL_CLASS std::shared_ptr<NioServer>;

class UE4_DLL_CLASS UE4BaseServer
{
private:
    UE4BaseServer(const UE4BaseServer&);
    void operator=(const UE4BaseServer&);
public:
    UE4BaseServer();
    virtual ~UE4BaseServer();

    void ActiveClient(NioSession& session);
    void CloseClient(const std::string& client_key);
    void ProcessPacket(NioSession& session, NioInPacket& in_packet);
    void BroadCastPacket(NioOutPacket& outpacket);
    void BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude);
    std::shared_ptr<UE4Client> GetClient(const std::string& uuid) const;
public:
    virtual void Initialize();
    virtual void Run();
    virtual void Stop();
    virtual void ConnectChannel();
    virtual void OnActiveClient(UE4Client& client);
    virtual void OnCloseClient(UE4Client& client);
    virtual void OnProcessPacket(const std::shared_ptr<UE4Client>& client, NioInPacket& in_packet);
protected:
    void SetNioServer(const std::shared_ptr<NioServer>& io_server);
    const std::shared_ptr<NioServer>& GetNIoServer() const;
private:
    class ImplUE4BaseServer* impl_;
};