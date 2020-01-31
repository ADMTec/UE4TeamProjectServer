#pragma once
#include "DatabaseDriver.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <mutex>

UE4CONNECTION_DLLEXPORT class UE4CONNECTION_DLLCLASS std::recursive_mutex;
UE4CONNECTION_DLLEXPORT template class UE4CONNECTION_DLLCLASS std::shared_ptr<Connection>;


class UE4CONNECTION_DLLCLASS ODBCConnectionPool
{
    ODBCConnectionPool();
public:
    static ODBCConnectionPool& Instance() {
        static ODBCConnectionPool instance;
        return instance;
    }
public:
    void Initialize(uint32_t pool_alloc_size, const char_t* odbc, const char_t* id, const char_t* pw);
    void Stop();
    void Exit();
    bool IsRunning() const;
    int GetPoolSize() const;

    std::shared_ptr<Connection> GetConnection();
private:
    class ODBCConnectionPoolImpl* impl_;
};