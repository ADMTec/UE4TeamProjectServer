#include "NioChannel.hpp"
#include "Exception.hpp"
#include "NioServer.hpp"
#include "NioSession.hpp"
#include "NioInternalBuffer.hpp"
#include "NioInPacket.hpp"
#include "Private/NioSocket.hpp"
#include <shared_mutex>


namespace {
    void TryBindFunction(const std::shared_ptr<NioEventHandler>& event_handler, int16_t opcode, const NioChannelEventHandler::handler_t& func)
    {
        auto channel_handler = std::dynamic_pointer_cast<NioChannelEventHandler>(event_handler);
        if (channel_handler) {
            channel_handler->BindFunction(opcode, func);
        } else {
            throw StackTraceException(ExceptionType::kRunTimeError, "dynamic cast NioEventHandler to NioChannelEventHandler fail...");
        }
    }
}


NioChannel::Receiver::Receiver(const NioChannel::Receiver& rhs)
{
    host_ = rhs.host_;
    id_ = rhs.id_;
}

void NioChannel::Receiver::operator=(const NioChannel::Receiver& rhs)
{
    host_ = rhs.host_;
    id_ = rhs.id_;
}

NioChannel::Receiver::Receiver(const std::shared_ptr<NioContext>& context, int id, uint16_t port, uint32_t max_connection)
    : id_(id)
{
    NioServerBuilder builder;
    builder.SetPort(port)
        .SetMaxConnection(max_connection)
        .SetNioInternalBufferSize(1024)
        .SetNioThreadCount(0)
        .SetNioPacketCipher(std::shared_ptr<NioCipher>(new NioChannelCipher()))
        .SetNioEventHandler(std::shared_ptr<NioEventHandler>(new NioChannelEventHandler()));

    host_ = builder.Build();
    host_->SetNioContext(context);
    host_->Initialize();
}

NioChannel::Receiver::Receiver()
    : id_(0x7FFFFFFF), host_(nullptr)
{
}

NioChannel::Receiver::~Receiver()
{
    if (host_) {
        host_->Stop();
    }
    
}

void NioChannel::Receiver::Run()
{
    if (host_)
        host_->Run();
}

void NioChannel::Receiver::Stop()
{
    if (host_)
        host_->Stop();
}

void NioChannel::Receiver::Clear()
{
    if (host_)
        host_->Clear();
}

void NioChannel::Receiver::BindFunction(int16_t opcode, const NioChannelEventHandler::handler_t& func)
{
    if (host_)
        TryBindFunction(host_->GetEventHandler(), opcode, func);
}

bool NioChannel::Receiver::IsValid() const
{
    return host_.operator bool();
}

bool NioChannel::Receiver::IsOpen() const
{
    if (IsValid() && host_->IsRunning())
        return true;
    return false;
}

int NioChannel::Receiver::GetId() const
{
    return id_;
}

NioChannel::Connection::Connection(const NioChannel::Connection& rhs)
{
    session_ = rhs.session_;
}


void NioChannel::Connection::operator=(const NioChannel::Connection& rhs)
{
    session_ = rhs.session_;
}

NioChannel::Connection::Connection(const std::shared_ptr<NioChannel>& channel, int identifier)
{
    session_ = std::make_shared<NioSession>(identifier);

    session_->SetPacketCipher(std::shared_ptr<NioCipher>(new NioChannelCipher()));

    char* buffer = BUFFER_MALLOC(1024);
    auto internal_buffer = std::shared_ptr<NioInternalBuffer>(
        new NioInternalBuffer(), [](NioInternalBuffer* ptr) { BUFFER_FREE(ptr->GetData()); delete ptr; });
    internal_buffer->Initialize(buffer, 1024);
    session_->SetInternalBuffer(internal_buffer);

    session_->SetInPacket(std::make_shared<NioInPacket>());

    session_->SetEventHandler(std::shared_ptr<NioEventHandler>(new NioChannelEventHandler()));

    std::shared_ptr<NioSocket> sock = std::shared_ptr<NioSocket>(
        new NioStrandSocket(*channel->GetContext()),
        [channel = channel, shared = session_->shared_from_this(), this, identifier](NioSocket* ptr) {
        shared->OnClosed();
        delete ptr;
        channel->CloseConnection(identifier);
    });
    session_->SetSocketContext(sock);
}

NioChannel::Connection::Connection()
    : session_(nullptr)
{
}

bool NioChannel::Connection::IsValid() const
{
    return session_.operator bool();
}

bool NioChannel::Connection::IsOpen() const
{
    if (IsValid() && session_->IsOpen())
        return true;
    return false;
}

int64_t NioChannel::Connection::GetId() const
{
    return session_->GetId();
}

void NioChannel::Connection::BindFunction(int16_t opcode, const NioChannelEventHandler::handler_t& func)
{
    TryBindFunction(session_->GetEventHandler(), opcode, func);
}

bool NioChannel::Connection::Connect(const char* ip, uint16_t port)
{
    bool result = session_->SyncConnect(ip, port);
    if (result) {
        session_->OnConnected();
    }
    return result;
}

void NioChannel::Connection::Send(NioOutPacket& out)
{
    session_->AsyncSend(out, false, false);
}

void NioChannel::Connection::Close()
{
    session_->Close();
}

NioChannel::NioChannel(const std::shared_ptr<NioContext>& context)
    : context_(context)
{
}

NioChannel::~NioChannel()
{   
    {
        std::shared_lock shared_lock(connection_pool_guard_);
        for (auto& guest : connection_pool_) {
            guest.Close();
        }
    }
    std::unique_lock lock(connection_pool_guard_);
    connection_pool_.clear();
}

void NioChannel::MakeReceiver(int identifier, uint16_t port, uint32_t max_connection)
{
    std::unique_lock lock(receiver_pool_guard_);
    receiver_pool_.emplace_back(context_, identifier, port, max_connection);
}

NioChannel::Receiver NioChannel::GetReceiver(int identifier)
{
    std::shared_lock lock(receiver_pool_guard_);
    auto iter = std::find_if(receiver_pool_.begin(), receiver_pool_.end(),
        [identifier](const NioChannel::Receiver& host) {
            if (host.IsValid()) {
                return host.GetId() == identifier;
            }
            return false;
        });
    if (iter != receiver_pool_.end()) {
        return *iter;
    } else {
        return NioChannel::Receiver();
    }
}

void NioChannel::MakeConnection(int identifier)
{
    std::unique_lock lock(connection_pool_guard_);
    connection_pool_.emplace_back(this->shared_from_this(), identifier);
}

void NioChannel::CloseConnection(int identifier)
{
    std::unique_lock lock(connection_pool_guard_);
    auto iter = std::find_if(connection_pool_.begin(), connection_pool_.end(),
        [identifier](const Connection& geust) {
            return geust.GetId() == identifier;
        });
    if (iter != connection_pool_.end()) {
        connection_pool_.erase(iter);
    }
}

NioChannel::Connection NioChannel::GetConnection(int identifier)
{
    std::shared_lock lock(connection_pool_guard_);
    auto iter = std::find_if(connection_pool_.begin(), connection_pool_.end(),
        [identifier](const Connection& guest) {
            if (guest.IsValid()) {
                return guest.GetId() == identifier;
            }
            return false;
        });
    if (iter != connection_pool_.end()) {
        return *iter;
    } else {
        return Connection();
    }
}

const std::shared_ptr<NioContext>& NioChannel::GetContext()
{
    return context_;
}
