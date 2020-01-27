#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <optional>
#include <array>

struct GameConstants : public TSingleton<const GameConstants>
{
private:
    friend class TSingleton<const GameConstants>;
    GameConstants() = default;
public:
    void Initialize();
public:
    using item_type_t = int8_t;
    enum class ItemType : item_type_t {
        kETC = 1,
        kConsume = 2,
        kEquip = 3,
    };
    using equip_position_t = int8_t;
    enum class EquipPosition : equip_position_t {
        kArmor = 0,
        kHand = 1,
        kShoes = 2,
        kWeapon = 3,
        kSubWeapon = 4,
        kCount = 5,
    };
public:
    float GetStaminaRecoveryFromLevel(int32_t level) const;
    ItemType GetItemTypeFromItemId(int32_t itemid) const;
    std::optional<EquipPosition> GetEquipPositionFromEquipPosInt(equip_position_t pos) const;
    std::optional<EquipPosition> GetEquipPositionFromItemId(int32_t itemid) const;
public:
    constexpr static const uint32_t inventory_size = 30;
    std::array<int32_t, 1> town_ids = { 100, };
    int32_t default_town = 100;

    float default_attack_min = 50.0f;
    float default_attack_max = 100.0f;
    float default_chr_speed = 100.0f;
public:

};

#define GameConstant GameConstants::Instance()