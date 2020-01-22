#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <optional>


struct GameConstants : public TSingleton<const GameConstants>
{
private:
    friend class TSingleton<const GameConstants>;
    GameConstants() = default;
public:
    void Initialize();
public:
    using item_type_t = int16_t;
    enum class ItemType : item_type_t {
        kETC = 1,
        kConsume = 2,
        kEquip = 3,
    };
    using equip_position_t = int16_t;
    enum class EquipPosition : equip_position_t {
        kHat = 1,
        kJacket = 2,
        kShoes = 3,
        kPants = 4,
        kWeapon = 5,
        kSubWeapon = 6,
        kCount,
    };
public:
    float GetStaminaRecoveryFromLevel(int32_t level) const;
    ItemType GetItemTypeFromItemId(int32_t itemid) const;
    std::optional<EquipPosition> GetEquipPositionFromEquipPosInt(equip_position_t pos) const;
    std::optional<EquipPosition> GetEquipPositionFromItemId(int32_t itemid) const;
public:
    constexpr static const uint32_t inventory_size = 30;
public:

};

#define GameConstant GameConstants::Instance()