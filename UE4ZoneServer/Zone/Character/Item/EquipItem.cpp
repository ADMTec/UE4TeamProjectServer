#include "EquipItem.hpp"
#include "../Character.hpp"


EquipItem::EquipItem(int32_t itemid)
    : BaseItem(itemid, GameConstants::ItemType::kEquip)
{
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

void EquipItem::SetAddLuk(int32_t value)
{
    add_luk_ = value;
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

int32_t EquipItem::GetAddLuk() const
{
    return add_luk_;
}

void EquipItem::Write(OutputStream& output) const
{
    BaseItem::Write(output);
    output << add_str_;
    output << add_dex_;
    output << add_int_;
    output << add_luk_;
}

void EquipItem::Read(InputStream& input)
{
    BaseItem::Read(input);
    input >> add_str_;
    input >> add_dex_;
    input >> add_int_;
    input >> add_luk_;
}