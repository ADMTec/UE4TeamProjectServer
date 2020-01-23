#include "RemoteServerInfo.hpp"
#include "../Exception.hpp"
#include "../../InterServerOpcode.hpp"


RemoteServerInfo::RemoteServerInfo(const RemoteServerInfo& rhs)
{
    RemoteServerInfo::operator=(std::move(rhs));
}

void RemoteServerInfo::operator=(const RemoteServerInfo& rhs)
{
    type_ = rhs.type_;
    ip_ = rhs.ip_;
    port_ = rhs.port_;
    max_connection_ = rhs.max_connection_;
    current_connection_ = rhs.current_connection_;
    mediator_ = rhs.mediator_;
}

RemoteServerInfo::RemoteServerInfo(ServerType type, const std::string& ip, uint16_t port)
    : type_(type), ip_(ip), port_(port)
    , max_connection_(0), current_connection_(0)
{
}

ServerType RemoteServerInfo::GetServerType() const
{
    return type_;
}

void RemoteServerInfo::SetServerType(ServerType type)
{
    type_ = type;
}

const std::string& RemoteServerInfo::GetIP() const
{
    return ip_;
}

void RemoteServerInfo::SetIP(const std::string& ip)
{
    ip_ = ip;
}

uint16_t RemoteServerInfo::GetPort() const
{
    return port_;
}

void RemoteServerInfo::SetPort(uint16_t port)
{
    port_ = port;
}

void RemoteServerInfo::SetMaxConnection(int64_t max_connection)
{
    max_connection_ = max_connection;
}

int64_t RemoteServerInfo::GetMaxConnection() const
{
    return max_connection_;
}

void RemoteServerInfo::SetCurrentConnection(int64_t current_connection)
{
    current_connection_ = current_connection;
}

int64_t RemoteServerInfo::GetCurrentConnection() const
{
    return current_connection_;
}

double RemoteServerInfo::GetCurrentCapacityRatio() const
{
    if (max_connection_ == 0) {
        throw StackTraceException(ExceptionType::kLogicError, "max connection cannot be 0");
    }
    return static_cast<double>(current_connection_)/static_cast<double>(max_connection_);
}

const std::weak_ptr<UE4Client> RemoteServerInfo::GetWeak() const
{
    return mediator_;
}

void RemoteServerInfo::SetWeak(const std::weak_ptr<UE4Client>& weak)
{
    mediator_ = weak;
}

void RemoteServerInfo::Write(OutputStream& outpacket) const
{
    outpacket << static_cast<int16_t>(type_);
    outpacket << ip_;
    outpacket << (int16_t)port_;
    outpacket << max_connection_;
    outpacket << current_connection_;
}

void RemoteServerInfo::Read(InputStream& inpacket)
{
    inpacket >> *reinterpret_cast<int16_t*>(&type_);
    inpacket >> ip_;
    inpacket >> *reinterpret_cast<int16_t*>(&port_);
    inpacket >> max_connection_;
    inpacket >> current_connection_;
}
