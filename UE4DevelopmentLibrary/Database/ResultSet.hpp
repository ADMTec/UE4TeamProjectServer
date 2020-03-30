#pragma once
#include "base.hpp"
#include <cstdint>

class ResultSet
{
public:
    virtual ~ResultSet() = 0;
    
    virtual bool Next() = 0;
    virtual void Close() = 0;   

    virtual void BindInt32(int index, int32_t* return_value) = 0;
    virtual void BindUInt32(int index, uint32_t* return_value) = 0;
    virtual void BindInt64(int index, int64_t* return_value) = 0;
    virtual void BindUInt64(int index, uint64_t* return_value) = 0;
    virtual void BindFloat32(int index, float* return_value) = 0;
    virtual void BindFloat64(int index, double* return_value) = 0;
    virtual void BindString(int index, char* return_buffer, uint64_t size) = 0;
};

class ODBCResultSet : public ResultSet
{
public:
    ODBCResultSet(SQLHSTMT statement_handle);
    ~ODBCResultSet();
    virtual bool Next() override;
    virtual void Close() override;

    virtual void BindInt32(int index, int32_t* return_value) override;
    virtual void BindUInt32(int index, uint32_t* return_value) override;
    virtual void BindInt64(int index, int64_t* return_value) override;
    virtual void BindUInt64(int index, uint64_t* return_value) override;
    virtual void BindFloat32(int index, float* return_value) override;
    virtual void BindFloat64(int index, double* return_value) override;
    virtual void BindString(int index, char* return_buffer, uint64_t size) override;
private:
    SQLHSTMT statement_handle_;
    bool closed_;
    SQLLEN length_;
};