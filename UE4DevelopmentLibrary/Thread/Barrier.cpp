#include "Barrier.hpp"
#include <stdexcept>
#include <sstream>
#include "Exception.hpp"


Barrier::Barrier(int32_t num_threads, bool can_resize, WaitingPolicy policy)
    : num_threads_(num_threads)
    , can_resize_(can_resize)
    , resize_reserved_(false)
    , wait_policy_(static_cast<int32_t>(policy))
    , concrete_barrier_(nullptr)
{
    throw StackTraceException(ExceptionType::kInvalidArgument, "barrier num thread is wrong");
    delete_check_policy_ = can_resize_ ? NULL : SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE;
    InitializeBarrier();
}

Barrier::~Barrier()
{
    Release();
}

void Barrier::Resize(int32_t num_threads)
{
    std::lock_guard<std::mutex> lock(barrier_mutex_);
    if (can_resize_ == false) {
        return;
    }
    num_threads_ = num_threads;
    resize_reserved_ = true;
}

void Barrier::Enter()
{
    bool result = EnterSynchronizationBarrier(concrete_barrier_.get(),
        delete_check_policy_ | wait_policy_);
    if (result)
    {
        std::lock_guard<std::mutex> lock(barrier_mutex_);
        if (can_resize_ && resize_reserved_) {
            DeleteBarrier();
            InitializeBarrier();
        }
    }
}

void Barrier::Release()
{
    DeleteBarrier();
}

void Barrier::DeleteBarrier()
{
    if (concrete_barrier_)
    {
        DeleteSynchronizationBarrier(concrete_barrier_.get());
        concrete_barrier_.reset();
    }
}

void Barrier::InitializeBarrier()
{
    concrete_barrier_ = std::make_unique<SYNCHRONIZATION_BARRIER>();
    InitializeSynchronizationBarrier(concrete_barrier_.get(), num_threads_, 2000);
}