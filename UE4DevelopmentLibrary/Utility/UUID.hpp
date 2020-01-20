#pragma once
#include "../Stream.hpp"
#include <string>
#include <cstdint>
#include <utility>


class __UUID : public SerializeInterface
{
public:
    __UUID(const __UUID& rhs);
    void operator=(const __UUID& rhs);
    __UUID(__UUID&& rhs) noexcept;
    void operator=(__UUID&& rhs) noexcept;
private:
    __UUID();
public:
    bool operator==(const __UUID& other) const;
    const std::string& ToString() const;
    void Clear();

    virtual void Write(OutputStream& outpacket) const override;
    virtual void Read(InputStream& inpacket) override;
public:
    static __UUID Generate();
private:
    std::string str_data_;
};

namespace std {
    template<>
    struct hash<__UUID> {
        size_t operator()(const __UUID& uuid) const {
            return std::hash<std::string>()(uuid.ToString());
        }
    };
}