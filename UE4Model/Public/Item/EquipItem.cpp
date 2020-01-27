#include "EquipItem.hpp"


EquipItem::EquipItem(int32_t itemid)
    : Item(itemid, Item::Type::kEquip), attack_(0)
{
}

void EquipItem::SetAddATK(int32_t value)
{
    attack_ = value;
}

void EquipItem::SetAddStr(int32_t value)
{
    add_str_ = value;
}

void EquipItem::SetAddDex(int32_t value)
{
    add_dex_ = value;
}

void EquipItem::SetAddInt(int32_t value)
{
    add_int_ = value;
}

int32_t EquipItem::GetAddATK() const
{
    return attack_;
}

int32_t EquipItem::GetAddStr() const
{
    return add_str_;
}

int32_t EquipItem::GetAddDex() const
{
    return add_dex_;
}

int32_t EquipItem::GetAddInt() const
{
    return add_int_;
}

void EquipItem::Write(OutputStream& output) const
{
    Item::Write(output);
    output << attack_;
    output << add_str_;
    output << add_dex_;
    output << add_int_;
}

void EquipItem::Read(InputStream& input)
{
    Item::Read(input);
    input >> attack_;
    input >> add_str_;
    input >> add_dex_;
    input >> add_int_;
}