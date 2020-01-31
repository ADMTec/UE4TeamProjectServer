#include "ZoneSystem.hpp"
#include <atomic>
#include "Provider/ZoneDataProvider.hpp"
#include "Provider/MonsterTemplateProvider.hpp"


std::shared_mutex ZoneSystem::map_guard_;
std::vector<std::shared_ptr<Zone>> ZoneSystem::maps_;
std::thread ZoneSystem::zone_update_thread_;
std::atomic<bool> ZoneSystem::zone_update_thread_exit_flag_ = false;

void ZoneSystem::Initialize()
{
    zone_update_thread_ = std::thread(
        []() {
            while (zone_update_thread_exit_flag_.load() == false)
            {
                std::shared_lock lock(map_guard_);
                size_t size = maps_.size();
                for (size_t i = 0; i < size; ++i) {
                    if (maps_[i]->GetState() == Zone::State::kActive) {
                        maps_[i]->Update();
                    }
                }
            }
        });
}

void ZoneSystem::Release()
{
    zone_update_thread_exit_flag_.store(true);
    zone_update_thread_.join();
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
    zone->StartUp();
    {
        std::unique_lock lock(map_guard_);
        maps_.emplace_back(zone);
    }
    return zone;
}

void ZoneSystem::DestoryInstance(int64_t instance_id)
{
    std::shared_ptr<Zone> zone = nullptr;
    {
        std::unique_lock lock(map_guard_);
        auto iter = std::find_if(maps_.begin(), maps_.end(),
            [instance_id](const std::shared_ptr<Zone>& zone) {
                return zone->GetInstanceId() == instance_id;
            });
        if (iter != maps_.end()) {
            zone = *iter;
            maps_.erase(iter);
        }
    }
    zone->Exit();
}

std::shared_ptr<Zone> ZoneSystem::GetInstance(int64_t instance_id)
{
    std::shared_lock lock(map_guard_);
    auto iter = std::find_if(maps_.begin(), maps_.end(),
        [instance_id](const std::shared_ptr<Zone>& zone) {
            return zone->GetInstanceId() == instance_id;
        });
    if (iter != maps_.end()) {
        return *iter;
    }
    return nullptr;
}