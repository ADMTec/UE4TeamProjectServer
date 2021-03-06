#pragma once
#include "../Stream.hpp"


enum class ServerType : int16_t;

class RemoteServerInfo : public SerializeInterface
{
public:
    RemoteServerInfo(const RemoteServerInfo& rhs);
    void operator=(const RemoteServerInfo& rhs);
public:
    RemoteServerInfo(ServerType type, const std::string& ip, uint16_t port);
    RemoteServerInfo() = default;

    ServerType GetServerType() const;
    void SetServerType(ServerType type);

    const std::string& GetIP() const;
    void SetIP(const std::string & ip);

    uint16_t GetPort() const;
#undef SetPort;
    void SetPort(uint16_t port);

    void SetMaxConnection(int64_t max_connection);
    int64_t GetMaxConnection() const;

    void SetCurrentConnection(int64_t current_connection);
    int64_t GetCurrentConnection() const;

    double GetCurrentCapacityRatio() const;
public:
    virtual void Write(OutputStream& outpacket) const;
    virtual void Read(InputStream& inpacket);
private:
    ServerType type_;
    std::string ip_;
    uint16_t port_;
    int64_t max_connection_;
    int64_t current_connection_;
};