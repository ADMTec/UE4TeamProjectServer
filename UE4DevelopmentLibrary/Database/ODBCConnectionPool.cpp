#include "ODBCConnectionPool.hpp"

ODBCConnectionPool::ODBCConnectionPool()
    : is_running_(false), pool_alloc_size_(0), pool_size_(0)
{
    driver_ = std::make_unique<ODBCDriver>();
}

void ODBCConnectionPool::Initialize(uint32_t pool_alloc_size, const string_t& odbc, const string_t& id, const string_t& pw)
{
    pool_alloc_size_ = pool_alloc_size;
    odbc_ = odbc;
    id_ = id;
    pw_ = pw;
    driver_->Initialize();
    AddConnection(pool_alloc_size_);
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    is_running_ = true;
}

void ODBCConnectionPool::Stop()
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    is_running_ = false;
}

void ODBCConnectionPool::Exit()
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    connection_pool_.clear();
}

bool ODBCConnectionPool::IsRunning() const
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    return is_running_;
}

int ODBCConnectionPool::GetPoolSize() const
{
    return pool_size_;
}

std::shared_ptr<Connection> ODBCConnectionPool::GetConnection()
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    if (is_running_ == false) {
        return nullptr;
    }
    if (connection_pool_.empty()) {
        AddConnection(pool_alloc_size_);
    }
    auto back = connection_pool_.back();
    connection_pool_.pop_back();
    return back;
}

bool ODBCConnectionPool::ReturnConnection(const std::shared_ptr<Connection>& conn)
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    if (is_running_ == false) {
        return false;
    }
    connection_pool_.emplace_back(conn);
    return true;
}

namespace {
    void CustomDeleter(Connection* con) {
        if (false == ODBCConnectionPool::Instance().ReturnConnection(std::shared_ptr<Connection>(con, CustomDeleter))) {
            delete con;
        }
    }
}


void ODBCConnectionPool::AddConnection(int size)
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    for (int i = 0; i < size; ++i) {
        connection_pool_.emplace_back(driver_->Connect(
            odbc_,
            id_,
            pw_,
            CustomDeleter));
    }
    pool_size_ += size;
}