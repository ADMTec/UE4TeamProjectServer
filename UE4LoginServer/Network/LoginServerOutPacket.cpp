#include "LoginServerOutPacket.hpp"

LoginServerOutPacket::LoginServerOutPacket(NioOutPacket::AllocBufferLength length)
    : NioOutPacket(length)
{
    this->SetAccesOffset(2);
}

void LoginServerOutPacket::MakePacketHead()
{
    *reinterpret_cast<uint16_t*>(this->buffer_) = GetAccessOffset();
}