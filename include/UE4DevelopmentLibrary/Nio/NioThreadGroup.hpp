#pragma once
#include "NioBase.hpp"


class NioThreadGroup
{
public:
    NioThreadGroup(const NioThreadGroup&) = delete;
    void operator=(const NioThreadGroup&) = delete;
public:
    explicit NioThreadGroup();
    ~NioThreadGroup();

    void Run(uint64_t num_thread, const task_t& task);
private:
    uint64_t num_thread_;
    std::vector<std::thread> thread_array_;
};