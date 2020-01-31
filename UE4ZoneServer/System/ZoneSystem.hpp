#pragma once
#include "UE4Model/Zone.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>
#include <memory>
#include <thread>
#include <atomic>


class ZoneSystem
{
public:
    static void Initialize();
    static void Release();
    static std::shared_ptr<Zone> CreateNewInstance(int32_t mapid);
    static void DestoryInstance(int64_t instance_id);
    static std::shared_ptr<Zone> GetInstance(int64_t instance_id);
private:
    static std::shared_mutex map_guard_;
    static std::vector<std::shared_ptr<Zone>> maps_;
    static std::thread zone_update_thread_;
    static std::atomic<bool> zone_update_thread_exit_flag_;
};