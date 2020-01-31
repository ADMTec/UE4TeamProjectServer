#pragma once
#include "ByteStream.hpp"
#include <memory>

UE4CONNECTION_DLLEXPORT template class UE4CONNECTION_DLLCLASS std::shared_ptr<char[]>;

class UE4CONNECTION_DLLCLASS OutputStream : public ByteStream
{
public:
    enum class AllocBufferLength : uint32_t {
        k64Bytes = 64,
        k128Bytes = 128,
        k256Bytes = 256,
        k512Bytes = 512,
        k1024Bytes = 1024,
        k2048Bytes = 2048,
    };
private:
    using ByteStream::GetData;
    using ByteStream::Initialize;
    using ByteStream::StreamValidCheck;
public:
    OutputStream(const OutputStream& rhs);
    void operator=(const OutputStream& rhs);
    OutputStream(OutputStream&& rhs) noexcept;
    void operator=(OutputStream&& rhs) noexcept;
public:
    explicit OutputStream(OutputStream::AllocBufferLength length = AllocBufferLength::k128Bytes);
    ~OutputStream() = default;

    OutputStream& operator<<(int8_t data);
    void WriteInt8(int8_t data);

    OutputStream& operator<<(int16_t data);
    void WriteInt16(int16_t data);

    OutputStream& operator<<(int32_t data);
    void WriteInt32(int32_t data);

    OutputStream& operator<<(int64_t data);
    void WriteInt64(int64_t data);

    OutputStream& operator<<(float data);
    void WriteFloat32(float data);

    OutputStream& operator<<(double data);
    void WriteFloat64(double data);

    OutputStream& operator<<(const SerializeInterface& data);
    void Write(const SerializeInterface& data);

    OutputStream& operator<<(bool data);
    void WriteBool(bool data);
    void WriteBytes(const char* bytes, size_t length);

    OutputStream& operator<<(const std::string& str);
    void WriteString(const std::string& str);
    void WriteFromStringBytesArray(const std::string& string_bytes);
    void WriteZeroBytes(size_t size);

    std::shared_ptr<char[]> GetSharedBuffer() const;
private:
    void BufferIfShortReallocMemory(uint64_t write_size);
private:
    std::shared_ptr<char[]> heap_buffer_;
};