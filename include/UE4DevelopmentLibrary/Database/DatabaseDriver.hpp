#pragma once
#include "base.hpp"


class Connection;


class DatabaseDriver
{
public:
    virtual ~DatabaseDriver() = 0;
    virtual void Initialize() = 0;
    virtual std::shared_ptr<Connection> Connect(const string_t& addr, const string_t& id, const string_t& pw) = 0;
};

class ODBCDriver : public DatabaseDriver
{
public:
    ODBCDriver(const ODBCDriver&) = delete;
    void operator=(const ODBCDriver&) = delete;
public:
    ODBCDriver();
    ~ODBCDriver();
    virtual void Initialize() override;
    virtual std::shared_ptr<Connection> Connect(const string_t& addr, const string_t& id, const string_t& pw) override;
private:
    SQLHENV environment_handle_;
};