#include "NioInPacket.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include <tbb/scalable_allocator.h>


NioInPacket::NioInPacket(const NioInPacket& rhs)
{
    NioInPacket::operator=(rhs);
}

void NioInPacket::operator=(const NioInPacket& rhs)
{
    buffer_ = (char*)scalable_malloc(rhs.buffer_length_);
    alloc_buffer_length_ = rhs.buffer_length_;
    memcpy(buffer_, rhs.buffer_, rhs.buffer_length_);
    buffer_length_ = rhs.buffer_length_;
    access_offset_ = rhs.access_offset_;
    has_data_ = rhs.has_data_;
}

std::shared_ptr<NioInPacket> NioInPacket::Clone()
{
    return std::shared_ptr<NioInPacket>(
        new NioInPacket(*this), [](NioInPacket* ptr) { delete ptr; });
}

NioInPacket::NioInPacket()
    : NioInPacket(1024)
{
}

NioInPacket::NioInPacket(uint64_t default_buffer_length)
    : InputStream(), alloc_buffer_length_(default_buffer_length), has_data_(false)
{
    buffer_ = (char*)scalable_malloc(alloc_buffer_length_);
    buffer_length_ = alloc_buffer_length_;
}

NioInPacket::~NioInPacket()
{
    if (buffer_ != nullptr) {
        scalable_free(buffer_);
    }
}

void NioInPacket::Initialize(char* buffer, uint64_t length)
{
    if (length > alloc_buffer_length_ || length == 0) {
        throw StackTraceException(ExceptionType::kRunTimeError, "packet buffer length error");
    }
    memcpy(buffer_, buffer, length);
    buffer_length_ = length;
    access_offset_ = 0;
    has_data_ = true;
}

bool NioInPacket::HasData()
{
    return has_data_;
}

void NioInPacket::Clear()
{
    buffer_length_ = alloc_buffer_length_;
    if (buffer_ != nullptr) {
        memset(buffer_, 0, buffer_length_);
    }
    access_offset_ = 0;
    has_data_ = false;
}

std::string NioInPacket::GetDebugString()
{
    if(buffer_ != nullptr) {
        return ByteStream::GetDebugString(buffer_, buffer_length_);
    } else {
        return std::string();
    }
}