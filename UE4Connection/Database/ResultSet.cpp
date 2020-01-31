#include "ResultSet.hpp"
#include "Private/ODBC.hpp"


ResultSet::~ResultSet()
{
}

ODBCResultSet::ODBCResultSet(SQLHSTMT statement_handle)
    : statement_handle_(statement_handle), closed_(true)
{
}

ODBCResultSet::~ODBCResultSet()
{
    if (closed_ == false) {
        Close(); // 소멸자에서 가상함수 호출로 문제가 되는 case가 아님
    }
}

bool ODBCResultSet::Next()
{
    SQLRETURN ret = SQLFetch(statement_handle_);
    closed_ = false;
    return ret != SQL_NO_DATA;
}

void ODBCResultSet::Close()
{
    SQLFreeStmt(statement_handle_, SQL_CLOSE);
    SQLFreeStmt(statement_handle_, SQL_UNBIND);
    SQLFreeStmt(statement_handle_, SQL_RESET_PARAMS);
    closed_ = true;
}

void ODBCResultSet::BindInt32(int index, void* return_value)
{
    SQLLEN value = 0;
    SQLRETURN result = SQLBindCol(statement_handle_, index, SQL_C_SLONG, return_value, 0, &value);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCResultSet::BindInt64(int index, void* return_value)
{
    SQLLEN value = 0;
    SQLRETURN result = SQLBindCol(statement_handle_, index, SQL_C_SBIGINT, return_value, 0, &value);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCResultSet::BindFloat32(int index, void* return_value)
{
    SQLLEN value = 0;
    SQLRETURN result = SQLBindCol(statement_handle_, index, SQL_C_FLOAT, return_value, 0, &value);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCResultSet::BindFloat64(int index, void* return_value)
{
    SQLLEN value = 0;
    SQLRETURN result = SQLBindCol(statement_handle_, index, SQL_C_DOUBLE, return_value, 0, &value);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCResultSet::BindString(int index, char* return_buffer, uint64_t size)
{
    SQLLEN value = 0;
    SQLRETURN result = SQLBindCol(statement_handle_, index, SQL_C_CHAR, return_buffer, size, &value);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}