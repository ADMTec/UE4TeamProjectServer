#include "PreparedStatement.hpp"
#include "Exception.hpp"
#include "ResultSet.hpp"
#include "ODBC.hpp"


ODBCPreparedStatement::ODBCPreparedStatement(SQLHDBC conn_handle)
    : statement_handle_(NULL), nts_length_(SQL_NTS)
{
    SQLRETURN result = SQLAllocHandle(SQL_HANDLE_STMT, conn_handle, &statement_handle_);
    if (result != SQL_SUCCESS) {
        ODBC::CheckSQLError(result, conn_handle, SQL_HANDLE_DBC);
    }
}

ODBCPreparedStatement::~ODBCPreparedStatement()
{
    if (statement_handle_ != NULL) {
        SQLFreeHandle(SQL_HANDLE_STMT, statement_handle_);
    }
}

void ODBCPreparedStatement::PrepareStatement(const char_t* string)
{
    SQLRETURN result =  SQLPrepareW(statement_handle_, (sqlchar_t*)string, SQL_NTS);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCPreparedStatement::SetInt32(int index, void* value)
{
    SQLRETURN result =  SQLBindParameter(
        statement_handle_,
        index,
        SQL_PARAM_INPUT,
        SQL_C_SLONG,
        SQL_INTEGER, 0, 0, (SQLPOINTER)value, 0, NULL);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCPreparedStatement::SetFloat32(int index, void* value)
{
    SQLRETURN result =  SQLBindParameter(
        statement_handle_,
        index,
        SQL_PARAM_INPUT,
        SQL_C_FLOAT,
        SQL_FLOAT, 0, 0, (SQLPOINTER)value, 0, NULL);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCPreparedStatement::SetFloat64(int index, void* value)
{
    SQLRETURN result =  SQLBindParameter(
        statement_handle_,
        index,
        SQL_PARAM_INPUT,
        SQL_C_DOUBLE,
        SQL_DOUBLE, 0, 0, (SQLPOINTER)value, 0, NULL);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCPreparedStatement::SetInt64(int index, void* value)
{
    SQLRETURN result =  SQLBindParameter(
        statement_handle_,
        index,
        SQL_PARAM_INPUT,
        SQL_C_SBIGINT,
        SQL_BIGINT, 0, 0, (SQLPOINTER)value, 0, NULL);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCPreparedStatement::SetString(int index, const std::string& string)
{
    SQLRETURN result =  SQLBindParameter(
        statement_handle_,
        index,
        SQL_PARAM_INPUT,
        SQL_C_CHAR,
        SQL_CHAR,
        string.length(),
        0,
        (SQLPOINTER)string.c_str(),
        0,
        &nts_length_);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}

void ODBCPreparedStatement::Close()
{
    SQLCloseCursor(statement_handle_);
}

std::unique_ptr<ResultSet> ODBCPreparedStatement::Execute()
{
    SQLRETURN result =  SQLExecute(statement_handle_);
    if (result != SQL_SUCCESS) {
        std::string err_msg = ODBC::GetExceptionMessage(statement_handle_, SQL_HANDLE_STMT);
        throw StackTraceException(ExceptionType::kSQLError, err_msg.c_str());
    }
    return std::unique_ptr<ResultSet>(new ODBCResultSet(statement_handle_));
}

void ODBCPreparedStatement::ExecuteUpdate()
{
    SQLRETURN result =  SQLExecute(statement_handle_);
    ODBC::CheckSQLError(result, statement_handle_, SQL_HANDLE_STMT);
}