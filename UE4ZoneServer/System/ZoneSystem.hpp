#pragma once
#include "UE4Model/Zone.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>
#include <memory>


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
    static std::unordered_map<int64_t, std::shared_ptr<Zone>> map_;
};