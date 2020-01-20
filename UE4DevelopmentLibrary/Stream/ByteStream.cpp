#include "ByteStream.hpp"
#include "../Exception.hpp"
#include <type_traits>
#include <algorithm>
#include <sstream>


ByteStream::ByteStream(const ByteStream& rhs)
{
    ByteStream::operator=(rhs);
}

void ByteStream::operator=(const ByteStream& rhs)
{
    buffer_ = rhs.buffer_;
    buffer_length_ = rhs.buffer_length_;
}

ByteStream::ByteStream(ByteStream&& rhs) noexcept
{
    ByteStream::operator=(std::move(rhs));
}

void ByteStream::operator=(ByteStream&& rhs) noexcept
{
    buffer_ = rhs.buffer_; rhs.buffer_ = nullptr;
    buffer_length_ = rhs.buffer_length_; rhs.buffer_length_ = 0;
}

ByteStream::ByteStream()
    : buffer_(nullptr), buffer_length_(0), access_offset_(0)
{
}

ByteStream::~ByteStream()
{
}

void ByteStream::Initialize(char* buffer, uint64_t buffer_length, uint64_t access_offset)
{
    buffer_ = buffer;
    buffer_length_ = buffer_length;
    access_offset_ = access_offset;
}

char* ByteStream::GetData()
{
    return buffer_;
}

uint64_t ByteStream::GetBufferLength() const
{
    return buffer_length_;
}

uint64_t ByteStream::GetAccessOffset() const
{
    return access_offset_;
}

void ByteStream::SetAccesOffset(uint64_t offset)
{
    if (offset >= buffer_length_) {
        throw StackTraceException(ExceptionType::kOutOfRange, "new index -> Out Of Range");
    }
    access_offset_ = offset;
}

std::string ByteStream::GetDebugString(const char* buffer, uint64_t length)
{
#undef min
    length = std::min((uint64_t)300, length);

    std::string str;
    str.reserve(length * 5 + 10);

    int32_t temp = 0;
    for (size_t i = 0; i < length; ++i) {
        std::stringstream ss;
        temp = buffer[i] & 0xFF;
        if (temp <= 0xF && temp >= 0) {
            ss << '0';
        }
        ss << std::hex << temp << " ";
        str += ss.str();
    }
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return str;
}

void ByteStream::StreamValidCheck(char* buffer, uint64_t buffer_length, uint64_t access_offset, uint64_t access_size)
{
    if (access_offset + access_size > buffer_length)
    {
        std::string packet_dump = GetDebugString(buffer, buffer_length);
        throw StackTraceException("buffer out of range - PacketDump", packet_dump.c_str());
    }
}