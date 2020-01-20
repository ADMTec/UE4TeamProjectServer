#pragma once
#include "UE4DevelopmentLibrary/Nio.hpp"

class LoginServerOutPacket : public NioOutPacket
{
public:
    explicit LoginServerOutPacket(NioOutPacket::AllocBufferLength length = NioOutPacket::AllocBufferLength::k64Bytes);
    virtual void MakePacketHead() override;
};