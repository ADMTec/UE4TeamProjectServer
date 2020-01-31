#pragma once
#include "../UE4Model.hpp"
#include "../PawnObject.hpp"
#include "Inventory.hpp"
#include "Equipment.hpp"
#include "QuickSlot.hpp"
#include "CharacterSkill.hpp"
#include <shared_mutex>
#include <memory>


class UE4Client;
class Zone;

UE4MODEL_DLLEXPORT class UE4MODEL_DLLCLASS std::shared_mutex;
UE4MODEL_DLLEXPORT template class UE4MODEL_DLLCLASS std::shared_ptr<Zone>;

class UE4MODEL_DLLCLASS Character : public PawnObject
{
private:
    Character(const Character&);
    void operator=(const Character&);
public:
    Character(int accid, int cid);
    
    UE4Client* GetClient() const;
    void SetClient(UE4Client* client);
    std::shared_ptr<Zone> GetZone() const;
    void SetZone(const std::shared_ptr<Zone>& zone);
    void Initialize(class Connection& con);

    void RecoveryPerSecond();
    
    bool InventoryToEquipment(int32_t inventory_index, Equipment::Position pos);
    bool EquipmentToInventory(Equipment::Position pos);
    bool EquipmentToInventory(Equipment::Position pos, int32_t inventory_index);
    bool UseConsumeItem(int32_t inventory_index);
    // 합치기가 될 경우 합쳐야함.
    bool ChangeInventoryItemPosition(int32_t prev_index, int32_t new_index);
    bool DestoryItem(int32_t inventory_index);

    std::array<QuickSlot, 10> GetQuickSlot() const;
public:
    const std::string& GetName() const;
    int32_t GetLevel() const;
    int32_t GetMapId() const;
    int32_t GetGender() const;
    int32_t GetHair() const;
    int32_t GetFace() const;
    
#undef GetJob
    int32_t GetJob() const;
    int32_t GetStr() const;
    int32_t GetDex() const;
    int32_t GetIntel() const;
    int32_t GetGold() const;
    float GetStamina() const;
    float GetMaxStamina() const;
    const Equipment& GetEquipment() const;
    const Inventory& GetInventory() const;
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    void UpdatePawnStat();
private:
    mutable std::shared_mutex pointer_guard_;
    UE4Client* client_ = nullptr;
    std::shared_ptr<Zone> zone_;

    mutable std::shared_mutex stat_gaurd_;
    int32_t accid_;
    int32_t cid_;
    std::string name_;
    int32_t map_id_ = 0;
    int64_t lv_ = 0;
    int32_t gender_ = 0;
    int32_t face_id_ = 0;
    int32_t hair_id_ = 0;
    int32_t job_ = 0;
    int32_t str_ = 0;
    int32_t dex_ = 0;
    int32_t intel_ = 0;
    int32_t gold_ = 0;
    float stamina_ = 0.0f;
    float max_stamina_ = 0.0f;
    float stamina_recovery_ = 0.0f;

    mutable std::shared_mutex equipment_guard_;
    Equipment equipment_;

    mutable std::shared_mutex inventory_guard_;
    Inventory inventory_;

    mutable std::shared_mutex quick_slot_guard_;
    std::array<QuickSlot, 10> quick_slot_;

    CharacterSkill skill_;
    
};