#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include "Model/FVector.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>


struct ZoneData
{
    struct SpawnPoint {
        enum class Type : int32_t {
            kMonster = 1,
            kNpc = 2,
            kReactor = 3,
        };
        SpawnPoint(int32_t _type, int64_t _id, float x, float y, float z, float rx, float ry, float rz)
            : type(_type), id(_id), location(x, y, z), rotation(rx, ry, rz)
        {
        }
        int32_t type;
        int64_t id;
        Location location;
        Rotation rotation;
    };
    struct Portal {
        Portal(int64_t id, float x, float y, float z)
            : map_id(id), location(x, y, z)
        {
        }
        Portal() = default;
        int32_t map_id = -1;
        float range = 0.0f;
        Location location;
    };
    ZoneData() = default;
    int64_t map_id = -1;
    int32_t type = -1;
    Portal portal;
    Location player_spawn;
    std::vector<SpawnPoint> spawn_point;
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