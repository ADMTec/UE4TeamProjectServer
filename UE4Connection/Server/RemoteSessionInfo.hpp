#pragma once
#include "../Stream.hpp"
#include "../Time.hpp"
#include <string>
#include <vector>


class RemoteSessionInfo : public SerializeInterface
{
public:
    using ip_t = std::string;
    using id_t = std::string;
public:
    RemoteSessionInfo(const RemoteSessionInfo& rhs);
    void operator=(const RemoteSessionInfo& rhs);
    RemoteSessionInfo(const std::string& ip, const std::string& id);
    RemoteSessionInfo();

    const std::string& GetIp() const;
    void SetIp(const std::string& ip);
    const std::string& GetId() const;
    void SetId(const std::string id);
    const Clock& GetClock() const;
    int32_t GetAccid() const;
    void SetAccid(int32_t accid);
    int32_t GetCid() const;
    void SetCid(int32_t cid);
public:
    virtual void Write(OutputStream& outpacket) const;
    virtual void Read(InputStream& inpacket);
private:
    std::string ip_;
    std::string id_;
    Clock time_;
    int32_t accid_;
    int32_t cid_;
};