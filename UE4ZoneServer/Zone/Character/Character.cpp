#include "Character.hpp"


void Character::Initialize()
{
    // DB에서 stat, skill, inventory, quick_slot 초기화하기.
}

void Character::EquipItem(int32_t item_inventory_index)
{
    // 해당 아이템의 착용 조건이 만족되지 않으면 return
    const std::shared_ptr<BaseItem>& item = inventory_.GetItem(item_inventory_index);
    if (item && CanEquipItem(*item)) 
    {
        auto opt_equip_pos = GameConstant.GetEquipPositionFromItemId(item->GetItemId());
        if (opt_equip_pos) 
        {
            auto off_item = inventory_.ExtractEquipItem(*opt_equip_pos);
            inventory_.SetEquipItem(*opt_equip_pos, item);
            if (off_item) {
                inventory_.SetItem(item_inventory_index, off_item);
            }
            inventory_.SendEquipAndInventorySlotUpdateResult(*opt_equip_pos, item_inventory_index);
        }
    }
}

void Character::UnequipItem(int32_t equip_position)
{
    // 인벤토리에 빈 공간이 없을 시 X
    if (inventory_.InventoryCapacity() == 0) {
        return;
    }
}

void Character::ConsumeItem(int32_t item_inventory_index)
{
}

void Character::ChangeInventoryItemPosition(int32_t item_inventory_index, int32_t new_index)
{
}

void Character::DestoryItem(int32_t item_inventory_index)
{
}

bool Character::CanEquipItem(BaseItem& item)
{
    int32_t itemid = item.GetItemId();
    auto type = GameConstant.GetItemTypeFromItemId(itemid);
    if (type != GameConstants::ItemType::kEquip) {
        return false;
    }
    // 아이템 데이터 Provider 불러온 후 req stat 비교 후 케릭터 스텟이 더 적으면 ㅂㅂ
    return true;
}
