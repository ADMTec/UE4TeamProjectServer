#include "UUID.hpp"
#include <random>
#include <numeric>
#include <sstream>

__UUID::__UUID(const __UUID& rhs)
{
    __UUID::operator=(rhs);
}

void __UUID::operator=(const __UUID& rhs)
{
    str_data_ = rhs.str_data_;
}


__UUID::__UUID(__UUID&& rhs) noexcept
{
    __UUID::operator=(std::move(rhs));
}

void __UUID::operator=(__UUID&& rhs) noexcept
{
    str_data_ = std::move(rhs.str_data_);
}

__UUID::__UUID()
{
}

bool __UUID::operator==(const __UUID& other) const
{
    return str_data_ == other.str_data_;
}

bool __UUID::operator!=(const __UUID& other) const
{
    return str_data_ != other.str_data_;
}

const std::string& __UUID::ToString() const
{
    return str_data_;
}

void __UUID::Clear()
{
    str_data_.clear();
}

void __UUID::Write(OutputStream& out) const
{
    out << str_data_;
}

void __UUID::Read(InputStream& input)
{
    input >> str_data_;
}

__UUID __UUID::Generate()
{
    static thread_local std::random_device rnd;
    static thread_local std::mt19937 mt(rnd());
    static thread_local std::uniform_int_distribution<int> 
        dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    int32_t data[4];
    data[0] = dist(mt);
    data[1] = dist(mt);
    data[2] = dist(mt);
    data[3] = dist(mt);
    reinterpret_cast<char*>(data)[10] = 0x04;

    std::stringstream ss;
    ss << std::hex;
    const char* bytes = reinterpret_cast<const char*>(data);
    for (int i = 0; i < sizeof(data); ++i) {
        switch (i) {
            case 4:
            case 6:
            case 8:
            case 10:
                ss << '-';
            default:
                if ((bytes[i] & 0xFF) < 0x10) {
                    ss << '0';
                }
                ss << (bytes[i] & 0xFF);
        }
    }
    __UUID uuid;
    uuid.str_data_ = ss.str();
    return uuid;
}