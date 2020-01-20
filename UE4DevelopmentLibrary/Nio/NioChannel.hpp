#pragma once
#include "NioChannelHandler.hpp"
#include "NioServer.hpp"
#include "NioSession.hpp"


class NioChannel : public std::enable_shared_from_this<NioChannel>
{
public:
    class Receiver
    {
    public:
        Receiver(const Receiver& rhs);
        void operator=(const Receiver& rhs);
    public:
        Receiver(const std::shared_ptr<NioContext>& context, int id, uint16_t port, uint32_t max_connection);
        Receiver();
        ~Receiver();
        void Run();
        void Stop();
        void Clear();
        void BindFunction(int16_t opcode, const NioChannelEventHandler::handler_t& func);
        bool IsValid() const;
        int GetId() const;
    private:
        int id_;
        std::shared_ptr<NioServer> host_;
    };
    class Connection
    {
    public:
        Connection(const Connection& rhs);
        void operator=(const Connection& rhs);
    public:
        Connection(const std::shared_ptr<NioChannel>& channel, int identifier);
        Connection();
        void BindFunction(int16_t opcode, const NioChannelEventHandler::handler_t& func);
        void Connect(const char* ip, uint16_t port);
        void Send(NioOutPacket& out);
        void Close();
        bool IsValid() const;
        int GetId() const;
    private:
        std::shared_ptr<NioSession> session_;
    };
public:
    NioChannel(const NioChannel&) = delete;
    void operator=(const NioChannel&) = delete;
public:
    explicit NioChannel(const std::shared_ptr<NioContext>& context);
    ~NioChannel();

    void MakeReceiver(int identifier, uint16_t port, uint32_t max_connection);
    NioChannel::Receiver GetReceiver(int identifier);

    void MakeConnection(int identifier);
    void CloseConnection(int identifier);
    NioChannel::Connection GetConnection(int identifier);

    const std::shared_ptr<NioContext>& GetContext();
private:
    std::shared_ptr<NioContext> context_;

    mutable std::shared_mutex host_pool_guard_;
    std::vector<NioChannel::Receiver> host_pool_;

    mutable std::shared_mutex guest_pool_guard_;
    std::vector<NioChannel::Connection> guest_pool_;
};