#pragma once
#include "base.hpp"
#include "PreparedStatement.hpp"


UE4CONNECTION_DLLEXPORT template class UE4CONNECTION_DLLCLASS std::shared_ptr<PreparedStatement>;

class UE4CONNECTION_DLLCLASS Connection
{
public:
    virtual ~Connection();
    virtual void Connect(const string_t& addr, const string_t& id, const string_t& pw) = 0;
    virtual void SetSchema() = 0;
    virtual std::shared_ptr<PreparedStatement> GetPreparedStatement() = 0;
};

class UE4CONNECTION_DLLCLASS ODBCConnection : public Connection
{
public:
    ODBCConnection(const ODBCConnection&) = delete;
    void operator=(const ODBCConnection&) = delete;
public:
    ODBCConnection(SQLHENV enviroment_handle);
    ~ODBCConnection();
    virtual void Connect(const string_t& addr, const string_t& id, const string_t& pw) override;
    virtual void SetSchema() override;
    virtual std::shared_ptr<PreparedStatement> GetPreparedStatement() override;
private:
    SQLHDBC connect_handle_;
};