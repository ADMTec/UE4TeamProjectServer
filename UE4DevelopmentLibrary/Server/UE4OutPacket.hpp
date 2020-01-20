#pragma once
#include "../Nio.hpp"

class UE4OutPacket : public NioOutPacket
{
public:
    explicit UE4OutPacket(NioOutPacket::AllocBufferLength length = NioOutPacket::AllocBufferLength::k64Bytes);
    virtual void MakePacketHead() override;
};