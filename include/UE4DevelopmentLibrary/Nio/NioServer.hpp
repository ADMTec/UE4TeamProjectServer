#pragma once
#include "NioBase.hpp"
#include "NioThreadGroup.hpp"

class NioContext;
class NioAcceptor;
class NioSession;
class NioCipher;
class NioInternalBuffer;
class NioInPacket;
class NioOutPacket;
class NioEventHandler;
class NioChannel;
class NioLogger;


class NioServer : public std::enable_shared_from_this<NioServer>
{
    friend class NioServerBuilder;
public:
    NioServer(const NioServer&) = delete;
    void operator=(const NioServer&) = delete;
private:
    NioServer();
public:
    ~NioServer();
    void PrintCurrentSessionQueue();
    void Initialize();
    void Run();
    void Accept();

    void Clear();
    void Stop();
    void Exit();
    bool IsRunning() const;
    void Log(const char* msg);
    std::shared_ptr<NioSession> GetSession(int64_t index);
    void ReleaseSession(int64_t index);

    void SetNioContext(const std::shared_ptr<NioContext>& context);
    const std::shared_ptr<NioContext>& GetContext();
    const std::shared_ptr<NioEventHandler>& GetEventHandler();
    NioChannel& GetChannel();
    // return timer_key for manual cancel timer
    size_t CreateTimer(const std::function<void(void)>& func, std::chrono::milliseconds milli);
    size_t CancelTimer(size_t timer_key);
private:
    std::shared_ptr<NioContext> nio_context_;
    NioThreadGroup work_thread_group_;
    std::atomic<bool> is_running_;
    
    uint64_t max_connection_;
    uint16_t port_;
    uint64_t num_worker_;
    uint64_t buffer_size_;
    
    acceptor_ptr_t acceptor_;

    std::shared_ptr<NioLogger> logger_;
    std::shared_ptr<NioCipher> cipher_template_;
    std::shared_ptr<NioInPacket> in_packet_template_;
    std::shared_ptr<NioEventHandler> event_handler_template_;

    
    mutable std::shared_mutex session_pool_gaurd_;
    std::vector<std::shared_ptr<NioSession>> session_pool_;
    concurrency::concurrent_queue<int64_t> session_id_queue_;

    std::shared_ptr<NioChannel> channel_;

    mutable std::mutex timer_pool_guard_;
    std::unordered_map<size_t, std::shared_ptr<class NioSystemClockTimer>> timer_;
};

class NioServerBuilder
{
public:
    NioServerBuilder(const NioServerBuilder&) = delete;
    void operator=(const NioServerBuilder&) = delete;
public:
    NioServerBuilder();
    ~NioServerBuilder();
#undef SetPort;
    NioServerBuilder& SetPort(uint16_t port);
    NioServerBuilder& SetMaxConnection(uint64_t max_connection);
    NioServerBuilder& SetNioThreadCount(uint64_t nio_worker_thread_count);
    NioServerBuilder& SetNioLogger(const std::shared_ptr<NioLogger>& logger);
    NioServerBuilder& SetNioInternalBufferSize(uint64_t buffer_size);
    NioServerBuilder& SetNioPacketCipher(const std::shared_ptr<NioCipher>& cipher);
    NioServerBuilder& SetNioEventHandler(const std::shared_ptr<NioEventHandler>& event_handler);
    std::shared_ptr<NioServer> Build();
private:
    std::shared_ptr<NioServer> server_;
};