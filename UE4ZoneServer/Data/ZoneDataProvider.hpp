#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include "Game/FVector.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>


struct ZoneData
{
    struct SpawnPoint {
        SpawnPoint(const std::string& _type, int64_t _id, float x, float y, float z)
            : type(_type), id(_id), location(x, y, z)
        {
        }
        std::string type;
        int64_t id;
        Location location;
    };
    struct Portal {

    };
    int64_t map_id;
    std::string type;
    std::vector<Location> player_spawn_location_;
    std::vector<SpawnPoint> npc_spawn_point;
};


class ZoneDataProvider : public TSingleton<const ZoneDataProvider>
{
    friend class TSingleton<const ZoneDataProvider>;
    ZoneDataProvider();
public:
    void Initialize();

    std::optional<const ZoneData* const> GetData(int64_t mapid) const;
private:
    std::unordered_map<int64_t, ZoneData> zone_data_;
};