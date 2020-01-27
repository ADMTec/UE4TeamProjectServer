#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
// Monster Header Ãß°¡
#include <unordered_map>
#include <optional>
#include <cstdint>

//
//struct MobData
//{
//    int32_t mob_id;
//    float hp_ = 0.0f;
//    float max_hp_ = 0.0f;
//    float attack_min_ = 0.0f;
//    float attack_max_ = 0.0f;
//    float attack_range_ = 0.0f;
//    float attack_speed_ = 0.0f;
//    float defence_ = 0.0f;
//    float speed_ = 0.0f;
//};


class MonsterTemplateProvider : public TSingleton<const MonsterTemplateProvider>
{
    friend class TSingleton<const MonsterTemplateProvider>;
    MonsterTemplateProvider();
public:
    void Initialize();
    std::optional<Monster> GetData(int64_t mobid) const;
private:
    std::unordered_map<int64_t, Monster> mob_data_;
};