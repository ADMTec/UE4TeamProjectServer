#include "GameConstants.hpp"

void GameConstants::Initialize()
{
}

float GameConstants::GetStaminaRecoveryFromLevel(int32_t level) const
{
    return level * 5.0f;
}

GameConstants::ItemType GameConstants::GetItemTypeFromItemId(int32_t itemid) const
{
    int type = itemid / 1000000;
    switch (type) {
        case 1:
            return ItemType::kETC;
        case 2:
            return ItemType::kConsume;
        case 3:
            return ItemType::kEquip;
    }
    return ItemType();
}

// Template 으로 만들면 좋곘네.
std::optional<GameConstants::EquipPosition> GameConstants::GetEquipPositionFromEquipPosInt(equip_position_t pos) const
{
    switch (pos)
    {
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kArmor):
            return EquipPosition::kArmor;
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kHand):
            return EquipPosition::kHand;
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kShoes):
            return EquipPosition::kShoes;
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kWeapon):
            return EquipPosition::kWeapon;
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kSubWeapon):
            return EquipPosition::kSubWeapon;
    }
    return std::nullopt;
}

std::optional<GameConstants::EquipPosition> GameConstants::GetEquipPositionFromItemId(int32_t itemid) const
{
    return EquipPosition::kArmor;
}