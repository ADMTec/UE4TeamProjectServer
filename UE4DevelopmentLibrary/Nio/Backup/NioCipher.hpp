#pragma once
#include "NioBase.hpp"


class NioSession;
class NioInternalBuffer;
class NioInPacket;
class NioOutPacket;

class NioCipher
{
public:
    NioCipher() = default;
    virtual ~NioCipher() = default;
public:
    virtual int64_t GetHead(NioInternalBuffer& buffer);
    virtual int64_t GetPacketLength(int64_t head);
    virtual void Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet);
    virtual void Encode(NioSession& session, NioOutPacket& out_packet);
};