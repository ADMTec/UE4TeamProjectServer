#pragma once
#include "NioBase.hpp"
#include "NioServer.hpp"
#include <string>
#include <cstdint>
#include <vector>
#include <mutex>
#include <deque>
#include <functional>


template<
    template <class> class T>
class TNioSession : public std::enable_shared_from_this<TNioSession<T>>
{
public:
    using Cipher = typename T<TNioSession>::Cipher;
    using Buffer = typename T<TNioSession>::Buffer;
    using InPacket = typename T<TNioSession>::InPacket;
    using OutPacket = typename T<TNioSession>::OutPacket;

    using Handler = typename T<TNioSession>::Handler;

    constexpr static bool Strand = T<TNioSession>::Strand;
    constexpr static int InternalBufferLength = T<TNioSession>::InternalBufferLength;
    constexpr static int RecvBufferLength = T<TNioSession>::RecvBufferLength;
public:
    TNioSession(const TNioSession&) = delete;
    void operator=(const TNioSession&) = delete;
    TNioSession(TNioSession&& rhs) noexcept {

    }
    void operator=(TNioSession&& rhs) noexcept {

    }
public:
    TNioSession(int64_t id, asio::io_context& ioc)
        : id_(id), strand_(ioc)
    {
        Clear();
        buffer_.Initialize(internal_buffer_, InternalBufferLength);
    }
    TNioSession(asio::io_context& ioc)
        : id_(-1), socket_(std::make_shared<asio::ip::tcp::socket>(ioc)), strand_(ioc)
    {
        Clear();
        buffer_.Initialize(internal_buffer_, InternalBufferLength);
    }
    bool SyncConnect(const char* ip, uint16_t port) {
        ERROR_CODE ec;
        auto ref_holder = GetSocketReference();
        if (ref_holder) {
            socket_->connect(asio::ip::tcp::endpoint{ asio::ip::make_address(ip), port }, ec);
        }
        return !ec.operator bool();
    }
    
    template<bool Strand = Strand>
    void Connect(const char* ip, uint16_t port) {
        auto ref_holder = GetSocketReference();
        if (ref_holder) {
            ref_holder->async_connect(
                asio::ip::tcp::endpoint{ asio::ip::make_address(ip), port },
                strand_.wrap(
                    std::bind(
                        &TNioSession::OnConnect,
                        this->shared_from_this()
                    )
                )
            );
        }
    }
    
    template<>
    void Connect<false>(const char* ip, uint16_t port) {
        auto ref_holder = GetSocketReference();
        if (ref_holder) {
            ref_holder->async_connect(
                asio::ip::tcp::endpoint{ asio::ip::make_address(ip), port },
                std::bind(
                    &TNioSession::OnConnect,
                    this->shared_from_this()
                )
            );
        }
    }

    template<bool Strand = Strand>
    void Send(OutPacket& out_packet, bool encode, bool immediate) {
        const auto& ref_holder = GetSocketReference();
        if (!ref_holder)
            return;
        std::lock_guard<std::recursive_mutex> lock(send_queue_mutex_);
        if (encode) {
            cipher_.Encode(*this, out_packet);
        }
        if (immediate || send_queue_.empty()) {
            asio::async_write(
                *ref_holder,
                asio::buffer(out_packet.GetHeapData(), out_packet.GetLength()),
                strand_.wrap(std::bind(
                    &TNioSession::OnSend,
                    this->shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2
                ))
            );
            send_release_queue_.push_back(out_packet);
        } else {
            send_queue_.push_back(out_packet);
        }
    }

    template<>
    void Send<false>(OutPacket& out_packet, bool encode, bool immediate) {
        const auto& ref_holder = GetSocketReference();
        if (!ref_holder)
            return;
        std::lock_guard<std::recursive_mutex> lock(send_queue_mutex_);
        if (encode) {
            cipher_.Encode(*this, out_packet);
        }
        if (immediate || send_queue_.empty()) {
            asio::async_write(
                *ref_holder,
                asio::buffer(out_packet.GetHeapData(), out_packet.GetLength()),
                std::bind(
                    &TNioSession::OnSend,
                    this->shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2
                )
            );
            send_release_queue_.push_back(out_packet);
        } else {
            send_queue_.push_back(out_packet);
        }
    }
    
    template<bool Strand = Strand>
    void Read() {
        const auto& ref_holder = GetSocketReference();
        if (!ref_holder)
            return;
        ref_holder->async_read_some(
            asio::buffer(recv_buffer_, RecvBufferLength),
            strand_.wrap(
                std::bind(
                    &TNioSession::OnRead<Strand>,
                    this->shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2
                )
            )
        );
    }
    
