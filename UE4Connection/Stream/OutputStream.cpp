#include "OutputStream.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <vector>

OutputStream::OutputStream(const OutputStream& rhs)
{
    OutputStream::operator=(rhs);
}

void OutputStream::operator=(const OutputStream& rhs)
{
    ByteStream::operator=(rhs);
    heap_buffer_ = rhs.heap_buffer_;
}

OutputStream::OutputStream(OutputStream&& rhs) noexcept
{
    OutputStream::operator=(std::move(rhs));
}

void OutputStream::operator=(OutputStream&& rhs) noexcept
{
    ByteStream::operator=(std::move(rhs));
    heap_buffer_ = std::move(rhs.heap_buffer_);
}

OutputStream::OutputStream(OutputStream::AllocBufferLength length)
    : ByteStream()
{
    heap_buffer_ = std::shared_ptr<char[]>(
        new char[static_cast<uint32_t>(length)], [](char* ptr) { delete[] ptr; });
    buffer_ = heap_buffer_.get();
    buffer_length_ = static_cast<uint32_t>(length);
    access_offset_ = 0;
}

OutputStream& OutputStream::operator<<(int8_t data)
{
    OutputStream::WriteInt8(data);
    return *this;
}

void OutputStream::WriteInt8(int8_t data)
{
    BufferIfShortReallocMemory(sizeof(int8_t));
    *reinterpret_cast<int8_t*>(buffer_ + access_offset_) = data;
    access_offset_ += sizeof(int8_t);
}

OutputStream& OutputStream::operator<<(int16_t data)
{
    OutputStream::WriteInt16(data);
    return *this;
}

void OutputStream::WriteInt16(int16_t data)
{
    BufferIfShortReallocMemory(sizeof(int16_t));
    *reinterpret_cast<int16_t*>(buffer_ + access_offset_) = data;
    access_offset_ += sizeof(int16_t);
}

OutputStream& OutputStream::operator<<(int32_t data)
{
    OutputStream::WriteInt32(data);
    return *this;
}

void OutputStream::WriteInt32(int32_t data)
{
    BufferIfShortReallocMemory(sizeof(int32_t));
    *reinterpret_cast<int32_t*>(buffer_ + access_offset_) = data;
    access_offset_ += sizeof(int32_t);
}

OutputStream& OutputStream::operator<<(int64_t data)
{
    OutputStream::WriteInt64(data);
    return *this;
}

void OutputStream::WriteInt64(int64_t data)
{
    BufferIfShortReallocMemory(sizeof(int64_t));
    *reinterpret_cast<int64_t*>(buffer_ + access_offset_) = data;
    access_offset_ += sizeof(int64_t);
}

OutputStream& OutputStream::operator<<(float data)
{
    OutputStream::WriteFloat32(data);
    return *this;
}

void OutputStream::WriteFloat32(float data)
{
    BufferIfShortReallocMemory(sizeof(float));
    *reinterpret_cast<float*>(buffer_ + access_offset_) = data;
    access_offset_ += sizeof(float);
}


OutputStream& OutputStream::operator<<(double data)
{
    OutputStream::WriteFloat64(data);
    return *this;
}

void OutputStream::WriteFloat64(double data)
{
    BufferIfShortReallocMemory(sizeof(double));
    *reinterpret_cast<double*>(buffer_ + access_offset_) = data;
    access_offset_ += sizeof(double);
}

OutputStream& OutputStream::operator<<(const SerializeInterface& data)
{
    OutputStream::Write(data);
    return *this;
}

void OutputStream::Write(const SerializeInterface& data)
{
    data.Write(*this);
}

OutputStream& OutputStream::operator<<(bool data)
{
    OutputStream::WriteBool(data);
    return *this;
}

void OutputStream::WriteBool(bool data) {
    WriteInt8(data);
}

void OutputStream::WriteBytes(const char* bytes, size_t length) 
{
    BufferIfShortReallocMemory(length);
    memcpy(buffer_ + access_offset_, bytes, length);
    access_offset_ += length;
}

OutputStream& OutputStream::operator<<(const std::string& str)
{
    OutputStream::WriteString(str);
    return *this;
}

void OutputStream::WriteString(const std::string& str)
{
    char* data = (char*)str.c_str();
    WriteBytes(data, strlen(data) + 1);
}

void OutputStream::WriteZeroBytes(size_t size)
{
    BufferIfShortReallocMemory(size);
    memset(buffer_ + access_offset_, 0, size);
    access_offset_ += size;
}

std::shared_ptr<char[]> OutputStream::GetSharedBuffer() const
{
    return heap_buffer_;
}

void OutputStream::BufferIfShortReallocMemory(uint64_t write_size)
{
    if (access_offset_ + write_size > buffer_length_)
    {
        auto temp = std::shared_ptr<char[]>(new char[buffer_length_ * 2], [](char* ptr) { delete[] ptr; });
        memcpy(temp.get(), buffer_, buffer_length_);
        heap_buffer_ = temp;
        buffer_ = heap_buffer_.get();
        buffer_length_ = (buffer_length_ * 2);
    }
}

void OutputStream::WriteFromStringBytesArray(const std::string& string_bytes)
{
    std::vector<std::string> result;
    boost::split(result, string_bytes, boost::is_any_of(" "));
    for (size_t i = 0; i < result.size(); ++i) {
        WriteInt8(std::atoi(result[i].c_str()));
    }
}