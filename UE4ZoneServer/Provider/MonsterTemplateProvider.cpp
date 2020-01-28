#include "MonsterTemplateProvider.hpp"
#include "ServerConstants.hpp"
#include "Provider/CSVDataProvider.hpp"
#include <filesystem>


namespace fs = std::filesystem;

MonsterTemplateProvider::MonsterTemplateProvider()
{

}

void MonsterTemplateProvider::Initialize()
{
    auto map_path = fs::current_path();
    map_path /= ServerConstant.data_root_dir;
    auto table_data = CSVDataProvider<MonsterTableData>::Instance().GetAll();
    for (const auto& data : table_data) {
        Monster mob;
        mob.SetTemplateId(data.GetID());
        mob.SetAttackMax(data.GetATKMax());
        mob.SetAttackMin(data.GetATKMin());
        mob.SetAttackRange(data.GetATKRange());
        mob.SetAttackSpeed(data.GetATKSpeed());
        mob.SetDefence(data.GetDefence());
        mob.SetHP(data.GetMaxHP());
        mob.SetMaxHP(data.GetMaxHP());
        mob_data_.emplace(mob.GetTemplateId(), mob);
    }
}

std::optional<Monster> MonsterTemplateProvider::GetData(int32_t mobid) const
{
    auto iter = mob_data_.find(mobid);
    if (iter != mob_data_.end()) {
        return iter->second;
    }
    return std::nullopt;
}