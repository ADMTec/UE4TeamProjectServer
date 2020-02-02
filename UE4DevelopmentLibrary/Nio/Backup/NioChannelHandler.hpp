#pragma once
#include "NioCipher.hpp"
#include "NioEventHandler.hpp"


class NioChannelCipher final : public NioCipher
{
public:
    virtual int64_t GetHead(NioInternalBuffer& buffer) override;
    virtual int64_t GetPacketLength(int64_t head) override;
    virtual void Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet) override;
    virtual void Encode(NioSession& session, NioOutPacket& out_packet) override;
};


class NioChannelEventHandler final : public NioEventHandler
{
public:
    using handler_t = std::function<void(class NioSession&, class InputStream&)>;
public:
    NioChannelEventHandler(const NioChannelEventHandler&) = delete;
    void operator=(const NioChannelEventHandler&) = delete;
public:
    NioChannelEventHandler();
    ~NioChannelEventHandler();

    void BindFunction(int16_t identifier, const handler_t& handler);

    virtual void OnSessionActive(NioSession& session) override final;
    virtual void OnSessionError(NioSession& session, int error_code, const char* message) override final;
    virtual void OnSessionClose(NioSession& session) override final;
    virtual void ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet);
private:
    mutable std::shared_mutex binded_function_guard_;
    std::unordered_map<int16_t, handler_t> binded_function_;
};
