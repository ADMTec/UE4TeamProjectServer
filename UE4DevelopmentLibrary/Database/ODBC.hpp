#pragma once
#include "base.hpp"
#include <iostream>

class ODBC
{
public:
    static std::string GetExceptionMessage(SQLHANDLE handle, int handle_type);
    static void CheckSQLError(SQLRETURN result, SQLHANDLE handle, int handle_type);
};