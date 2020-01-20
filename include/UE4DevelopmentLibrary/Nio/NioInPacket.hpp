#pragma once
#include "NioBase.hpp"
#include "../Stream.hpp"


class NioInPacket : public InputStream
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