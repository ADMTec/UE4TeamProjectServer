#include "MonsterTemplateProvider.hpp"
#include "ServerConstants.hpp"
#include <filesystem>


namespace fs = std::filesystem;

void MonsterTemplateProvider::Initialize()
{
    auto map_path = fs::current_path();
    map_path /= ServerConstant.data_root_dir;
}

std::optional<Monster> MonsterTemplateProvider::GetData(int64_t mobid) const
{
    auto iter = mob_data_.find(mobid);
    if (iter != mob_data_.end()) {
        return iter->second;
    }
    return std::nullopt;
}