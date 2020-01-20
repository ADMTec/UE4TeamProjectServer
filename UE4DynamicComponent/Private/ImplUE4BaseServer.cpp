#include "ImplUE4BaseServer.hpp"
#include "Public/UE4BaseServer.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include <sstream>


ImplUE4BaseServer::ImplUE4BaseServer(UE4BaseServer& server)
    : base_(server)
{
}

ImplUE4BaseServer::~ImplUE4BaseServer()
{
}

void ImplUE4BaseServer::ActiveClient(NioSession& session)
{
    std::shared_ptr<UE4Client> client = std::make_shared<UE4Client>(session.shared_from_this());
    const auto& uuid = client->GetUUID();
    session.SetClinetKey(uuid.ToString());
    storage_.Insert(uuid.ToString(), client);
    base_.OnActiveClient(*client);
}

void ImplUE4BaseServer::CloseClient(const std::string& client_key)
{
    std::shared_ptr<UE4Client> client = storage_.Find(client_key);
    if (client) {
        base_.OnCloseClient(*client);
        client->Close();
        storage_.Erase(client_key);
    }
}

void ImplUE4BaseServer::ProcessPacket(NioSession& session, NioInPacket& in_packet)
{
    std::shared_ptr<UE4Client> client = storage_.Find(session.GetClientKey());
    if (!client) {
        std::stringstream ss;
        ss << "ip[" << session.GetRemoteAddress() << "]'s clinet is nullptr";
        throw StackTraceException(ExceptionType::kNullPointer, ss.str().c_str());
    }
    base_.OnProcessPacket(client, in_packet);
}

void ImplUE4BaseServer::BroadCastPacket(NioOutPacket& outpacket)
{
    for (const auto& stored_client : storage_.GetCopy()) {
        if (stored_client) {
            stored_client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void ImplUE4BaseServer::BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude)
{
    for (const auto& stored_client : storage_.GetCopy()) {
        if (stored_client && stored_client.get() != exclude) {
            stored_client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

std::shared_ptr<UE4Client> ImplUE4BaseServer::GetClient(const std::string& uuid) const
{
    return storage_.Find(uuid);
}

void ImplUE4BaseServer::SetNioServer(const std::shared_ptr<NioServer>& io_server)
{
    server_ = io_server;
}

const std::shared_ptr<NioServer>& ImplUE4BaseServer::GetNIoServer() const
{
    return server_;
}
