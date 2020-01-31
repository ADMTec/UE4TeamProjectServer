#include "Inventory.hpp"
#include <cassert>
#include "UE4DevelopmentLibrary/Database.hpp"


Inventory::Slot::Slot(const Slot& rhs)
{
    Inventory::Slot::operator=(rhs);
}

void Inventory::Slot::operator=(const Slot& rhs)
{
    item = rhs.item;
    count = rhs.count;
}

Inventory::Slot::Slot(const std::shared_ptr<Item>& _item, int32_t _count)
    : item(_item), count(_count)
{
}

Inventory::Slot::Slot()
    : Slot(nullptr, 0)
{
}

bool Inventory::HasItem(int32_t slot_index)
{
    return inventory_[slot_index].has_value();
}

bool Inventory::IsFull() const
{
    return GetEmptySlotIndex() == -1;
}

void Inventory::Push(int32_t slot_index, const std::shared_ptr<Item>& item, int32_t count)
{
    inventory_[slot_index].emplace(item, count);
}

const Inventory::Slot& Inventory::Get(int32_t slot_index)
{
    return *inventory_[slot_index];
}

Inventory::Slot Inventory::Pop(int32_t slot_index)
{
    Slot slot = *inventory_[slot_index];
    inventory_[slot_index].reset();
    return slot;
}

int32_t Inventory::GetEmptySlotIndex() const
{
    int32_t index = -1;
    for (int i = 0; i < inventory_size; ++i) {
        if (inventory_[i].has_value() == false) {
            index = i;
            break;
        }
    }
    return index;
}

const std::array<std::optional<Inventory::Slot>, Inventory::inventory_size>& Inventory::GetData() const
{
    return inventory_;
}

void Inventory::Write(OutputStream& output) const
{
    //output << inventory_size;
    for (int i = 0; i < inventory_size; ++i)
    {
        int32_t has_item = inventory_[i].has_value();
        if (has_item) {
            output << *inventory_[i]->item.get();
            output << inventory_[i]->count;
        } else {
            output << has_item;
        }
    }
}

void Inventory::Read(InputStream& input)
{
    //int inventory_size = input.ReadInt32();
    for (int i = 0; i < inventory_size; ++i)
    {
        bool has_item = input.ReadInt8();
        if (has_item) {
            auto type = static_cast<Item::Type>(input.PeekInt32());
            Slot slot;
            if (type == Item::Type::kEquip) {
                slot.item = std::make_shared<EquipItem>();
                input >> *slot.item.get();
            } else if (type == Item::Type::kConsume) {
                slot.item = std::make_shared<ConsumeItem>();
                input >> *slot.item.get();
            } else if (type == Item::Type::kETC) {
                slot.item = std::make_shared<Item>();
                input >> *slot.item.get();
            } else {
                assert(false);
            }
            input >> slot.count;
            inventory_[i] = slot;
        }
    }
}