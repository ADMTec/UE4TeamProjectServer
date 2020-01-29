#pragma once
#include "UE4DevelopmentLibrary/Stream.hpp"
#include "../Ue4model.hpp"
#include "../Item/EquipItem.hpp"
#include "../Item/ConsumeItem.hpp"
#include <optional>
#include <memory>
#include <array>


UE4MODEL_DLLEXPORT template class UE4MODEL_DLLCLASS std::shared_ptr<Item>;


class UE4MODEL_DLLCLASS Inventory : public SerializeInterface
{
public:
    struct UE4MODEL_DLLCLASS Slot {
        Slot(const Slot& rhs);
        void operator=(const Slot& rhs);
        Slot(const std::shared_ptr<Item>& _item, int32_t _count);
        Slot();
        std::shared_ptr<Item> item;
        int32_t count;
    };
    static constexpr int inventory_size = 30;
public:
    bool HasItem(int32_t slot_index);
    bool IsFull() const;
    void Push(int32_t slot_index, const std::shared_ptr<Item>& item, int32_t count);
    const Inventory::Slot& Get(int32_t slot_index);
    Inventory::Slot Pop(int32_t slot_index);
    int32_t GetEmptySlotIndex() const;
    const std::array<std::optional<Slot>, inventory_size>& GetData() const;
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    std::array<std::optional<Slot>, inventory_size> inventory_;
};