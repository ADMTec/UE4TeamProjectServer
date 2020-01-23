#include "NioServer.hpp"
#include "NioSession.hpp"
#include "NioCipher.hpp"
#include "NioInternalBuffer.hpp"
#include "NioInPacket.hpp"
#include "NioEventHandler.hpp"
#include "NioOutPacket.hpp"
#include "NioChannel.hpp"
#include "NIoLogger.hpp"

#include "Exception.hpp"
#include "Private/NioSocket.hpp"
#include "Private/NioContext.hpp"
#include "Private/NioSystemClockTimer.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>


using concrete_acceptor_ptr_t = boost::asio::ip::tcp::acceptor*;
#define ACCEPTOR_CAST(acceptor_) reinterpret_cast<concrete_acceptor_ptr_t>(acceptor_)


NioServer::NioServer()
    : is_running_(false), acceptor_(NULL)
{
    max_connection_ = 50;
    port_ = 10000;
    num_worker_ = 4;
    buffer_size_ = 1024;

    logger_ = std::make_shared<NioLogger>();
    in_packet_template_ = std::make_shared<NioInPacket>();
    event_handler_template_ = std::make_shared<NioEventHandler>();
    cipher_template_ = std::make_shared<NioCipher>();
}

NioServer::~NioServer()
{
    if (acceptor_ != NULL) {
        delete ACCEPTOR_CAST(acceptor_);
    }
    this->Stop();
    this->Exit();
}

void NioServer::PrintCurrentSessionQueue() const
{
    std::stringstream ss;
    auto copy = session_pool_;
    ss << "-----------------------------------------------------------\n";
    ss << " Max Session Pool Size: " << max_connection_ << '\n';
    ss << " Current Session Pool Size: " << copy.size();
    if (copy.size() > 0) {
        ss << " Connected Session IP: " << '\n';
        for (const auto& session : copy) {
            ss << " " << session->GetRemoteAddress() << '\n';
        }
    }
    ss << "-----------------------------------------------------------\n";
}

void NioServer::Initialize()
{
    {
        std::unique_lock lock(session_pool_gaurd_);
        session_pool_.clear();
    }
    if (!nio_context_) {
        if (num_worker_ > 0) {
            nio_context_ = std::make_shared<NioContext>(num_worker_);
        } else {
            nio_context_ = std::make_shared<NioContext>();
        }
        channel_ = std::make_shared<NioChannel>(nio_context_);
    }
    acceptor_ = reinterpret_cast<acceptor_ptr_t>(
        new boost::asio::ip::tcp::acceptor(
            nio_context_->GetContext(),
            boost::asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_)));
    if (num_worker_ > 0) {
        work_thread_group_.Run(
            num_worker_,
            std::bind(
                [shared = shared_from_this()](std::shared_ptr<NioContext> ioc) {
                    try {
                        ioc->Run();
                    } catch (const std::exception & e) {
                        std::stringstream ss;
                        ss << "Nio Work Thread[" << std::this_thread::get_id() << "] Terminate because Unhanlded Exception...\n";
                        ss << e.what();
                        shared->Log(e.what());
                    }
                }, nio_context_)
        );
    }
    {
        std::unique_lock lock(session_pool_gaurd_);
        for (uint64_t i = 0; i < max_connection_; ++i) {
            session_id_queue_.push(i);

            auto session = std::make_shared<NioSession>(i);
            session->SetPacketCipher(cipher_template_);
            char* buffer = BUFFER_MALLOC(buffer_size_);
            auto internal_buffer = std::shared_ptr<NioInternalBuffer>(
                new NioInternalBuffer(), 
                [](NioInternalBuffer* ptr) { 
                    BUFFER_FREE(ptr->GetData()); 
                    delete ptr; 
                });
            internal_buffer->Initialize(buffer, buffer_size_);
            session->SetInternalBuffer(internal_buffer);
            session->SetInPacket(in_packet_template_);
            session->SetEventHandler(event_handler_template_);
            session_pool_.emplace_back(session);
        }
    }
}

void NioServer::Run()
{
    bool expected{ false };
    if (is_running_.compare_exchange_strong(expected, true)) {
        Accept();
    }
}

