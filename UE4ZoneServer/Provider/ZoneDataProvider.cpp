#include "ZoneDataProvider.hpp"
#include "nlohmann/json.hpp"
#include "ServerConstants.hpp"
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
                if (map_data["Portal"].is_null() == false) {
                    data.portal.map_id = map_data["Portal"]["Mapid"];
                    data.portal.location.x = map_data["Portal"]["x"];
                    data.portal.location.y = map_data["Portal"]["y"];
                    data.portal.location.z = map_data["Portal"]["z"];
                }
                for (const auto& sp : map_data["SpawnPoint"]) {
                        data.spawn_point.emplace_back(
                            sp["type"], sp["id"], sp["x"], sp["y"], sp["z"], sp["rx"], sp["ry"], sp["rz"]);
                }
                data.player_spawn.x   = map_data["PlayerSpawn"]["x"];
                data.player_spawn.y = map_data["PlayerSpawn"]["y"];
                data.player_spawn.z = map_data["PlayerSpawn"]["z"];
                zone_data_.emplace(data.map_id, data);
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
