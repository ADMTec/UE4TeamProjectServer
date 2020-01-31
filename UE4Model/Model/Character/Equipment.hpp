#pragma once
#include "EnumClassHelper.hpp"
#include "../Item/EquipItem.hpp"
#include <shared_mutex>
#include <array>


class Equipment
{
public:
    enum class Position {
        kArmor = 0,
        kHand = 1,
        kShoes = 2,
        kWeapon = 3,
        kCount = 4,
    };
public:
    bool HasEquipItem(Equipment::Position pos);
    void Equip(Equipment::Position pos, const std::shared_ptr<EquipItem>& item);
    std::shared_ptr<EquipItem> UnEquip(Equipment::Position pos);
    int32_t GetTotalATK() const;
    int32_t GetTotalAddStr() const;
    int32_t GetTotalAddDex() const;
    int32_t GetTotalAddIntel() const;
    const std::array<std::shared_ptr<EquipItem>, ToInt32(Position::kCount)>& GetData() const;
private:
    std::array<std::shared_ptr<EquipItem>, ToInt32(Position::kCount)> equipments_;
};