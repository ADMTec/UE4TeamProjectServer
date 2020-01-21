#include "GameConstants.hpp"

void GameConstants::Initialize()
{
}

GameConstants::ItemType GameConstants::GetItemTypeFromItemId(int32_t itemid) const
{
    return ItemType();
}

// Template 으로 만들면 좋곘네.
std::optional<GameConstants::EquipPosition> GameConstants::GetEquipPositionFromEquipPosInt(equip_position_t pos) const
{
    switch (pos)
    {
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kHat):
            return EquipPosition::kHat;
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kJacket):
            return EquipPosition::kJacket;
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kPants):
            return EquipPosition::kPants;
        case static_cast<equip_position_t>(GameConstants::EquipPosition::kWeapon):
            return EquipPosition::kWeapon;
    }
    return std::nullopt;
}

std::optional<GameConstants::EquipPosition> GameConstants::GetEquipPositionFromItemId(int32_t itemid) const
{
    return EquipPosition::kHat;
}