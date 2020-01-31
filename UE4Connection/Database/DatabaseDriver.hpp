#pragma once
#include "base.hpp"
#include <functional>

class Connection;
UE4CONNECTION_DLLEXPORT template class UE4CONNECTION_DLLCLASS std::shared_ptr<Connection>;
UE4CONNECTION_DLLEXPORT template class UE4CONNECTION_DLLCLASS std::function<void(Connection*)>;

using deleter_t = std::function<void(Connection*)>;

class UE4CONNECTION_DLLCLASS DatabaseDriver
{
public:
    virtual ~DatabaseDriver() = 0;
    virtual void Initialize() = 0;
    virtual std::shared_ptr<Connection> Connect(const string_t& addr, const string_t& id, const string_t& pw, const deleter_t& deleter = nullptr) = 0;
};

class UE4CONNECTION_DLLCLASS ODBCDriver : public DatabaseDriver
{
public:
    ODBCDriver(const ODBCDriver&) = delete;
    void operator=(const ODBCDriver&) = delete;
public:
    ODBCDriver();
    ~ODBCDriver();
    virtual void Initialize() override;
    virtual std::shared_ptr<Connection> Connect(const string_t& addr, const string_t& id, const string_t& pw, const deleter_t& deleter = nullptr) override;
private:
    SQLHENV environment_handle_;
};