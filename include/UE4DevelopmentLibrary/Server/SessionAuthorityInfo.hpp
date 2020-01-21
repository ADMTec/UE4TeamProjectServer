#pragma once
#include "../Stream.hpp"
#include "../Time.hpp"
#include <string>


class SessionAuthorityInfo : public SerializeInterface
{
public:
    using ip_t = std::string;
    using id_t = std::string;
public:
    SessionAuthorityInfo(const SessionAuthorityInfo& rhs);
    void operator=(const SessionAuthorityInfo& rhs);
    SessionAuthorityInfo(const std::string& ip, const std::string& id);
    SessionAuthorityInfo();

    const std::string& GetIp() const;
    const std::string& GetId() const;
    const Clock& GetClock() const;
public:
    virtual void Write(OutputStream& outpacket) const;
    virtual void Read(InputStream& inpacket);
private:
    std::string ip_;
    std::string id_;
    Clock time_;
};