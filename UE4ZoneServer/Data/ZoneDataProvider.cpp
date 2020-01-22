#include "ZoneDataProvider.hpp"
#include "nlohmann/json.hpp"
#include "Constants/ServerConstants.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>


using namespace nlohmann;
namespace fs = std::filesystem;

ZoneDataProvider::ZoneDataProvider()
{
}

void ZoneDataProvider::Initialize()
{
    auto map_path = fs::current_path();
    map_path /= ServerConstant.data_root_dir;
    map_path /= ServerConstant.map_data_root;

    for (const auto& entry : fs::directory_iterator(map_path))
    {
        auto path = entry.path();
        if (path.has_extension() && (path.extension() == ".json")) {
            std::ifstream file(path);
            if (file.is_open() == false) {
                std::stringstream ss;
                ss << path;
                throw StackTraceException(ExceptionType::kFileOpenFail, ss.str().c_str());
            }
            json map_data;
            file >> map_data;

            ZoneData data;
            try {
                data.map_id = map_data["Mapid"];
                data.type = map_data["Type"];
                for (const auto& sp : map_data["PlayerSpawnPoint"]) {
                    data.player_spawn_location_.emplace_back(sp["x"], sp["y"], sp["z"]);
                }
                for (const auto& sp : map_data["SpawnPoint"]) {
                    data.npc_spawn_point.emplace_back(
                        sp["type"], sp["id"], sp["x"], sp["y"], sp["z"]);
                }

            } catch (const std::exception & e) {
                std::stringstream ss;
                ss << path << ' - ' << e.what();
                throw StackTraceException(ExceptionType::kRunTimeError, ss.str().c_str());
            }
        }
    }
}

std::optional<const ZoneData* const> ZoneDataProvider::GetData(int64_t mapid) const
{
    auto iter = zone_data_.find(mapid);
    if (iter != zone_data_.end()) {
        return &iter->second;
    }
    return std::nullopt;
}
