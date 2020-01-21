#pragma once
#include "UE4DevelopmentLibrary/Stream.hpp"
#include "Constants/GameConstants.hpp"
#include "Item/EquipItem.hpp"
#include "Item/ConsumeItem.hpp"
#include <memory>
#include <array>



class Character;
using std::shared_ptr;

class CharacterInventory : public SerializeInterface
{
    static constexpr int inventory_size = GameConstants::inventory_size;
    static constexpr int equipment_size = static_cast<
        int16_t>(GameConstants::EquipPosition::kCount);
public:
    CharacterInventory(Character& chr);

    void Initialize(int32_t db_cid);

    std::optional<int32_t> GetEmptryInventorySpaceIndex() const;
    size_t InventoryCapacity() const;
    size_t InventorySize() const;
    void SetItem(int32_t inventory_index, const shared_ptr<BaseItem>& item);
    void SetEquipItem(GameConstants::EquipPosition equip_pos, const shared_ptr<BaseItem>& equip);
    const shared_ptr<BaseItem>& GetItem(int32_t inventory_index);
    const shared_ptr<BaseItem>& GetEquipItem(GameConstants::EquipPosition equip_pos);
    std::shared_ptr<BaseItem> ExtractItem(int32_t inventory_index);
    std::shared_ptr<BaseItem> ExtractEquipItem(GameConstants::EquipPosition equip_pos);

    void SendSingleInventorySlotUpdateResult(int32_t inventory_slot);
    void SendEquipAndInventorySlotUpdateResult(GameConstants::EquipPosition equip, int32_t inventry);
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    Character& base_;
    std::array<shared_ptr<BaseItem>, equipment_size> equipment_;
    std::array<shared_ptr<BaseItem>, inventory_size> inventory_;
};