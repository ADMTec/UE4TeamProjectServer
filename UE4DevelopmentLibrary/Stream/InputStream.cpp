#include "InputStream.hpp"


InputStream::InputStream(const InputStream& rhs)
{
    InputStream::operator=(rhs);
}

void InputStream::operator=(const InputStream& rhs)
{
    ByteStream::operator=(rhs);
}

InputStream::InputStream(InputStream&& rhs) noexcept
{
    InputStream::operator=(std::move(rhs));
}

void InputStream::operator=(InputStream&& rhs) noexcept
{
    ByteStream::operator=(std::move(rhs));
}

InputStream::InputStream()
    : ByteStream()
{
}

InputStream& InputStream::operator>>(int8_t& data)
{
    data = InputStream::ReadInt8();
    return *this;
}

int8_t InputStream::ReadInt8()
{
    int8_t value = PeekInt8();
    access_offset_ += sizeof(int8_t);
    return value;
}

int8_t InputStream::PeekInt8()
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, sizeof(int8_t));
    return *reinterpret_cast<int8_t*>(buffer_ + access_offset_);

}

InputStream& InputStream::operator>>(int16_t& data)
{
    data = InputStream::ReadInt16();
    return *this;
}

int16_t InputStream::ReadInt16()
{
    int16_t value = PeekInt16();
    access_offset_ += sizeof(int16_t);
    return value;
}

int16_t InputStream::PeekInt16()
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, sizeof(int16_t));
    return *reinterpret_cast<int16_t*>(buffer_ + access_offset_);
}

InputStream& InputStream::operator>>(int32_t& data)
{
    data = InputStream::ReadInt32();
    return *this;
}

int32_t InputStream::ReadInt32()
{
    int32_t value = PeekInt32();
    access_offset_ += sizeof(int32_t);
    return value;
}

int32_t InputStream::PeekInt32()
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, sizeof(int32_t));
    return *reinterpret_cast<int32_t*>(buffer_ + access_offset_);
}

InputStream& InputStream::operator>>(int64_t& data)
{
    data = InputStream::ReadInt64();
    return *this;
}

int64_t InputStream::ReadInt64()
{
    int64_t value = PeekInt64();
    access_offset_ += sizeof(int64_t);
    return value;
}

int64_t InputStream::PeekInt64()
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, sizeof(int64_t));
    return *reinterpret_cast<int64_t*>(buffer_ + access_offset_);
}

InputStream& InputStream::operator>>(float& data)
{
    data = InputStream::ReadFloat32();
    return *this;
}

float InputStream::ReadFloat32()
{
    float value = PeekFloat32();
    access_offset_ += sizeof(float);
    return value;
}

float InputStream::PeekFloat32()
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, sizeof(float));
    return *reinterpret_cast<float*>(buffer_ + access_offset_);
}

InputStream& InputStream::operator>>(double& data)
{
    data = InputStream::ReadFloat64();
    return *this;
}

double InputStream::ReadFloat64()
{
    double value = PeekFloat64();
    access_offset_ += sizeof(double);
    return value;
}

double InputStream::PeekFloat64()
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, sizeof(double));
    return *reinterpret_cast<double*>(buffer_ + access_offset_);
}

InputStream& InputStream::operator>>(SerializeInterface& serialize_object)
{
    InputStream::Read(serialize_object);
    return *this;
}

void InputStream::Read(SerializeInterface& serialize_object)
{
    serialize_object.Read(*this);
}

InputStream& InputStream::operator>>(std::string& data)
{
    data = InputStream::ReadString();
    return *this;
}

std::string InputStream::ReadString()
{
    return InputStream::ReadString(strlen(buffer_ + access_offset_) + 1);
}

std::string InputStream::ReadString(uint64_t str_length)
{
    std::string str = PeekString(str_length);
    access_offset_ += str_length;
    return str;
}

std::string InputStream::PeekString()
{
    return InputStream::PeekString(strlen(buffer_ + access_offset_) + 1);
}

std::string InputStream::PeekString(uint64_t str_length)
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, str_length);
    // 실제 std::string에는 null문자가 들어가지 않는다.
    return std::string(buffer_ + access_offset_, buffer_ + access_offset_ + str_length - 1);
}

void InputStream::ReadBytes(uint64_t num_read_byte, void* out_buffer)
{
    PeekBytes(num_read_byte, out_buffer);
    access_offset_ += num_read_byte;
}

void InputStream::PeekBytes(uint64_t num_read_byte, void* out_buffer)
{
    StreamValidCheck(buffer_, buffer_length_, access_offset_, num_read_byte);
    memcpy(out_buffer, buffer_ + access_offset_, num_read_byte);
}