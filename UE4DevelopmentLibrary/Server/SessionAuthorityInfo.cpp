#include "SessionAuthorityInfo.hpp"

SessionAuthorityInfo::SessionAuthorityInfo(const SessionAuthorityInfo& rhs)
{
    SessionAuthorityInfo::operator=(rhs);
}

void SessionAuthorityInfo::operator=(const SessionAuthorityInfo& rhs)
{
    ip_ = rhs.ip_;
    id_ = rhs.id_;
    time_ = rhs.time_;
}

SessionAuthorityInfo::SessionAuthorityInfo(const std::string& ip, const std::string& id)
    : ip_(ip), id_(id)
{
}

SessionAuthorityInfo::SessionAuthorityInfo()
{
}

const std::string& SessionAuthorityInfo::GetIp() const
{
    return ip_;
}

const std::string& SessionAuthorityInfo::GetId() const
{
    return id_;
}

const Clock& SessionAuthorityInfo::GetClock() const
{
    return time_;
}

void SessionAuthorityInfo::Write(OutputStream& outpacket) const
{
    outpacket << ip_;
    outpacket << id_;
    outpacket << time_.to_time_t();
}

void SessionAuthorityInfo::Read(InputStream& inpacket)
{
    inpacket >> ip_;
    inpacket >> id_;

    time_t time;
    inpacket >> time;
    time_ = Clock(time);
}