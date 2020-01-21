#pragma once
#include "BaseItem.hpp"


class EquipItem : public BaseItem
{
public:
    explicit EquipItem(int32_t itemid = -1);

    void SetAddStr(int32_t value);
    void SetAddDex(int32_t value);
    void SetAddInt(int32_t value);
    void SetAddLuk(int32_t value);

    int32_t GetAddStr() const;
    int32_t GetAddDex() const;
    int32_t GetAddInt() const;
    int32_t GetAddLuk() const;
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    int32_t add_str_ = 0;
    int32_t add_dex_ = 0;
    int32_t add_int_ = 0;
    int32_t add_luk_ = 0;
};