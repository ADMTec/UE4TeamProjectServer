#pragma once
#include "../UE4Model.hpp"
#include <array>


class UE4MODEL_DLLCLASS CharacterQuickSlot
{
public:
    struct UE4MODEL_DLLCLASS Slot {
        enum class State : int8_t {
            kNull = 0,
            HasSkill = 1,
            HasItem = 2,
        };
    public:
        CharacterQuickSlot::Slot::State slot_state;
        int32_t id;
    };
public:
    CharacterQuickSlot() = default;
    
    const CharacterQuickSlot::Slot& GetSlot(int32_t index) const;
    void SetSlot(int32_t index, CharacterQuickSlot::Slot::State state, int32_t id);
    const std::array<CharacterQuickSlot::Slot, 10>& GetData() const;
private:
    std::array<Slot, 10> slots_;
};