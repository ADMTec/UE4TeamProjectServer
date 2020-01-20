#pragma once
#include "base.hpp"


class PreparedStatement;

class Connection
{
public:
    virtual ~Connection();
    virtual void Connect(const string_t& addr, const string_t& id, const string_t& pw) = 0;
    virtual void SetSchema() = 0;
    virtual std::unique_ptr<PreparedStatement> GetPreparedStatement() = 0;
};

class ODBCConnection : public Connection
{
public:
    ODBCConnection(const ODBCConnection&) = delete;
    void operator=(const ODBCConnection&) = delete;
public:
    ODBCConnection(SQLHENV enviroment_handle);
    ~ODBCConnection();
    virtual void Connect(const string_t& addr, const string_t& id, const string_t& pw) override;
    virtual void SetSchema() override;
    virtual std::unique_ptr<PreparedStatement> GetPreparedStatement() override;
private:
    SQLHDBC connect_handle_;
};