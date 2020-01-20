#pragma once
#include "Exception/StackTraceException.hpp"

enum class ExceptionType : int32_t
{
    kNullPointer,
    kOutOfRange,
    kLogicError,
    kConcurrency,
    kFileOpenFail,
    kInvalidArgument,
    kOverflow,
    kUnderflow,
    kBadAlloc,
    kSQLError,
    kRunTimeError,
};

