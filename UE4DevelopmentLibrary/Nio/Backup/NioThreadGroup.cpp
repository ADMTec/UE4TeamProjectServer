#include "NioThreadGroup.hpp"


NioThreadGroup::NioThreadGroup()
    : num_thread_(0)
{
}

NioThreadGroup::~NioThreadGroup()
{
    for (uint64_t i = 0; i < num_thread_; ++i) {
        if (thread_array_[i].joinable()) {
            thread_array_[i].join();
        }
    }
}

void NioThreadGroup::Run(uint64_t num_thread, const task_t& task)
{
    num_thread_ = num_thread;
    for (uint64_t i = 0; i < num_thread_; ++i) {
        thread_array_.emplace_back(task);
    }
}