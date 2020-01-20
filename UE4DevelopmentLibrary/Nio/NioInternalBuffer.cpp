#include "NioInternalBuffer.hpp"
#include "Exception.hpp"
#include "NioInPacket.hpp"
#include <algorithm>
#include <sstream>


NioInternalBuffer::NioInternalBuffer() : memory_block_size_(0)
{
}

NioInternalBuffer::~NioInternalBuffer()
{
}

void NioInternalBuffer::Initialize(char* memory_block, uint64_t memory_block_size)
{
    buffer_ = memory_block;
    buffer_length_ = 0;
    memory_block_size_ = memory_block_size;
}

void NioInternalBuffer::Append(const char* buffer, uint64_t length)
{
    if (length + buffer_length_ > memory_block_size_) {
        throw StackTraceException(ExceptionType::kOverflow, "Appending buffer -> bufferoverflow");
    }
    memcpy(buffer_ + buffer_length_, buffer, length);
    buffer_length_ += length;
}

void NioInternalBuffer::InitializePacket(NioInPacket& in_packet, uint64_t mark_index)
{
    in_packet.Initialize(buffer_, mark_index);
}

void NioInternalBuffer::MarkReadIndex(uint64_t mark_index)
{
    MoveMemory(buffer_, buffer_ + mark_index, buffer_length_ - mark_index);
    buffer_length_ -= mark_index;
    access_offset_ = 0;
    if (buffer_length_ > 0) {
        memset(buffer_ + buffer_length_, 0, memory_block_size_ - buffer_length_);
    }
}


uint64_t NioInternalBuffer::GetMemoryBlockSize() const
{
    return memory_block_size_;
}

uint64_t NioInternalBuffer::GetReadableLength() const
{
    return buffer_length_ - access_offset_;
}

void NioInternalBuffer::ResetReadOffset()
{
    access_offset_ = 0;
}

uint64_t NioInternalBuffer::GetReceiveLength() const
{
    return buffer_length_;
}

void NioInternalBuffer::Clear()
{
    buffer_length_ = 0;
    access_offset_ = 0;
}