#include "NioOutPacket.hpp"
#include "../Exception.hpp"


NioOutPacket::NioOutPacket(const NioOutPacket& rhs)
    : OutputStream(rhs)
{
}

void NioOutPacket::operator=(const NioOutPacket& rhs)
{

    OutputStream::operator=(rhs);
}

NioOutPacket::NioOutPacket(OutputStream::AllocBufferLength length)
    : NioOutPacket(2, length)
{
}

NioOutPacket::NioOutPacket(uint64_t access_offset, OutputStream::AllocBufferLength length)
    : OutputStream(length)
{
    access_offset_ = access_offset;
}

NioOutPacket::~NioOutPacket()
{
}


char* NioOutPacket::GetHeapData()
{
    if (!GetSharedBuffer())
        throw StackTraceException(ExceptionType::kNullPointer, "send buffer is nullptr");
    return GetSharedBuffer().get();
}

uint64_t NioOutPacket::GetLength() const
{
    return access_offset_;
}

void NioOutPacket::MakePacketHead()
{
    *reinterpret_cast<uint16_t*>(GetHeapData()) = GetLength();
}