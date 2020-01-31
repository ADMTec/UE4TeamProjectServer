#include "UE4BaseServer.hpp"
#include "../Exception.hpp"
#include "../Time.hpp"
#include <sstream>
#include <iostream>

UE4BaseServer::UE4BaseServer()
    : print_log_(true)
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
    if (print_log_) {
        std::stringstream ss;
        Clock clock;
        ss << "[" << Calendar::DateTime(clock) << "] "
            << client->GetSession()->GetRemoteAddress() << " connect...\n";
        std::cout << ss.str();
    }
}

void UE4BaseServer::CloseClient(const std::string& client_key)
{
    std::shared_ptr<UE4Client> client = storage_.Find(client_key);
    if (client) {
        if (print_log_) {
            std::stringstream ss;
            Clock clock;
            ss << "[" << Calendar::DateTime(clock) << "] "
                << client->GetSession()->GetRemoteAddress() << " close...\n";
            std::cout << ss.str();
        }
        this->OnCloseClient(*client);
        client->Close();
        storage_.Erase(client_key);
    }
}

void UE4BaseServer::ProcessPacket(NioSession& session, const shared_ptr<NioInPacket>& in_packet)
{
    std::shared_ptr<UE4Client> client = storage_.Find(session.GetClientKey());
    if (!client) {
        std::stringstream ss;
        ss << "ip[" << session.GetRemoteAddress() << "]'s clinet is nullptr";
        throw StackTraceException(ExceptionType::kNullPointer, ss.str().c_str());
    }
    if (print_log_) {
        std::stringstream ss;
        ss << "Packet Dump: " << in_packet->GetDebugString() << '\n';
        std::cout << ss.str();
    }
    in_packet->SetAccesOffset(2);
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

void UE4BaseServer::SetPrintLogState(bool value)
{
    print_log_ = value;
}

bool UE4BaseServer::GetPrintState() const
{
    return print_log_;
}

std::shared_ptr<UE4Client> UE4BaseServer::GetClient(const std::string& uuid) const
{
    return storage_.Find(uuid);
}

void UE4BaseServer::SetNioServer(const shared_ptr<NioServer>& io_server)
{
    server_ = io_server;
}

const std::shared_ptr<NioServer>& UE4BaseServer::GetNioServer() const
{
    return server_;
}
