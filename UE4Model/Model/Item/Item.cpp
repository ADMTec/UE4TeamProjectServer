#include "Item.hpp"
#include "EnumClassHelper.hpp"


Item::Item(int32_t itemid, Item::Type type)
    : itemid_(itemid), type_(type)
{
}

Item::Item()
    : itemid_(-1)
    , type_(Item::Type::kETC)
{
}

Item::~Item()
{
}

Item::Type Item::GetItemType() const
{
    return static_cast<Item::Type>(type_);
}

void Item::SetItemType(Item::Type type)
{
    type_ = type;
}

int32_t Item::GetItemId() const
{
    return itemid_;
}

void Item::SetItemId(int32_t itemid)
{
    itemid_ = itemid;
}

void Item::Write(OutputStream& output) const
{
    output << ToInt32(type_);
    output << itemid_;
}

void Item::Read(InputStream& input)
{
    input >> *reinterpret_cast<int32_t*>(&type_);
    input >> itemid_;
}