#include "DatabaseDriver.hpp"
#include "Exception.hpp"
#include "Connection.hpp"
#include "ODBC.hpp"

DatabaseDriver::~DatabaseDriver()
{

}

ODBCDriver::ODBCDriver()
    : environment_handle_(NULL)
{
}

ODBCDriver::~ODBCDriver()
{
    if (environment_handle_ != NULL) {
        SQLFreeHandle(SQL_HANDLE_ENV, environment_handle_);
    }
}

void ODBCDriver::Initialize()
{
    SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &environment_handle_);
    if (result != SQL_SUCCESS) {
        throw StackTraceException(ExceptionType::kSQLError, "sql enviroment handle allocation fail");
    }
    result = SQLSetEnvAttr(environment_handle_, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3_80, SQL_IS_INTEGER);
    ODBC::CheckSQLError(result, environment_handle_, SQL_HANDLE_ENV);
}

std::shared_ptr<Connection> ODBCDriver::Connect(const string_t& addr, const string_t& id, const string_t& pw, const deleter_t& deleter)
{
    std::shared_ptr<Connection> con = nullptr;
    if (deleter == nullptr) {
        con = std::shared_ptr<Connection>(new ODBCConnection(environment_handle_));
    } else {
        con = std::shared_ptr<Connection>(new ODBCConnection(environment_handle_), deleter);
    }
    con->Connect(addr, id, pw);
    return con;
}
//
//std::shared_ptr<Connection> ODBCDriver::Connect(const string_t& addr, const string_t& id, const string_t& pw)
//{
//    auto con = std::shared_ptr<Connection>(new ODBCConnection(environment_handle_));
//    con->Connect(addr, id, pw);
//    return con;
//}