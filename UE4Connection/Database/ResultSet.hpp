#pragma once
#include "base.hpp"


class ResultSet
{
public:
    virtual ~ResultSet() = 0;
    
    virtual bool Next() = 0;
    virtual void Close() = 0;   

    virtual void BindInt32(int index, void* return_value) = 0;
    virtual void BindInt64(int index, void* return_value) = 0;
    virtual void BindFloat32(int index, void* return_value) = 0;
    virtual void BindFloat64(int index, void* return_value) = 0;
    virtual void BindString(int index, char* return_buffer, uint64_t size) = 0;
};

class ODBCResultSet : public ResultSet
{
public:
    ODBCResultSet(SQLHSTMT statement_handle);
    ~ODBCResultSet();
    virtual bool Next() override;
    virtual void Close() override;

    virtual void BindInt32(int index, void* return_value) override;
    virtual void BindInt64(int index, void* return_value) override;
    virtual void BindFloat32(int index, void* return_value) override;
    virtual void BindFloat64(int index, void* return_value) override;
    virtual void BindString(int index, char* return_buffer, uint64_t size) override;
private:
    SQLHSTMT statement_handle_;
    bool closed_;
};