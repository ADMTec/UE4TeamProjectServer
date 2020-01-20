#pragma once
#include "base.hpp"


class ResultSet;

class PreparedStatement
{
public:
    virtual ~PreparedStatement() = 0;
    virtual void PrepareStatement(const char_t* string) = 0;
    virtual void SetInt32(int index, void* value) = 0;
    virtual void SetInt64(int index, void* value) = 0;
    virtual void SetFloat64(int index, void* value) = 0;
    virtual void SetFloat32(int index, void* value) = 0;
    virtual void SetString(int index, const std::string& string) = 0;
    virtual void Close() = 0;

    virtual std::unique_ptr<ResultSet> Execute() = 0;
    virtual void ExecuteUpdate() = 0;
private:
};

class ODBCPreparedStatement : public PreparedStatement
{
public:
    ODBCPreparedStatement(const ODBCPreparedStatement&) = delete;
    void operator=(const ODBCPreparedStatement&) = delete;
public:
    ODBCPreparedStatement(SQLHDBC conn_handle);
    virtual ~ODBCPreparedStatement();
    virtual void PrepareStatement(const char_t* string) override;
    virtual void SetInt32(int index, void* value) override;
    virtual void SetInt64(int index, void* value) override;
    virtual void SetFloat32(int index, void* value) override;
    virtual void SetFloat64(int index, void* value) override;    
    virtual void SetString(int index, const std::string& string) override;
    virtual void Close() override;

    virtual std::unique_ptr<ResultSet> Execute() override;
    virtual void ExecuteUpdate() override;
private:
    SQLHSTMT statement_handle_;
    SQLLEN nts_length_;
};