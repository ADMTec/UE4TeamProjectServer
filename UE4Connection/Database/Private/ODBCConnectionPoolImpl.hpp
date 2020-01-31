#pragma once
#include "../ODBCConnectionPool.hpp"

class ODBCConnectionPoolImpl
{
public:
    ODBCConnectionPoolImpl();
    ~ODBCConnectionPoolImpl();
    void Initialize(uint32_t pool_alloc_size, const char_t* odbc, const char_t* id, const char_t* pw);
    void Stop();
    void Exit();
    bool IsRunning() const;
    int GetPoolSize() const;

    std::shared_ptr<Connection> GetConnection();
    bool ReturnConnection(const std::shared_ptr<Connection>& conn);
    static ODBCConnectionPoolImpl* static_class_;
private:
    void AddConnection(int size);
private:
    bool is_running_;
    int pool_alloc_size_;
    int pool_size_;
    string_t odbc_;
    string_t id_;
    string_t pw_;
    DatabaseDriver* driver_;

    mutable std::recursive_mutex pool_guard_;
    std::vector<std::shared_ptr<Connection>> connection_pool_;
};