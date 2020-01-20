#include "UE4BaseServer.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include <sstream>


UE4BaseServer::UE4BaseServer()
{
}

UE4BaseServer::~UE4BaseServer()
{
}

void UE4BaseServer::ActiveClient(NioSession& session)
{
    std::shared_ptr<UE4Client> client = std::make_shared<UE4Client>(session.shared_from_this());
    const auto& uuid = client->GetUUID();
    session.SetClinetKey(uuid.ToString());
    storage_.Insert(uuid.ToString(), client);
    this->OnActiveClient(*client);
}

void UE4BaseServer::CloseClient(const std::string& client_key)
{
    std::shared_ptr<UE4Client> client = storage_.Find(client_key);
    if (client) {
        this->OnCloseClient(*client);
        client->Close();
        storage_.Erase(client_key);
    }
}

void UE4BaseServer::ProcessPacket(NioSession& session, NioInPacket& in_packet)
{
    std::shared_ptr<UE4Client> client = storage_.Find(session.GetClientKey());
    if (!client) {
        std::stringstream ss;
        ss << "ip[" << session.GetRemoteAddress() << "]'s clinet is nullptr";
        throw StackTraceException(ExceptionType::kNullPointer, ss.str().c_str());
    }
    this->OnProcessPacket(client, in_packet);
}

void UE4BaseServer::BroadCastPacket(NioOutPacket& outpacket)
{
    for (const auto& stored_client : storage_.GetCopy()) {
        if (stored_client) {
            stored_client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void UE4BaseServer::BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude)
{
    for (const auto& stored_client : storage_.GetCopy()) {
        if (stored_client && stored_client.get() != exclude) {
            stored_client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void UE4BaseServer::Initialize()
{
}

void UE4BaseServer::Run()
{
}

void UE4BaseServer::Stop()
{
}

void UE4BaseServer::ConnectChannel()
{
}

void UE4BaseServer::OnActiveClient(UE4Client& client)
{
}

void UE4BaseServer::OnCloseClient(UE4Client& client)
{
}

void UE4BaseServer::OnProcessPacket(const std::shared_ptr<UE4Client>& client, NioInPacket& in_packet)
{
}

std::shared_ptr<UE4Client> UE4BaseServer::GetClient(const std::string& uuid) const
{
    return storage_.Find(uuid);
}

void UE4BaseServer::SetNioServer(const std::shared_ptr<NioServer>& io_server)
{
    server_ = io_server;
}

const std::shared_ptr<NioServer>& UE4BaseServer::GetNIoServer() const
{
    return server_;
}
