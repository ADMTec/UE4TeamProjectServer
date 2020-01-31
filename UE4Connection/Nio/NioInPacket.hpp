#pragma once
#include "../Stream/InputStream.hpp"
#include <memory>

class NioInPacket;
UE4CONNECTION_DLLEXPORT template class UE4CONNECTION_DLLCLASS std::shared_ptr<NioInPacket>;

class UE4CONNECTION_DLLCLASS NioInPacket : public InputStream
{
public:
    NioInPacket(const NioInPacket& rhs);
    void operator=(const NioInPacket& rhs);
    std::shared_ptr<NioInPacket> Clone();
public:
    NioInPacket();
    NioInPacket(uint64_t default_buffer_length);
    ~NioInPacket();

    void Initialize(char* buffer, uint64_t length);
    bool HasData();
    void Clear();

    std::string GetDebugString();
private:
    uint64_t alloc_buffer_length_;
    bool has_data_;
};