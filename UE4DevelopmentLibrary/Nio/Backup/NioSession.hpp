#pragma once
#include "NioBase.hpp"
#include "NioOutPacket.hpp"
#include <string>

class NioCipher;
class NioInternalBuffer;
class NioInPacket;
class NioEventHandler;
class NioSession;
class NioLogger;

class NioSession : public std::enable_shared_from_this<NioSession>
{
    constexpr static const uint32_t recv_buffer_length_ = 2048;
public:
    NioSession(const NioSession&) = delete;
    void operator=(const NioSession&) = delete;
public:
    NioSession();
    explicit NioSession(int64_t id);
    NioSession(int64_t id, const std::shared_ptr<NioLogger>& logger);

    bool SyncConnect(const char* ip, uint16_t port);
    void Connect(const char* ip, uint16_t port);
    void AsyncSend(NioOutPacket& out_packet, bool immediate, bool encode);
    void AsyncRead();
    void Close();

    bool IsOpen() const;
    void Clear();

    void OnConnected();
    void OnClosed();
    void OnError(const boost::system::error_code& ec);
public: // getter, setter
    int64_t GetId() const;
    const std::string& GetRemoteAddress() const;
    void SetClinetKey(const std::string& key);
    const std::string& GetClientKey() const;
    void SetSocketContext(const std::shared_ptr<NioSocket>& socket);
    void SetPacketCipher(const std::shared_ptr<NioCipher>& cipher);
    void SetInternalBuffer(const std::shared_ptr<NioInternalBuffer>& internal_buffer);
    void SetInPacket(const std::shared_ptr<NioInPacket>& in_packet);
    const std::shared_ptr<NioEventHandler>& GetEventHandler();
    void SetEventHandler(const std::shared_ptr<NioEventHandler>& event_handler);
private:
    void HandleReceiveCompletion(const boost::system::error_code& ec, size_t bytes_transferred);
    void HandleSendCompletion(const boost::system::error_code& ec, size_t bytes_transferred);
private:
    std::shared_ptr<NioSocket> GetSocketReference() const;

private:
    std::weak_ptr<NioLogger> weak_logger_;
    int64_t id_;
    std::string remote_address_;
    std::string client_key_;

    std::shared_ptr<NioCipher> packet_cipher_;
    std::shared_ptr<NioInternalBuffer> packet_buffer_;
    std::shared_ptr<NioInPacket> in_packet_;
    std::shared_ptr<NioEventHandler> event_handler_;

    mutable std::recursive_mutex socket_ref_mutex_;
    std::shared_ptr<NioSocket> socket_;

    mutable std::recursive_mutex send_queue_mutex_;
    std::deque<NioOutPacket> send_queue_;
    std::vector<NioOutPacket> send_release_queue_;

    char recv_buffer_[recv_buffer_length_];
};