#include "ODBC.hpp"
#include "Exception.hpp"
#include <codecvt>
#include <vector>
#include <locale>

std::string wcs_to_mbs(std::wstring const& str, std::locale const& loc)
{
    typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
    std::codecvt<wchar_t, char, std::mbstate_t> const& codecvt = std::use_facet<codecvt_t>(loc);
    std::mbstate_t state = std::mbstate_t();
    std::vector<char> buff((str.size() + 1) * codecvt.max_length());
    wchar_t const* in_next = str.c_str();
    char* out_next = &buff[0];
    codecvt_t::result r = codecvt.out(state, str.c_str(), str.c_str() + str.size(), in_next, &buff[0], &buff[0] + buff.size(), out_next);
    return std::string(&buff[0]);
}


std::string ODBC::GetExceptionMessage(SQLHANDLE handle, int handle_type)
{
    SQLINTEGER NativeError;
    sqlchar_t SqlState[6], Msg[255];
    SQLSMALLINT MsgLen;;
    int num_diag = 0;
    int i = 1;
#ifdef _UNICODE
    std::wstringstream ss; 
#else
    std::stringstream ss;
#endif
    SQLGetDiagField(handle_type, handle, 0, SQL_DIAG_NUMBER, &num_diag, 0, &MsgLen);
    ss << "NumDiag[" << num_diag << "], ";
    while(true)
    {
        SQLRETURN Ret = SQLGetDiagRec(handle_type, handle, i++,
            SqlState,
            &NativeError, 
            Msg, sizeof(Msg), &MsgLen);
        if (Ret == SQL_NO_DATA) {
            break;
        }
        ss << "SQLSTATE[" << SqlState << "], NativeError[" << NativeError << "] " << Msg;
    }
#ifdef _UNICODE
    return wcs_to_mbs(ss.str(), std::locale("kor"));
#else
    return ss.str();
#endif
}

void ODBC::CheckSQLError(SQLRETURN result, SQLHANDLE handle, int handle_type)
{
    if ((result != SQL_SUCCESS) && (result != SQL_SUCCESS_WITH_INFO)) {
        std::string err_msg = ODBC::GetExceptionMessage(handle, handle_type);
        throw StackTraceException(ExceptionType::kSQLError, err_msg.c_str());
    }
}