void NioServer::Accept()
{
    int64_t id = -1;
    if (session_id_queue_.try_pop(id))
    {
        std::shared_ptr<NioSocket> sock = std::shared_ptr<NioSocket>(
            new NioStrandSocket(*nio_context_),
            [id, this](NioSocket* ptr) {
                this->GetSession(id)->OnClosed();
                delete ptr;
                this->ReleaseSession(id);
            }
        );
        {
            std::shared_lock lock(session_pool_gaurd_);
            ACCEPTOR_CAST(acceptor_)->async_accept(
                sock->GetAsioSocket(),
                [session = session_pool_[id], sock = sock, this](const boost::system::error_code& ec) {
                    if (!ec) {
                        session->SetSocketContext(sock);
                        session->OnConnected();
                    } else {
                        session->OnError(ec);
                    }
                    this->Accept();
                }
            );
        }
    }
}

void NioServer::Clear()
{
    std::shared_lock lock(session_pool_gaurd_);
    for (auto& element : session_pool_)
    {
        if (element) {
            element->Close();
        }
    }
}

void NioServer::Stop()
{
    bool expected{ true };
    if (is_running_.compare_exchange_strong(expected, false)) {
        Clear();
    }
}

void NioServer::Exit()
{
    this->Stop();
    if (nio_context_) {
        nio_context_->Stop();
    }
}

bool NioServer::IsRunning() const
{
    return is_running_;
}

void NioServer::Log(const char* msg)
{
    if (logger_) {
        logger_->Log(msg);
    }
}

std::shared_ptr<NioSession> NioServer::GetSession(int64_t index)
{
    return session_pool_[index];
}

void NioServer::ReleaseSession(int64_t index)
{
    if (is_running_) {
        session_id_queue_.push(index);
        Accept();
    }
}

void NioServer::SetNioContext(const std::shared_ptr<NioContext>& context)
{
    nio_context_ = context;
}

const std::shared_ptr<NioContext>& NioServer::GetContext()
{
    return nio_context_;
}

const std::shared_ptr<NioEventHandler>& NioServer::GetEventHandler()
{
    return event_handler_template_;
}

NioChannel& NioServer::GetChannel()
{
    return *channel_;
}

const __UUID& NioServer::CreateTimer(const std::function<void(void)>& func, std::chrono::milliseconds milli)
{
    auto new_timer = std::make_shared<NioSystemClockTimer>(*nio_context_);
    __UUID uuid = __UUID::Generate();
    new_timer->SetTimer(
        [func, shared = shared_from_this(), uuid = uuid]() {
        if (func) {
            func();
        }
        shared->CancelTimer(uuid);
    }, milli);
    std::lock_guard lock(timer_pool_guard_);
    timer_.emplace(uuid, new_timer);
    return uuid;
}

size_t NioServer::CancelTimer(const __UUID& timer_key)
{
    std::lock_guard lock(timer_pool_guard_);
    return timer_.erase(timer_key);
}

NioServerBuilder::NioServerBuilder()
{
    server_ = std::shared_ptr<NioServer>(new NioServer());
}

NioServerBuilder::~NioServerBuilder()
{
}

NioServerBuilder& NioServerBuilder::SetPort(uint16_t port)
{
    server_->port_ = port;
    return *this;
}

NioServerBuilder& NioServerBuilder::SetMaxConnection(uint64_t max_connection)
{
    server_->max_connection_ = max_connection;
    return *this;
}

NioServerBuilder& NioServerBuilder::SetNioThreadCount(uint64_t nio_worker_thread_count)
{
    server_->num_worker_ = nio_worker_thread_count;
    return *this;
}

NioServerBuilder& NioServerBuilder::SetNioLogger(const std::shared_ptr<NioLogger>& logger)
{
    server_->logger_ = logger;
    return *this;
}

NioServerBuilder& NioServerBuilder::SetNioInternalBufferSize(uint64_t buffer_size)
{
    server_->buffer_size_ = buffer_size;
    return *this;
}

NioServerBuilder& NioServerBuilder::SetNioPacketCipher(const std::shared_ptr<NioCipher>& cipher)
{
    server_->cipher_template_ = cipher;
    return *this;
}

NioServerBuilder& NioServerBuilder::SetNioEventHandler(const std::shared_ptr<NioEventHandler>& event_handler)
{
    server_->event_handler_template_ = event_handler;
    return *this;
}

std::shared_ptr<NioServer> NioServerBuilder::Build()
{
    std::shared_ptr<NioServer> server = std::move(server_);
    server_ = std::shared_ptr<NioServer>(new NioServer());
    server->Initialize();
    return server;
}