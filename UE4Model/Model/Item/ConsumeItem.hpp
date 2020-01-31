#pragma once
#include "Item.hpp"

class ConsumeItem : public Item
{
public:
    explicit ConsumeItem(int32_t itemid = -1);
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
};