#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Zone/Fvector.hpp"


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
    int64_t map_id;
    std::string type;
    std::vector<Location> player_spawn_location_;
    std::vector<SpawnPoint> npc_spawn_point;
};