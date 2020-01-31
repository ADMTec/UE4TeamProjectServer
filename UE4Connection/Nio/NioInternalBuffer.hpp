#pragma once
#include "../Stream/InputStream.hpp"


class NioInPacket;

class UE4CONNECTION_DLLCLASS NioInternalBuffer : public InputStream
{
public:
    NioInternalBuffer(const NioInternalBuffer&) = delete;
    void operator=(const NioInternalBuffer&) = delete;
public:
    NioInternalBuffer();
    virtual ~NioInternalBuffer();

    void Initialize(char* memory_block, uint64_t memory_block_size);
    void Append(const char* buffer, uint64_t length);
    void InitializePacket(NioInPacket& in_packet, uint64_t mark_index);
    void MarkReadIndex(uint64_t mark_index);

    uint64_t GetMemoryBlockSize() const;
    uint64_t GetReadableLength() const;
    void ResetReadOffset();
    uint64_t GetReceiveLength() const;
    void Clear();
private:
    uint64_t memory_block_size_;
};