#include "NioSocket.hpp"
#include "../NioSession.hpp"
    

NioSocket::NioSocket(NioContext& io_context)
    : socket_(io_context.GetContext())
{
}

NioSocket::~NioSocket()
{
    Close();
}

void NioSocket::Connect(const char* server_ip, uint16_t port, const conn_complete_task_t& callback)
{
    socket_.async_connect(
        asio::ip::tcp::endpoint{ asio::ip::make_address(server_ip), port },
        callback
    );
}

std::string NioSocket::GetRemoteAddress() const
{
    return socket_.remote_endpoint().address().to_string();
}

void NioSocket::Close()
{
    if (IsOpen())
    {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket_.close();
    }
}

bool NioSocket::IsOpen() const
{
    return socket_.is_open();
}

NioSocket::socket_t& NioSocket::GetAsioSocket()
{
    return socket_;
}


NioAsyncSocket::NioAsyncSocket(NioContext& io_context)
    : NioSocket(io_context)
{
}

void NioAsyncSocket::Send(const char* buffer, uint64_t length, const io_complete_task_t& callback)
{
    asio::async_write(
        socket_,
        asio::buffer(buffer, length),
        callback
    );
}

void NioAsyncSocket::Receive(char* buffer, uint64_t length, const io_complete_task_t& callback)
{
    socket_.async_read_some(
        asio::buffer(buffer, length),
        callback
    );
}

NioStrandSocket::NioStrandSocket(NioContext& io_context)
    : NioAsyncSocket(io_context), strand_(io_context.GetContext())
{
}

void NioStrandSocket::Send(const char* buffer, uint64_t length, const io_complete_task_t& callback)
{
    NioAsyncSocket::Send(buffer, length, strand_.wrap(callback));
}

void NioStrandSocket::Receive(char* buffer, uint64_t length, const io_complete_task_t& callback)
{
    NioAsyncSocket::Receive(buffer, length, strand_.wrap(callback));
}