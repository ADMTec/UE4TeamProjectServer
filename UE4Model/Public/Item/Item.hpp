#pragma once
#include "../UE4Model.hpp"
#include "UE4DevelopmentLibrary/Stream.hpp"


class UE4MODEL_DLLCLASS Item : public SerializeInterface
{
public:
    enum class Type : int32_t {
        kNull = 0,
        kETC = 1,
        kConsume = 2,
        kEquip = 3,
    };
public:
    Item(const Item&) = delete;
    void operator=(const Item&) = delete;
public:
    Item(int32_t itemid, Type type);
    Item();
    virtual ~Item();

    Type GetItemType() const;
    void SetItemType(Item::Type type);
    int32_t GetItemId() const;
    void SetItemId(int32_t itemid);
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    Type type_;
    int32_t itemid_;
};