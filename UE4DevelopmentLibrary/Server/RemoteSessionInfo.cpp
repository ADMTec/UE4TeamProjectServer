#include "RemoteSessionInfo.hpp"

RemoteSessionInfo::RemoteSessionInfo(const RemoteSessionInfo& rhs)
{
    RemoteSessionInfo::operator=(rhs);
}

void RemoteSessionInfo::operator=(const RemoteSessionInfo& rhs)
{
    ip_ = rhs.ip_;
    id_ = rhs.id_;
    time_ = rhs.time_;
    accid_ = rhs.accid_;
    cid_ = rhs.cid_;
}

RemoteSessionInfo::RemoteSessionInfo(const std::string& ip, const std::string& id)
    : ip_(ip), id_(id), accid_(-1), cid_(-1)
{
}

RemoteSessionInfo::RemoteSessionInfo()
    : accid_(-1), cid_(-1)
{
}

const std::string& RemoteSessionInfo::GetIp() const
{
    return ip_;
}

void RemoteSessionInfo::SetIp(const std::string& ip)
{
    ip_ = ip;
}

const std::string& RemoteSessionInfo::GetId() const
{
    return id_;
}

void RemoteSessionInfo::SetId(const std::string id)
{
    id_ = id;
}

const Clock& RemoteSessionInfo::GetClock() const
{
    return time_;
}

int32_t RemoteSessionInfo::GetAccid() const
{
    return accid_;
}

void RemoteSessionInfo::SetAccid(int32_t accid)
{
    accid_ = accid;
}

int32_t RemoteSessionInfo::GetCid() const
{
    return cid_;
}

void RemoteSessionInfo::SetCid(int32_t cid)
{
    cid_ = cid;
}

void RemoteSessionInfo::Write(OutputStream& outpacket) const
{
    outpacket << ip_;
    outpacket << id_;
    outpacket << time_.to_time_t();
    outpacket << accid_;
    outpacket << cid_;
}

void RemoteSessionInfo::Read(InputStream& inpacket)
{
    inpacket >> ip_;
    inpacket >> id_;


    time_t time;
    inpacket >> time;
    time_ = Clock(time);

    inpacket >> accid_;
    inpacket >> cid_;
}