    template<>
    void Read<false>() {
        const auto& ref_holder = GetSocketReference();
        if (!ref_holder)
            return;
        ref_holder->async_read_some(
            asio::buffer(recv_buffer_, RecvBufferLength),
            std::bind(
                &TNioSession::OnRead<false>,
                this->shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
    }

    void Close() {
        std::lock_guard<std::recursive_mutex> lock(socket_ref_mutex_);
        if (socket_) {
            socket_->close();
            socket_.reset();
        }
    }

    bool IsOpen() const {
        std::lock_guard<std::recursive_mutex> lock(socket_ref_mutex_);
        if (!socket_)
            return false;
        return socket_->is_open();
    }
    void Clear() {
        remote_address_.clear();
        buffer_.Clear();
        in_packet_.Clear();
        memset(recv_buffer_, 0, RecvBufferLength);
    }
public:
    int64_t GetId() const {
        return id_;
    }

    const std::string& GetRemoteAddress() const {
        return remote_address_;
    }

    void SetSocket(const std::shared_ptr<asio::ip::tcp::socket>& socket) {
        std::lock_guard<std::recursive_mutex> lock(socket_ref_mutex_);
        socket_ = socket;
    }

    void OnConnect() 
    {
        Clear();
        {
            std::lock_guard<std::recursive_mutex> lock(socket_ref_mutex_);
            remote_address_ = socket_->remote_endpoint().address().to_string();
            local_port_ = socket_->local_endpoint().port();
        }
        Handler::OnActive(*this);
        this->Read();
    }

    void OnClose()
    {
        Handler::OnClose(*this);
    }

    void OnError(const ERROR_CODE& ec) 
    {
        Handler::OnError(ec.value(), ec.message().c_str());
        Close();
    }

    uint16_t GetLocalPort() const 
    {
        return local_port_;
    }

    int32_t GetState() const 
    {
        return state_;
    }

    void SetState(int32_t new_state)
    {
        state_ = new_state;
    }

    const std::string& GetClientKey() const {
        return client_key_;
    }

    void SetClientKey(const std::string& key) {
        client_key_ = key;
    }
private:
    void OnSend(const ERROR_CODE& ec, size_t bytes_transferred)
    {
        const auto& ref_holder = GetSocketReference();
        if (!ref_holder) {
            return;
        }
        if (ec) {
            OnError(ec);
            return;
        }
        std::lock_guard<std::recursive_mutex> lock(send_queue_mutex_);
        send_release_queue_.clear();
        if (send_queue_.empty() == false) {
            OutPacket& out = send_queue_.front();
            send_queue_.pop_front();
            this->Send(out, true, true);
        }
    }
#include <iostream>
#include <sstream>
    template<bool Strand = Strand>
    void OnRead(const ERROR_CODE& ec, size_t bytes_transferred)
    {
        const auto& ref_holder = GetSocketReference();
        if (!ref_holder) {
            return;
        }
        if (ec) {
            OnError(ec);
            return;
        }

        try
        {
            if (bytes_transferred > 1000) {
                std::stringstream ss;
                ss << "bytes_transferred: " << bytes_transferred << '\n';
                std::cout << ss.str();
            }
            buffer_.Append(recv_buffer_, bytes_transferred);
            while (true)
            {
                cipher_.Decode(*this, buffer_, in_packet_);
                if (in_packet_.HasData()) {
                    Handler::ProcessPacket(*this, in_packet_.Clone());
                    in_packet_.Clear();
                } else {
                    break;
                }
            }
            Read<Strand>();
        } 
        catch (const std::exception & e)
        {
            Handler::OnError(-1, e.what());
            Close();
            return;
        }
    }

    std::shared_ptr<asio::ip::tcp::socket> GetSocketReference() {
        std::shared_ptr<asio::ip::tcp::socket> ref_holder;
        {
            std::lock_guard<std::recursive_mutex> lock(socket_ref_mutex_);
            if (!socket_) {
                return ref_holder;
            }
            ref_holder = socket_;
        }
        return ref_holder;
    }
private:
    int64_t id_;
    int32_t state_;
    std::string remote_address_;
    std::string client_key_;
    uint16_t local_port_;
    mutable std::recursive_mutex socket_ref_mutex_;
    std::shared_ptr<asio::ip::tcp::socket> socket_;
    asio::io_context::strand strand_;

    mutable std::recursive_mutex send_queue_mutex_;
    std::deque<OutPacket> send_queue_;
    std::vector<OutPacket> send_release_queue_;

    Cipher cipher_;
    Buffer buffer_;
    InPacket in_packet_;
    char internal_buffer_[InternalBufferLength];
    char recv_buffer_[RecvBufferLength];
};