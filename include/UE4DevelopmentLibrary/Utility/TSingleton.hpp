#pragma once

template<typename T>
class TSingleton
{
public:
    TSingleton(const TSingleton&) = delete;
    void operator=(const TSingleton&) = delete;

public:
    static T& Instance() {
        static T instance;
        return instance;
    }
protected:
    TSingleton() = default;
    ~TSingleton() = default;
};