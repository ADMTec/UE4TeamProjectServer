#include "CharacterInventory.hpp"

CharacterInventory::CharacterInventory(Character& chr)
    : base_(chr)
{
}

void CharacterInventory::Initialize(int32_t db_cid)
{
}

std::optional<int32_t> CharacterInventory::GetEmptryInventorySpaceIndex() const
{
    for (uint32_t i = 0; i < inventory_size; ++i) {
        if (!inventory_[i]) {
            return i;
        }
    }
    return std::nullopt;
}

size_t CharacterInventory::InventoryCapacity() const
{
    uint32_t empty_space = 0;
    for (uint32_t i = 0; i < inventory_size; ++i) {
        if(!inventory_[i]) {
            empty_space++;
        }
    }
    return empty_space;
}

size_t CharacterInventory::InventorySize() const
{
    return equipment_size;
}

void CharacterInventory::SetItem(int32_t inventory_index, const shared_ptr<BaseItem>& item)
{
    if (inventory_index < 0 || inventory_index >= inventory_size) {
        return;
    }
    inventory_[inventory_index] = item;
}

void CharacterInventory::SetEquipItem(GameConstants::EquipPosition equip_pos, const shared_ptr<BaseItem>& equip)
{
    auto equip_pos_int = static_cast<GameConstants::equip_position_t>(equip_pos);
    if (equip_pos_int < 0 || equip_pos_int >= equipment_size) {
        return;
    }
    equipment_[equip_pos_int] = equip;
}

const shared_ptr<BaseItem>& CharacterInventory::GetItem(int32_t inventory_index)
{
    if (inventory_index < 0 || inventory_index >= inventory_size) {
        return nullptr;
    }
    return inventory_[inventory_index];
}

const shared_ptr<BaseItem>& CharacterInventory::GetEquipItem(GameConstants::EquipPosition equip_pos)
{
    auto equip_pos_int = static_cast<GameConstants::equip_position_t>(equip_pos);
    if (equip_pos_int < 0 || equip_pos_int >= equipment_size) {
        return nullptr;
    }
    return equipment_[equip_pos_int];
}

std::shared_ptr<BaseItem> CharacterInventory::ExtractItem(int32_t inventory_index)
{
    std::shared_ptr<BaseItem> item = GetItem(inventory_index);
    if (item) {
        inventory_[inventory_index] = nullptr;
    }
    return item;
}

std::shared_ptr<BaseItem> CharacterInventory::ExtractEquipItem(GameConstants::EquipPosition equip_pos)
{
    auto equip_pos_int = static_cast<GameConstants::equip_position_t>(equip_pos);
    std::shared_ptr<BaseItem> equip = GetEquipItem(equip_pos);
    if (equip) {
        equipment_[equip_pos_int] = nullptr;
    }
    return equip;
}

void CharacterInventory::Write(OutputStream& output) const
{
    for (int i = 0; i < equipment_size; ++i) {
        output << equipment_[i].operator bool();
        if (equipment_[i]) {
            output << *equipment_[i];
        }
    }
    for (int i = 0; i < inventory_size; ++i) {
        output << equipment_[i].operator bool();
        if (inventory_[i]) {
            output << *inventory_[i];
        }
    }
}

void CharacterInventory::Read(InputStream& input)
{
    int8_t has_data_;
    for (int i = 0; i < equipment_size; ++i) {
        input >> *reinterpret_cast<int8_t*>(&has_data_);
        if (has_data_) {
            shared_ptr<BaseItem> item = std::shared_ptr<BaseItem>(new EquipItem());
            input >> *item;
            equipment_[i] = item;
        }
    }
    for (int i = 0; i < inventory_size; ++i) {
        input >> *reinterpret_cast<int8_t*>(&has_data_);
        if (has_data_) {
            auto type = static_cast<GameConstants::ItemType>(input.PeekInt16());
            shared_ptr<BaseItem> item;
            switch (type) {
                case GameConstants::ItemType::kEquip:
                    item = std::shared_ptr<BaseItem>(new EquipItem());
                    break;
                case GameConstants::ItemType::kConsume:
                    item = std::shared_ptr<BaseItem>(new ConsumeItem());
                    break;
                case GameConstants::ItemType::kETC:
                    item = std::shared_ptr<BaseItem>(new BaseItem());
                    break;
            }
            input >> *item;
            inventory_[i] = item;
        }
    }
}