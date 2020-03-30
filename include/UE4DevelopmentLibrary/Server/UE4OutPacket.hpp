#pragma once
#include "../Nio.hpp"

class UE4OutPacket : public NioOutPacket
{
public:
    explicit UE4OutPacket(NioOutPacket::AllocBufferLength length = NioOutPacket::AllocBufferLength::k64Bytes)
        : NioOutPacket(length)
    {
        this->SetAccesOffset(2);
    }
    void MakePacketHead()
    {
        *reinterpret_cast<uint16_t*>(this->buffer_) = GetAccessOffset();
    }
};