#pragma once
#include "UE4DevelopmentLibrary/Nio.hpp"


class LoginServerPacketCipher : public NioCipher
{
public:
    virtual int64_t GetHead(NioInternalBuffer& buffer);
    virtual int64_t GetPacketLength(int64_t head);
    virtual void Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet);
    virtual void Encode(NioSession& session, NioOutPacket& out_packet);
};