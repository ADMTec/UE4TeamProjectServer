#pragma once
#include "../ZoneObject.hpp"
#include "Constants/GameConstants.hpp"
#include "CharacterInventory.hpp"
#include "CharacterQuickSlot.hpp"
#include "CharacterSkill.hpp"
#include "CharacterStat.hpp"


class Character : ZoneObject
{
public:
    Character(const Character&) = delete;
    void operator=(const Character&) = delete;
public:
    void Initialize();

    void EquipItem(int32_t item_inventory_index);
    void UnequipItem(int32_t equip_position);
    void ConsumeItem(int32_t item_inventory_index);
    // 합치기가 될 경우 합쳐야함.
    void ChangeInventoryItemPosition(int32_t item_inventory_index, int32_t new_index);
    void DestoryItem(int32_t item_inventory_index);
private:
    bool CanEquipItem(BaseItem& item);
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    CharacterStat stat_;
    CharacterSkill skill_;
    CharacterInventory inventory_;
    CharacterQuickSlot quick_slot_;
};