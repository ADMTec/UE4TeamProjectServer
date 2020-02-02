#include "Equipment.hpp"
#include "EnumClassHelper.hpp"


bool Equipment::HasEquipItem(Equipment::Position pos)
{
    return equipments_[ToInt32(pos)].operator bool();
}

void Equipment::Equip(Equipment::Position pos, const std::shared_ptr<EquipItem>& item)
{
    equipments_[ToInt32(pos)] = item;
}

std::shared_ptr<EquipItem> Equipment::UnEquip(Equipment::Position pos)
{
    auto item = equipments_[ToInt32(pos)];
    equipments_[ToInt32(pos)] = nullptr;
    return item;
}

int32_t Equipment::GetTotalAddStr() const
{
    int32_t str = 0;
    for (const auto& equip : equipments_) {
        if (equip) {
            str += equip->GetAddStr();
        }
    }
    return str;
}

int32_t Equipment::GetTotalAddDex() const
{
    int32_t str = 0;
    for (const auto& equip : equipments_) {
        if (equip) {
            str += equip->GetAddDex();
        }
    }
    return str;
}

int32_t Equipment::GetTotalAddIntel() const
{
    int32_t str = 0;
    for (const auto& equip : equipments_) {
        if (equip) {
            str += equip->GetAddInt();
        }
    } 
    return str;
}

const Equipment::Data& Equipment::GetData() const
{
    return equipments_;
}

int32_t Equipment::GetTotalATK() const
{
    int32_t str = 0;
    for (const auto& equip : equipments_) {
        if (equip) {
            str += equip->GetAddATK();
        }
    }
    return str;
}
