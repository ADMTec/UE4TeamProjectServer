#include "CharacterInventory.hpp"
#include "Data/Database.hpp"
#include "Data/DatabaseTable.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "Constants/GameConstants.hpp"
#include <sstream>


CharacterInventory::CharacterInventory(Character& chr)
    : base_(chr)
{
}

void CharacterInventory::Initialize(int32_t db_cid, class Connection& con)
{
    auto ps = con.GetPreparedStatement();
    ps->PrepareStatement(InventoryItemTable::GetSelectQueryFromCid());
    ps->SetInt32(1, &db_cid);

    auto rs = ps->Execute();
   

    InventoryItemTable table;
    InventoryItemTable::BindResultSet(*rs, &table);
    while (rs->Next()) {
        auto item_type = GameConstant.GetItemTypeFromItemId(table.itemid);
        CheckInventoryTableData(table);
        std::shared_ptr<BaseItem> item;
        if (item_type == GameConstants::ItemType::kETC) {
            item = std::make_shared<BaseItem>(table.itemid, GameConstants::ItemType::kETC);
        } else if (item_type == GameConstants::ItemType::kConsume) {
            std::shared_ptr<BaseItem> item = std::make_shared<ConsumeItem>(table.itemid);
            inventory_[table.slot].first = table.quantity;
            inventory_[table.slot].second = item;
        } else if (item_type == GameConstants::ItemType::kEquip) {
            std::shared_ptr<BaseItem> item = std::make_shared<EquipItem>(table.itemid);
            if (table.equipped) {
                auto opt_pos = GameConstant.GetEquipPositionFromItemId(table.itemid);
                if (opt_pos) {
                    equipment_[static_cast<int32_t>(*opt_pos)] = item;
                    continue;
                }
            }
        }
        inventory_[table.slot].first = table.quantity;
        inventory_[table.slot].second = item;
    }
}

std::optional<int32_t> CharacterInventory::GetEmptryInventorySpaceIndex() const
{
    for (uint32_t i = 0; i < inventory_size; ++i) {
        if (!inventory_[i].second) {
            return i;
        }
    }
    return std::nullopt;
}

size_t CharacterInventory::InventoryCapacity() const
{
    uint32_t empty_space = 0;
    for (uint32_t i = 0; i < inventory_size; ++i) {
        if(!inventory_[i].second) {
            empty_space++;
        }
    }
    return empty_space;
}

size_t CharacterInventory::InventorySize() const
{
    return equipment_size;
}

void CharacterInventory::SetItem(int32_t inventory_index, const shared_ptr<BaseItem>& item, uint32_t quantity)
{
    if (inventory_index < 0 || inventory_index >= inventory_size) {
        return;
    }
    inventory_[inventory_index].second = item;
    inventory_[inventory_index].first = quantity;
}

void CharacterInventory::SetEquipItem(GameConstants::EquipPosition equip_pos, const shared_ptr<BaseItem>& equip)
{
    auto equip_pos_int = static_cast<GameConstants::equip_position_t>(equip_pos);
    if (equip_pos_int < 0 || equip_pos_int >= equipment_size) {
        return;
    }
    equipment_[equip_pos_int] = equip;
}

const std::pair<uint32_t, shared_ptr<BaseItem>>& CharacterInventory::GetItem(int32_t inventory_index)
{
    if (inventory_index < 0 || inventory_index >= inventory_size) {
        return std::make_pair(0, nullptr);
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

pair<uint32_t, shared_ptr<BaseItem>> CharacterInventory::ExtractItem(int32_t inventory_index)
{
    auto item = GetItem(inventory_index);
    if (item.first != 0 && item.second != nullptr) {
        inventory_[inventory_index] = std::pair(0, nullptr);
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

void CharacterInventory::CheckInventoryTableData(InventoryItemTable& table)
{
    if (table.type != static_cast<int32_t>(GameConstant.GetItemTypeFromItemId(table.itemid))) {
        std::stringstream ss;
        ss << "wrong data type -  itemid: " << table.itemid << ", type: " << table.type;
        throw StackTraceException(ExceptionType::kSQLError, ss.str().c_str());
    }
    if (table.slot < 0 || table.slot >= inventory_size) {
        std::stringstream ss;
        ss << "wrong inventory slot - itemid: " << table.itemid << ", type: " << table.slot;
        throw StackTraceException(ExceptionType::kSQLError, ss.str().c_str());
    }
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
        if (inventory_[i].second) {
            output << (int32_t)inventory_[i].first;
            output << *inventory_[i].second;
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
            int32_t count = 0;
            input >> count;
            input >> *item;
            inventory_[i].first = count;
            inventory_[i].second = item;
        }
    }
}