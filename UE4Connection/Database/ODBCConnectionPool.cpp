#include "ODBCConnectionPool.hpp"
#include "Private/ODBCConnectionPoolImpl.hpp"


ODBCConnectionPool::ODBCConnectionPool()
{
    impl_ = new ODBCConnectionPoolImpl();
}

void ODBCConnectionPool::Initialize(uint32_t pool_alloc_size, const char_t* odbc, const char_t* id, const char_t* pw)
{
    impl_->Initialize(pool_alloc_size, odbc, id, pw);
}

void ODBCConnectionPool::Stop()
{
    impl_->Stop();
}

void ODBCConnectionPool::Exit()
{
    impl_->Exit();
}

bool ODBCConnectionPool::IsRunning() const
{
    return impl_->IsRunning();
}

int ODBCConnectionPool::GetPoolSize() const
{
    return impl_->GetPoolSize();
}

std::shared_ptr<Connection> ODBCConnectionPool::GetConnection()
{
    return impl_->GetConnection();
}