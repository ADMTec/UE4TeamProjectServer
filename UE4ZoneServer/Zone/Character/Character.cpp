#include "Character.hpp"


void Character::Initialize()
{
    // DB���� stat, skill, inventory, quick_slot �ʱ�ȭ�ϱ�.
}

void Character::EquipItem(int32_t item_inventory_index)
{
    // �ش� �������� ���� ������ �������� ������ return
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
    // �κ��丮�� �� ������ ���� �� X
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
    // ������ ������ Provider �ҷ��� �� req stat �� �� �ɸ��� ������ �� ������ ����
    return true;
}
