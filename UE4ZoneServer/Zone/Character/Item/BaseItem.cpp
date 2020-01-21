#include "BaseItem.hpp"

BaseItem::BaseItem(int32_t itemid, GameConstants::ItemType type)
    : itemid_(itemid), type_(static_cast<GameConstants::item_type_t>(type))
{
}

BaseItem::BaseItem()
    : itemid_(-1)
    , type_(static_cast<GameConstants::item_type_t>(GameConstants::ItemType::kETC))
{
}

BaseItem::~BaseItem()
{
}

GameConstants::ItemType BaseItem::GetItemType() const
{
    return static_cast<GameConstants::ItemType>(type_);
}

int32_t BaseItem::GetItemId() const
{
    return itemid_;
}

void BaseItem::Write(OutputStream& output) const
{
    output << type_;
    output << itemid_;
}

void BaseItem::Read(InputStream& input)
{
    input >> type_;
    input >> itemid_;
}