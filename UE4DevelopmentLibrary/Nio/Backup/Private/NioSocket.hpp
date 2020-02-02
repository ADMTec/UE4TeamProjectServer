#pragma once
#include "../NioBase.hpp"
#include "NioContext.hpp"


using namespace boost;


using conn_complete_task_t = std::function<void(const boost::system::error_code&)>;
using io_complete_task_t = std::function<void(const boost::system::error_code&, size_t)>;

class NioSocket
{
public:
    using socket_t = boost::asio::ip::tcp::socket;
public:
    NioSocket(NioContext& io_context);
    virtual ~NioSocket();
    bool SyncConnect(const char* server_ip, uint16_t port);
    virtual void Connect(const char* server_ip, uint16_t port, const conn_complete_task_t& callback);
    virtual void Send(const char* buffer, uint64_t length, const io_complete_task_t& callback) = 0;
    virtual void Receive(char* buffer, uint64_t length, const io_complete_task_t& callback) = 0;

    std::string GetRemoteAddress() const;
    void Close();
    bool IsOpen() const;
    socket_t& GetAsioSocket();
protected:
    socket_t socket_;
};

class NioAsyncSocket : public NioSocket
{
public:
    NioAsyncSocket(NioContext& io_context);
    virtual void Send(const char* buffer, uint64_t length, const io_complete_task_t& callback) override;
    virtual void Receive(char* buffer, uint64_t length, const io_complete_task_t& callback) override;
};

class NioStrandSocket : public NioAsyncSocket
{
    using sync_wrapper_t = boost::asio::io_context::strand;
public:
    NioStrandSocket(NioContext& io_context);
    virtual void Send(const char* buffer, uint64_t length, const io_complete_task_t& callback) override;
    virtual void Receive(char* buffer, uint64_t length, const io_complete_task_t& callback) override;
private:
    sync_wrapper_t strand_;
};