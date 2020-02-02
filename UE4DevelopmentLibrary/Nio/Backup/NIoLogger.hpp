#pragma once

class NioLogger
{
public:
    virtual ~NioLogger();

    virtual void Log(const char* msg);
    virtual void Log(const char* func, const char* msg);
    virtual void Log(const char* func, int line, const char* msg);
};