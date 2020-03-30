#pragma once
#include <cstdint>
#include <memory>
#include <mutex>
#include <WinSock2.h>
#include <Windows.h>
#include <concurrent_unordered_map.h>

class Barrier
{
public:
    enum class WaitingPolicy : size_t {
        Default = NULL,
        BlockOnly = SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY,
        SpinOnly = SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY
    };
public:
    Barrier(const Barrier&) = delete;
    void operator=(const Barrier&) = delete;
    Barrier(Barrier&&) noexcept = delete;
    void operator=(Barrier&&) noexcept = delete;
public:
    Barrier(int32_t num_threads, bool can_resize, WaitingPolicy policy = WaitingPolicy::Default);
    ~Barrier();
    void Resize(int32_t num_threads);
    void Enter();
    void Release();
private:
    void DeleteBarrier();
    void InitializeBarrier();
private:
    int32_t num_threads_;
    int32_t delete_check_policy_;
    int32_t wait_policy_;
    std::mutex barrier_mutex_;
    bool can_resize_;
    bool resize_reserved_;
    std::unique_ptr<SYNCHRONIZATION_BARRIER> concrete_barrier_;
};