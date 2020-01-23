#pragma once
#include "UE4DevelopmentLibrary/Stream.hpp"
#include "Constants/GameConstants.hpp"

class Character;

class BaseItem : public SerializeInterface
{
public:
    BaseItem(const BaseItem&) = delete;
    void operator=(const BaseItem&) = delete;
public:
    BaseItem(int32_t itemid, GameConstants::ItemType type);
    BaseItem();
    virtual ~BaseItem();

    GameConstants::ItemType GetItemType() const;
    int32_t GetItemId() const;
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    GameConstants::item_type_t type_;
    int32_t itemid_;
};