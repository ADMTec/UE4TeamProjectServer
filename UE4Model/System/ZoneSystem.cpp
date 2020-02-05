#include "ZoneSystem.hpp"
#include <atomic>
#include "Provider/ZoneDataProvider.hpp"
#include "Provider/MonsterTemplateProvider.hpp"


std::shared_mutex ZoneSystem::map_guard_;
std::vector<std::shared_ptr<Zone>> ZoneSystem::maps_;
std::thread ZoneSystem::zone_update_thread_;
std::atomic<bool> ZoneSystem::zone_update_thread_exit_flag_ = false;
std::shared_ptr<Zone> ZoneSystem::town_ = nullptr;


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
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    town_ = ZoneSystem::CreateNewInstance(100);
    town_->SetState(Zone::State::kActive);
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

    std::shared_ptr<Zone> zone = std::shared_ptr<Zone>(new Zone(), 
        [new_id](Zone* zone) {
            ZoneSystem::DestoryInstance(new_id);
            delete zone;
        });
    zone->SetMapId(data_ptr->map_id);
    zone->SetInstanceId(new_id);
    zone->SetType(static_cast<Zone::Type>(data_ptr->type));
    zone->SetPlayerSpawn(data_ptr->player_spawn);
    zone->SetNextMapPortalInfo(data_ptr->portal.map_id, data_ptr->portal.range, data_ptr->portal.location);
    for (const auto& spawn_point : data_ptr->spawn_point) {
        auto type = static_cast<ZoneData::SpawnPoint::Type>(spawn_point.type);
        if (type == ZoneData::SpawnPoint::Type::kMonster) {
            auto mob = MonsterTemplateProvider::Instance().GetMonster(spawn_point.id);
            if (mob) {
                zone->AddMonster(mob, spawn_point.location, spawn_point.rotation);
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
    if (zone) {
        zone->Exit();
    }
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

std::shared_ptr<Zone> ZoneSystem::GetTown()
{
    return town_;
}

std::string ZoneSystem::GetDebugString()
{
    std::vector<std::shared_ptr<Zone>> copy;
    {
        std::shared_lock lock(map_guard_);
        for (const auto& zone : maps_) {
            copy.emplace_back(zone);
        }
    }
    std::stringstream ss;
    ss << "-----------------------------------------------" << '\n';
    ss << "-----------------------------------------------" << '\n';
    ss << " Debug State [ZoneSystem]" << '\n';
    ss << town_->GetDebugString();
    if (!copy.empty()) {
        for (const auto& zone : copy) {
            ss << zone->GetDebugString();
        }
    }
    ss << "-----------------------------------------------" << '\n';
    ss << "-----------------------------------------------" << '\n';
    return ss.str();
}