#include "UE4OutPacket.hpp"

UE4OutPacket::UE4OutPacket(NioOutPacket::AllocBufferLength length)
    : NioOutPacket(length)
{
    this->SetAccesOffset(2);
}

void UE4OutPacket::MakePacketHead()
{
    *reinterpret_cast<uint16_t*>(this->buffer_) = GetAccessOffset();
}