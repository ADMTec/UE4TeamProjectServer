#pragma once
#include "../Stream/OutputStream.hpp"


class NioOutPacket : public OutputStream
{
public:
    NioOutPacket(const NioOutPacket& rhs);
public:
    explicit NioOutPacket(OutputStream::AllocBufferLength length = OutputStream::AllocBufferLength::k128Bytes);
    explicit NioOutPacket(uint64_t access_offset, OutputStream::AllocBufferLength length = OutputStream::AllocBufferLength::k128Bytes);
    virtual ~NioOutPacket();
    void operator=(const NioOutPacket& rhs);

    char* GetHeapData();
    uint64_t GetLength() const;
    virtual void MakePacketHead();
};