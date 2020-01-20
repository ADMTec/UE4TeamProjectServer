#include "NIoLogger.hpp"
#include <iostream>
#include <sstream>

NioLogger::~NioLogger()
{
}

void NioLogger::Log(const char* msg)
{
    std::cout << msg << std::endl;
}

void NioLogger::Log(const char* func, const char* msg)
{
    std::stringstream ss;
    ss << "------------------------------------------------------------------" << '\n';
    ss << "LogMessage function [" << func << "]" << '\n';
    ss << msg << '\n';
    ss << "------------------------------------------------------------------" << '\n';
    std::cout << ss.str();
}

void NioLogger::Log(const char* func, int line, const char* msg)
{
    std::stringstream ss;
    ss << "------------------------------------------------------------------" << '\n';
    ss << "LogMessage function [" << func << ":" << line << "]" << '\n';
    ss << msg << '\n';
    ss << "------------------------------------------------------------------" << '\n';
    std::cout << ss.str();
}
