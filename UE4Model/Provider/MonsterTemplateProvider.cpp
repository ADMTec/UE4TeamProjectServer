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
        MobData mob;
        mob.template_id = data.GetID();
        mob.attack_max = data.GetATKMax();
        mob.attack_min = data.GetATKMin();
        mob.attack_range = data.GetATKRange();
        mob.attack_speed = data.GetATKSpeed();
        mob.defence = data.GetDefence();
        mob.hp = data.GetMaxHP();
        mob_data_.emplace(mob.template_id, mob);
    }
}

std::shared_ptr<Monster> MonsterTemplateProvider::GetMonster(int32_t mobid) const
{
    auto iter = mob_data_.find(mobid);
    if (iter != mob_data_.end()) {
        const auto& data = iter->second;
        auto mob = std::make_shared<Monster>();
        std::lock_guard lock(mob->mutex_);
        mob->SetTemplateId(data.template_id);
        mob->SetAttackMax(data.attack_max);
        mob->SetAttackMin(data.attack_min);
        mob->SetAttackRange(data.attack_range);
        mob->SetAttackSpeed(data.attack_speed);
        mob->SetDefence(data.defence);
        mob->SetHP(data.hp);
        mob->SetMaxHP(data.hp);
        return mob;
    }
    return nullptr;
}