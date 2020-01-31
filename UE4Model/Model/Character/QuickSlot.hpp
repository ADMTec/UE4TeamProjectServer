#pragma once
#include <array>


struct QuickSlot
{
public:
    enum class Type : int8_t {
        kNull = 0,
        kSkill = 1,
        kItem = 2,
    };
public:
    QuickSlot() : type(Type::kNull), id(-1)
    {
    }
    QuickSlot::Type type;
    int32_t id;
};