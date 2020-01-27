#pragma once
#include "../PawnObject.hpp"
#include "GameConstants.hpp"
#include "CharacterInventory.hpp"
#include "CharacterQuickSlot.hpp"
#include "CharacterSkill.hpp"
#include "CharacterStat.hpp"
#include <memory>


class Character : public PawnObject
{
public:
    Character(const Character&) = delete;
    void operator=(const Character&) = delete;
public:
    Character(int accid, int cid);
    void Initialize();
    void UpdatePawnStat();

    void EquipItem(int32_t item_inventory_index);
    void UnequipItem(int32_t equip_position);
    void ConsumeItem(int32_t item_inventory_index);
    // 합치기가 될 경우 합쳐야함.
    void ChangeInventoryItemPosition(int32_t item_inventory_index, int32_t new_index);
    void DestoryItem(int32_t item_inventory_index);
public:
    void SetWeakClient(const std::weak_ptr<class UE4Client>& client);
    const std::weak_ptr<class UE4Client>& GetWeakClient() const;
private:
    bool CanEquipItem(BaseItem& item);
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    std::weak_ptr<class UE4Client> weak_client_;
    int32_t accid_;
    int32_t cid_;
    std::string name_;
    int32_t zone_id_ = 0;
    int32_t gender_ = 0;
    int32_t face_id_ = 0;
    int32_t hair_id_ = 0;
    int32_t job_ = 0;
    int64_t lv_ = 0;
    int32_t str_ = 0;
    int32_t dex_ = 0;
    int32_t intel_ = 0;
    int32_t gold_ = 0;
    float stamina_ = 0.0f;
    float max_stamina_ = 0.0f;
    float stamina_recovery_ = 0.0f;
    CharacterSkill skill_;
    CharacterInventory inventory_;
    CharacterQuickSlot quick_slot_;
};