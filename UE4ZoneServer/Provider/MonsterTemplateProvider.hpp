#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include "UE4Model/Monster/Monster.hpp"
#include <unordered_map>
#include <optional>
#include <cstdint>


class MonsterTemplateProvider : public TSingleton<const MonsterTemplateProvider>
{
    friend class TSingleton<const MonsterTemplateProvider>;
    MonsterTemplateProvider();
public:
    void Initialize();
    std::optional<Monster> GetData(int32_t mobid) const;
private:
    std::unordered_map<int32_t, Monster> mob_data_;
};