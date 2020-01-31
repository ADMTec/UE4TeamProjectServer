#include "ODBCConnectionPoolImpl.hpp"


ODBCConnectionPoolImpl::ODBCConnectionPoolImpl()
    : is_running_(false), pool_alloc_size_(0), pool_size_(0)
{
    driver_ = new ODBCDriver();
    static_class_ = this;
}

ODBCConnectionPoolImpl::~ODBCConnectionPoolImpl()
{
    delete driver_;
}

void ODBCConnectionPoolImpl::Initialize(uint32_t pool_alloc_size, const char_t* odbc, const char_t* id, const char_t* pw)
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

void ODBCConnectionPoolImpl::Stop()
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    is_running_ = false;
}

void ODBCConnectionPoolImpl::Exit()
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    connection_pool_.clear();
}

bool ODBCConnectionPoolImpl::IsRunning() const
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    return is_running_;
}

int ODBCConnectionPoolImpl::GetPoolSize() const
{
    return pool_size_;
}

std::shared_ptr<Connection> ODBCConnectionPoolImpl::GetConnection()
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

bool ODBCConnectionPoolImpl::ReturnConnection(const std::shared_ptr<Connection>& conn)
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    if (is_running_ == false) {
        return false;
    }
    connection_pool_.emplace_back(conn);
    return true;
}

namespace {
    void CustomDeleter(ODBCConnectionPoolImpl* instance, Connection* con) {
        if (false == instance->ReturnConnection(
            std::shared_ptr<Connection>(con, std::bind(CustomDeleter, instance, std::placeholders::_1)))) {
            delete con;
        }
    }
}

void ODBCConnectionPoolImpl::AddConnection(int size)
{
    std::lock_guard<std::recursive_mutex> lock(pool_guard_);
    for (int i = 0; i < size; ++i) {
        connection_pool_.emplace_back(driver_->Connect(
            odbc_,
            id_,
            pw_,
            std::bind(CustomDeleter, this, std::placeholders::_1)));
    }
    pool_size_ += size;
}
