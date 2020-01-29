#include "ZoneSystem.hpp"
#include <atomic>
#include "Provider/ZoneDataProvider.hpp"
#include "Provider/MonsterTemplateProvider.hpp"


std::shared_mutex ZoneSystem::map_guard_;
std::unordered_map<int64_t, std::shared_ptr<Zone>> ZoneSystem::map_;


void ZoneSystem::Initialize()
{
}

void ZoneSystem::Release()
{
}

std::shared_ptr<Zone> ZoneSystem::CreateNewInstance(int32_t mapid)
{
    static std::atomic<int64_t> new_instance_id = 0;
    int64_t new_id = new_instance_id.fetch_add(1);
    auto data = ZoneDataProvider::Instance().GetData(mapid);
    if (!data) {
        return nullptr;
    }
    const auto& data_ptr = *data;

    std::shared_ptr<Zone> zone = std::make_shared<Zone>();
    zone->SetInstanceId(new_id);
    zone->SetType(static_cast<Zone::Type>(data_ptr->type));
    zone->SetPlayerSpawn(data_ptr->player_spawn);
    for (const auto& spawn_point : data_ptr->spawn_point) {
        auto type = static_cast<ZoneData::SpawnPoint::Type>(spawn_point.type);
        if (type == ZoneData::SpawnPoint::Type::kMonster) {
            auto opt_mob = MonsterTemplateProvider::Instance().GetData(spawn_point.id);
            if (opt_mob) {
                zone->AddMonster(*opt_mob, spawn_point.location, spawn_point.rotation);
            }
        } else if (type == ZoneData::SpawnPoint::Type::kNpc) {
            // ...
        } else if (type == ZoneData::SpawnPoint::Type::kReactor) {
            // ...
        }
    }
    {
        std::unique_lock lock(map_guard_);
        map_.emplace(zone->GetInstanceId(), zone);
    }
    return zone;
}

void ZoneSystem::DestoryInstance(int64_t instance_id)
{
    std::unique_lock lock(map_guard_);
    map_.erase(instance_id);
}

std::shared_ptr<Zone> ZoneSystem::GetInstance(int64_t instance_id)
{
    std::shared_lock lock(map_guard_);
    auto iter = map_.find(instance_id);
    if (iter != map_.end()) {
        return iter->second;
    }
    return nullptr;
}