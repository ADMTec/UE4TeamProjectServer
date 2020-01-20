#include "UE4BaseServer.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "Private/ImplUE4BaseServer.hpp"
#include <sstream>


UE4BaseServer::UE4BaseServer(const UE4BaseServer&)
{
    assert(false);
}

void UE4BaseServer::operator=(const UE4BaseServer&)
{
    assert(false);
}

UE4BaseServer::UE4BaseServer()
{
    impl_ = new ImplUE4BaseServer(*this);
}

UE4BaseServer::~UE4BaseServer()
{
    delete impl_;
}

void UE4BaseServer::ActiveClient(NioSession& session)
{
    impl_->ActiveClient(session);
}

void UE4BaseServer::CloseClient(const std::string& client_key)
{
    impl_->CloseClient(client_key);
}

void UE4BaseServer::ProcessPacket(NioSession& session, NioInPacket& in_packet)
{
    impl_->ProcessPacket(session, in_packet);
}

void UE4BaseServer::BroadCastPacket(NioOutPacket& outpacket)
{
    impl_->BroadCastPacket(outpacket);
}

void UE4BaseServer::BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude)
{
    impl_->BroadCastPacket(outpacket, exclude);
}

std::shared_ptr<UE4Client> UE4BaseServer::GetClient(const std::string& uuid) const
{
    return impl_->GetClient(uuid);
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

void UE4BaseServer::SetNioServer(const std::shared_ptr<NioServer>& io_server)
{
    impl_->SetNioServer(io_server);
}

const std::shared_ptr<NioServer>& UE4BaseServer::GetNIoServer() const
{
    return impl_->GetNIoServer();
}
