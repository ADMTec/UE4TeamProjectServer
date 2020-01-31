#pragma once
#include "Item.hpp"
#include <memory>


class EquipItem : public Item
{
public:
    explicit EquipItem(int32_t itemid = -1);

    void SetAddATK(int32_t value);
    void SetAddDEF(int32_t value);
    void SetAddStr(int32_t value);
    void SetAddDex(int32_t value);
    void SetAddInt(int32_t value);

    int32_t GetAddATK() const;
    int32_t GetAddDEF() const;
    int32_t GetAddStr() const;
    int32_t GetAddDex() const;
    int32_t GetAddInt() const;
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    int32_t add_atk_ = 0;
    int32_t add_def_ = 0;
    int32_t add_str_ = 0;
    int32_t add_dex_ = 0;
    int32_t add_int_ = 0;
};