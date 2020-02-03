#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include "Model/Monster/Monster.hpp"
#include <unordered_map>
#include <memory>
#include <cstdint>


class MonsterTemplateProvider : public TSingleton<const MonsterTemplateProvider>
{
    friend class TSingleton<const MonsterTemplateProvider>;
    MonsterTemplateProvider();
    struct MobData
    {
        int32_t template_id;
        float attack_max;
        float attack_min;
        float attack_range;
        float attack_speed;
        float defence;
        float hp;
    };
public:
    void Initialize();
    std::shared_ptr<Monster> GetMonster(int32_t mobid) const;
private:
    std::unordered_map<int32_t, MobData> mob_data_;
};