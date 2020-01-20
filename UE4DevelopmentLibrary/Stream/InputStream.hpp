#pragma once
#include "ByteStream.hpp"


class InputStream : public ByteStream
{
public:
    InputStream(const InputStream& rhs);
    void operator=(const InputStream& rhs);
    InputStream(InputStream&& rhs) noexcept;
    void operator=(InputStream&& rhs) noexcept;
public:
    InputStream();
    ~InputStream() = default;
public:
    InputStream& operator>>(int8_t& data);
    int8_t ReadInt8();
    int8_t PeekInt8();

    InputStream& operator>>(int16_t& data);
    int16_t ReadInt16();
    int16_t PeekInt16();

    InputStream& operator>>(int32_t& data);
    int32_t ReadInt32();
    int32_t PeekInt32();

    InputStream& operator>>(int64_t& data);
    int64_t ReadInt64();
    int64_t PeekInt64();

    InputStream& operator>>(float& data);
    float ReadFloat32();
    float PeekFloat32();

    InputStream& operator>>(double& data);
    double ReadFloat64();
    double PeekFloat64();

    InputStream& operator>>(SerializeInterface& serialize_object);
    void Read(SerializeInterface& serialize_object);

    InputStream& operator>>(std::string& data);
    std::string ReadString();
    std::string ReadString(uint64_t str_length);
    std::string PeekString();
    std::string PeekString(uint64_t str_length);

    void ReadBytes(uint64_t num_read_byte, void* out_buffer);
    void PeekBytes(uint64_t num_read_byte, void* out_buffer);
};