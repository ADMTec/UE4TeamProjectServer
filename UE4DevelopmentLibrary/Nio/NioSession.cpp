#include "NioSession.hpp"
#include "Private/NioSocket.hpp"
#include "Exception.hpp"
#include "NioCipher.hpp"
#include "NioInternalBuffer.hpp"
#include "NioInPacket.hpp"
#include "NioEventHandler.hpp"
#include "NIoLogger.hpp"
#include <iostream>


NioSession::NioSession()
    : NioSession(-1)
{
    remote_address_.reserve(16);
}

NioSession::NioSession(int64_t id)
    : id_(id)
{
    memset(recv_buffer_, 0, recv_buffer_length_);
}

NioSession::NioSession(int64_t id, const std::shared_ptr<NioLogger>& logger)
    : id_(id), weak_logger_(logger)
{
    memset(recv_buffer_, 0, recv_buffer_length_);
}

void NioSession::SetSocketContext(const std::shared_ptr<NioSocket>& socket)
{
    socket_ = socket;
}

void NioSession::SetPacketCipher(const std::shared_ptr<NioCipher>& cipher)
{
    packet_cipher_ = cipher;
}

void NioSession::SetInternalBuffer(const std::shared_ptr<NioInternalBuffer>& internal_buffer)
{
    packet_buffer_ = internal_buffer;
}

void NioSession::SetInPacket(const std::shared_ptr<NioInPacket>& in_packet)
{
    in_packet_ = in_packet;
}

const std::shared_ptr<NioEventHandler>& NioSession::GetEventHandler()
{
    return event_handler_;
}

void NioSession::SetEventHandler(const std::shared_ptr<NioEventHandler>& event_handler)
{
    event_handler_ = event_handler;
}

void NioSession::OnConnected()
{
    Clear();
    remote_address_ = socket_->GetRemoteAddress();
    try {
        event_handler_->OnSessionActive(*this);
    } catch (const std::exception & e) {
        auto shared = weak_logger_.lock();
        if (shared) {
            shared->Log(__func__, __LINE__, e.what());
        }
    }
    this->AsyncRead();
}

void NioSession::Connect(const char* ip, uint16_t port)
{
    socket_->Connect(ip, port,
        [shared = this->shared_from_this()](const boost::system::error_code& ec) {
            if (!ec) {
                shared->OnConnected();
            } else {
                shared->OnError(ec);
            }}
    );
}

void NioSession::AsyncSend(NioOutPacket& out_packet, bool immediate, bool encode)
{
    const auto& ref_holder = GetSocketReference();
    if (!ref_holder)
        return;
    std::lock_guard<std::recursive_mutex> lock(send_queue_mutex_);
    if (encode) { // 암호화 순서
        packet_cipher_->Encode(*this, out_packet);
    }
    if(immediate || send_queue_.empty()) {
        ref_holder->Send(
            out_packet.GetHeapData(), out_packet.GetLength(),
            std::bind(
                &NioSession::HandleSendCompletion,
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

void NioSession::AsyncRead()
{
    const auto& ref_holder = GetSocketReference();
    if (!ref_holder)
        return;
    ref_holder->Receive(
        recv_buffer_, recv_buffer_length_,
        std::bind(
            &NioSession::HandleReceiveCompletion,
            this->shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void NioSession::Close()
{
    std::lock_guard<std::recursive_mutex> lock(socket_ref_mutex_);
    if (socket_) {
        socket_->Close();
        socket_.reset();
    }
}

bool NioSession::IsOpen() const
{
    const auto& ref_holder = GetSocketReference();
    if (!ref_holder) {
        return false;
    }
    return ref_holder->IsOpen();
}

void NioSession::Clear()
{
    client_key_.clear();
    remote_address_.clear();
    packet_buffer_->Clear();
    in_packet_->Clear();
}

int64_t NioSession::GetId() const
{
    return id_;
}

const std::string& NioSession::GetRemoteAddress() const
{
    return remote_address_;
}

void NioSession::SetClinetKey(const std::string& key)
{
    client_key_ = key;
}

const std::string& NioSession::GetClientKey() const
{
    return client_key_;
}

void NioSession::OnClosed()
{
    event_handler_->OnSessionClose(*this);
}

void NioSession::OnError(const boost::system::error_code& ec)
{
    try {
        event_handler_->OnSessionError(*this, ec.value(), ec.message().c_str());
    } catch (const std::exception & e) {
        auto shared = weak_logger_.lock();
        if (shared) {
            shared->Log(__func__, __LINE__, e.what());
        }
    }
    Close();
    return;
}


void NioSession::HandleReceiveCompletion(const boost::system::error_code& ec, size_t bytes_transferred)
{
    const auto& ref_holder = GetSocketReference();
    if (!ref_holder) {
        return;
    }
    if (!event_handler_) {
        Close();
        return;
    }
    if (ec) {
        OnError(ec);
        return;
    }

    try
    {
        packet_buffer_->Append(recv_buffer_, bytes_transferred);
        while (true)
        {
            packet_cipher_->Decode(*this, *packet_buffer_.get(), *in_packet_.get());
            if (in_packet_->HasData()) {
                event_handler_->ProcessPacket(*this, in_packet_->Clone());
                in_packet_->Clear();
            }
            else {
                break;
            }
        }
        AsyncRead();
    }
    catch (const std::exception & e)
    {
        auto shared = weak_logger_.lock();
        if (shared) {
            shared->Log(__func__, __LINE__, e.what());
        }
        event_handler_->OnSessionError(*this, -1, e.what());
        Close();
        return;
    }
}

void NioSession::HandleSendCompletion(const boost::system::error_code& ec, size_t bytes_transferred)
{
    const auto& ref_holder = GetSocketReference();
    if (!ref_holder) {
        return;
    }
    if (!event_handler_) {
        return;
    }
    if (ec) {
        OnError(ec);
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(send_queue_mutex_);
    send_release_queue_.clear();
    if (send_queue_.empty() == false) {
        NioOutPacket& out = send_queue_.front();
        send_queue_.pop_front();
        this->AsyncSend(out, true, true);
    }
}

std::shared_ptr<NioSocket> NioSession::GetSocketReference() const
{
    std::shared_ptr<NioSocket> ref_holder;
    {
        std::lock_guard<std::recursive_mutex> lock(socket_ref_mutex_);
        if (!socket_) {
            return ref_holder;
        }
        ref_holder = socket_;
    }
    return ref_holder;
}