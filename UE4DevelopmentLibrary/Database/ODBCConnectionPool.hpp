#pragma once
#include "DatabaseDriver.hpp"
#include "../Utility.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <mutex>



class ODBCConnectionPool : public TSingleton<ODBCConnectionPool>
{
    friend class TSingleton<ODBCConnectionPool>;
    ODBCConnectionPool();
public:
    void Initialize(uint32_t pool_alloc_size, const string_t& odbc, const string_t& id, const string_t& pw);
    void Stop();
    void Exit();
    bool IsRunning() const;
    int GetPoolSize() const;

    std::shared_ptr<Connection> GetConnection();
    bool ReturnConnection(const std::shared_ptr<Connection>& conn);
private:
    void AddConnection(int size);
private:
    bool is_running_;
    int pool_alloc_size_;
    int pool_size_;
    string_t odbc_;
    string_t id_;
    string_t pw_;
    std::unique_ptr<DatabaseDriver> driver_;

    mutable std::recursive_mutex pool_guard_;
    std::vector<std::shared_ptr<Connection>> connection_pool_;
};