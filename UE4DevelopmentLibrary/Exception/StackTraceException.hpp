#pragma once
#include <stdexcept>
#include <string>
#include <cstdint>

enum class ExceptionType;

class StackTraceException : public std::exception
{
public:
    const char* TypeToString(ExceptionType type) const;
public:
    StackTraceException(const StackTraceException& rhs);
    void operator=(const StackTraceException& rhs);

    StackTraceException(const char* except_type, const char* message);
    StackTraceException(ExceptionType except_type, const char* message);
    ~StackTraceException();
    virtual char const* what() const override;
private:
    std::string message_;
};