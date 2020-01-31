#include "ConsumeItem.hpp"

ConsumeItem::ConsumeItem(int32_t itemid)
    : Item(itemid, Item::Type::kConsume)
{
}

void ConsumeItem::Write(OutputStream& output) const
{
    Item::Write(output);
}

void ConsumeItem::Read(InputStream& input)
{
    Item::Read(input);
}
