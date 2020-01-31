#pragma once
#include "../DllBase.hpp"
#include <cstdint>
#include <string>
#include <memory>
#include <sstream>

#include <WinSock2.h>
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>


#ifdef _UNICODE
using string_t = std::wstring;
using sqlchar_t = SQLWCHAR;
using char_t = wchar_t;
#else
using string_t = std::string;
using sqlchar_t = SQLCHAR;
using char_t = char;
#endif