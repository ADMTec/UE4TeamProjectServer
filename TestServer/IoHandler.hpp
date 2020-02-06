#pragma once
#include "UE4DevelopmentLibrary/Nio.hpp"

class PacketCipher : public NioCipher
{
public:
    virtual int64_t GetHead(NioInternalBuffer& buffer) override;
    virtual int64_t GetPacketLength(int64_t head) override;
    virtual void Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet) override;
    virtual void Encode(NioSession& session, NioOutPacket& out_packet) override;
};


class IoHandler : public NioEventHandler
{
public:
    virtual void OnSessionActive(NioSession& session) override;
    virtual void OnSessionError(NioSession& session, int error_code, const char* message) override;
    virtual void OnSessionClose(NioSession& session) override;
    virtual void ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet) override;
};

class OutPacket : public NioOutPacket
{
public:
    explicit OutPacket(NioOutPacket::AllocBufferLength length = NioOutPacket::AllocBufferLength::k64Bytes);
    virtual void MakePacketHead() override;
};