#include "Connection.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "PreparedStatement.hpp"
#include "Private/ODBC.hpp"

Connection::~Connection()
{
}

ODBCConnection::ODBCConnection(SQLHENV enviroment_handle)
{
    SQLRETURN result = SQLAllocHandle(SQL_HANDLE_DBC, enviroment_handle, &connect_handle_);
    ODBC::CheckSQLError(result, enviroment_handle, SQL_HANDLE_ENV);
}

ODBCConnection::~ODBCConnection()
{
    if (connect_handle_) {
        SQLDisconnect(connect_handle_);
        SQLFreeHandle(SQL_HANDLE_DBC, connect_handle_);
    }
}

void ODBCConnection::Connect(const string_t& addr, const string_t& id, const string_t& pw)
{
    SQLRETURN Ret = SQLConnect(
        connect_handle_,
        (sqlchar_t*)addr.c_str(), SQL_NTS,
        (sqlchar_t*)id.c_str(), SQL_NTS,
        (sqlchar_t*)pw.c_str(), SQL_NTS);
    if ((Ret != SQL_SUCCESS) && (Ret != SQL_SUCCESS_WITH_INFO)) {
        std::string error_msg = ODBC::GetExceptionMessage(connect_handle_, SQL_HANDLE_DBC);
        throw StackTraceException(ExceptionType::kSQLError, error_msg.c_str());
    }
}

void ODBCConnection::SetSchema()
{
}

std::shared_ptr<PreparedStatement> ODBCConnection::GetPreparedStatement()
{    return std::shared_ptr<PreparedStatement>(
        new ODBCPreparedStatement(connect_handle_),
        [](PreparedStatement* ptr) {
            delete ptr;
        });
}
