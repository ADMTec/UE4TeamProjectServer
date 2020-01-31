#include "Character.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Server/UE4Client.hpp"
#include "../Item/EquipItem.hpp"
#include "../Item/ConsumeItem.hpp"
#include "boost/python.hpp"
#include "Private/PythonScriptEngine.hpp"
#include <algorithm>


struct CharacterTable
{
    char name[100];
    int32_t level;
    int32_t str;
    int32_t dex;
    int32_t intel;
    int32_t job;
    int32_t face;
    int32_t hair;
    int32_t gold;
    int32_t zone_id;
    float x;
    float y;
    float z;
    int32_t gender;

    static const wchar_t* GetSelectQueryFromCid() {
        return L"select name, level, str, dex, intel, job, face, hair, gold, zone, x, y, z, gender from characters where cid = ?";
    }
    static void BindResultSet(class ResultSet& rs, CharacterTable* data) {
        rs.BindString(1, data->name, 100);
        rs.BindInt32(2, &data->level);
        rs.BindInt32(3, &data->str);
        rs.BindInt32(4, &data->dex);
        rs.BindInt32(5, &data->intel);
        rs.BindInt32(6, &data->job);
        rs.BindInt32(7, &data->face);
        rs.BindInt32(8, &data->hair);
        rs.BindInt32(9, &data->gold);
        rs.BindInt32(10, &data->zone_id);
        rs.BindFloat32(11, &data->x);
        rs.BindFloat32(12, &data->y);
        rs.BindFloat32(13, &data->z);
        rs.BindInt32(14, &data->gender);
    }
    static const wchar_t* GetUpdateQueryFromCid() {
        return L"update characters set name = ?, level = ?, str = ?, dex = ?, intel = ?, job = ?, hair = ?, gold = ?, zone = ?, x = ?, y = ?, z = ?, gender = ? where cid = ?";
    }
};

struct InventoryItemTable
{
    int32_t itemid;
    int32_t type;
    int32_t quantity;
    int32_t slot;
    int32_t atk;
    int32_t def;
    int32_t str;
    int32_t dex;
    int32_t intel;
    int32_t equipped;
    static const wchar_t* GetSelectQueryFromCid() {
        return L"select itemid, type, quantity, slot, atk, def, str, dex, intel, equipped from inventoryitems where cid = ?";
    }
    static void BindResultSet(class ResultSet& rs, InventoryItemTable* data) {
        rs.BindInt32(1, &data->itemid);
        rs.BindInt32(2, &data->type);
        rs.BindInt32(3, &data->quantity);
        rs.BindInt32(4, &data->slot);
        rs.BindInt32(5, &data->atk);
        rs.BindInt32(6, &data->def);
        rs.BindInt32(7, &data->str);
        rs.BindInt32(8, &data->dex);
        rs.BindInt32(9, &data->intel);
        rs.BindInt32(10, &data->equipped);
    }
    static const wchar_t* GetDeleteQueryFromCid() {
        return L"delete from inventoryitems where cid = ?";
    }
};

Character::Character(const Character& rhs)
    : PawnObject(rhs)
{
}

void Character::operator=(const Character&)
{
}

Character::Character(int accid, int cid)
    : PawnObject(ZoneObject::Type::kCharacter), accid_(accid), cid_(cid)
{

}

void Character::SetClient(UE4Client* client) {
    std::unique_lock lock(pointer_guard_);
    client_ = client;
}

std::shared_ptr<Zone> Character::GetZone() const
{
    std::shared_lock lock(pointer_guard_);
    return zone_;
}

void Character::SetZone(const std::shared_ptr<Zone>& zone)
{
    std::unique_lock lock(pointer_guard_);
    zone_ = zone;
}

UE4Client* Character::GetClient() const {
    std::shared_lock lock(pointer_guard_);
    return client_;
}


void Character::Initialize(Connection& con)
{
    // DB에서 stat, skill, inventory, quick_slot 초기화하기.
    auto ps = con.GetPreparedStatement();
    ps->PrepareStatement(CharacterTable::GetSelectQueryFromCid());
    ps->SetInt32(1, &cid_);
    auto rs = ps->Execute();

    CharacterTable chr_data;
    CharacterTable::BindResultSet(*rs, &chr_data);
    if (rs->Next()) {
        ZoneObject::GetLocation().x = chr_data.x;
        ZoneObject::GetLocation().y = chr_data.y;
        ZoneObject::GetLocation().z = chr_data.z;
        name_ = chr_data.name;
        map_id_ = chr_data.zone_id;
        gender_ = chr_data.gender;
        face_id_ = chr_data.face;
        hair_id_ = chr_data.hair;
        job_ = chr_data.job;
        lv_ = chr_data.level;
        str_ = chr_data.str;
        dex_ = chr_data.dex;
        intel_ = chr_data.intel;
        gold_ = chr_data.gold;
        rs->Close();
        ps->Close();

        
        ps = con.GetPreparedStatement();
        ps->PrepareStatement(InventoryItemTable::GetSelectQueryFromCid());
        ps->SetInt32(1, &cid_);
        rs = ps->Execute();

        InventoryItemTable inven_data;
        InventoryItemTable::BindResultSet(*rs, &inven_data);
        while (rs->Next()) {
            auto item_type = static_cast<Item::Type>(inven_data.type);
            if (item_type == Item::Type::kEquip) {
                auto equip = std::make_shared<EquipItem>();
                equip->SetItemId(inven_data.itemid);
                equip->SetItemType(item_type);
                equip->SetAddATK(inven_data.atk);
                equip->SetAddDEF(inven_data.def);
                equip->SetAddStr(inven_data.str);
                equip->SetAddDex(inven_data.dex);
                equip->SetAddInt(inven_data.intel);
                if (inven_data.equipped) {
                    auto equip_slot = (inven_data.itemid / 100000) % 10;
                    if (equip_slot >= ToInt32(Equipment::Position::kArmor) && equip_slot <= ToInt32(Equipment::Position::kWeapon)) {
                        equipment_.Equip(static_cast<Equipment::Position>(equip_slot), equip);
                    }
                } else {
                    if (inven_data.slot >= 0 && inven_data.slot < inventory_.inventory_size) {
                        inventory_.Push(inven_data.slot, equip, 1);
                    }
                }
            } else if (item_type == Item::Type::kConsume || item_type == Item::Type::kETC) {
                std::shared_ptr<Item> item;
                if(item_type == Item::Type::kConsume) {
                    item = std::shared_ptr<ConsumeItem>();
                } else {
                    item = std::make_shared<ConsumeItem>();
                }
                item->SetItemId(inven_data.itemid);
                item->SetItemType(item_type);
                if (inven_data.slot >= 0 && inven_data.slot < inventory_.inventory_size) {
                    inventory_.Push(inven_data.slot, item, inven_data.quantity);
                }
            } 
        }
        rs->Close();
        ps->Close();
        ps->PrepareStatement(L"select slot, type, id from quickslot where cid = ?");
        ps->SetInt32(1, &cid_);
        rs = ps->Execute();
        int32_t slot, type, id;
        rs->BindInt32(1, &slot);
        rs->BindInt32(2, &type);
        rs->BindInt32(3, &id);
        while (rs->Next()) {
            if (type >= ToInt32(QuickSlot::Type::kNull) && type <= ToInt32(QuickSlot::Type::kItem) && slot >= 0 && slot < 10) {
                std::lock_guard lock(quick_slot_guard_);
                quick_slot_[slot].type = static_cast<QuickSlot::Type>(type);
                quick_slot_[slot].id = id;
            }
        }
        rs->Close();
        ps->Close();
    } else {
        throw StackTraceException(ExceptionType::kSQLError, "CharacterTable no data");
    }
    UpdatePawnStat();
}

void Character::RecoveryPerSecond()
{
    std::unique_lock lock(stat_gaurd_);
    SetHP(std::clamp(GetHP() + 3.0f, 0.0f, GetMaxHP()));
    stamina_ = std::clamp(stamina_ + stamina_recovery_, 0.0f, max_stamina_);
}

void Character::UpdatePawnStat()
{
    PythonScript::Engine::Instance().ReloadExecute(
        PythonScript::Path::kCharacter, "StatUpdate", "Start", PYTHON_PASSING_BY_REFERENCE(*this));
}

bool Character::InventoryToEquipment(int32_t inventory_index, Equipment::Position pos)
{
    std::scoped_lock lock(equipment_guard_, inventory_guard_);
    if (inventory_index < 0 || inventory_index >= inventory_.inventory_size) {
        return false;
    }
    if (inventory_.HasItem(inventory_index) == false || equipment_.HasEquipItem(pos) == true) {
        return false;
    }
    auto slot = inventory_.Pop(inventory_index);
    std::shared_ptr<EquipItem> equip = std::dynamic_pointer_cast<EquipItem>(slot.item);
    if (!equip) {
        return false;
    }
    equipment_.Equip(pos, equip);
    return true;
}

bool Character::EquipmentToInventory(Equipment::Position pos)
{
    int slot_index = -1;
    {
        std::unique_lock lock(inventory_guard_);
        slot_index = inventory_.GetEmptySlotIndex();
    }
    return Character::EquipmentToInventory(pos, slot_index);
}

bool Character::EquipmentToInventory(Equipment::Position pos, int32_t inventory_index)
{
    std::scoped_lock lock(equipment_guard_, inventory_guard_);
    if (inventory_index < 0 || inventory_index >= inventory_.inventory_size) {
        return false;
    }
    if (inventory_.HasItem(inventory_index) == true || equipment_.HasEquipItem(pos) == false) {
        return false;
    }
    auto unequip = equipment_.UnEquip(pos);
    inventory_.Push(inventory_index, unequip, 1);
    return true;
}

bool Character::UseConsumeItem(int32_t inventory_index)
{
    // 갯수 줄이고 효과 적용
    return true;
}


bool Character::ChangeInventoryItemPosition(int32_t prev_index, int32_t new_index)
{
    std::unique_lock lock(inventory_guard_);
    if (prev_index < 0 || prev_index >= inventory_.inventory_size ||
        new_index < 0 || new_index >= inventory_.inventory_size) {
        return false;
    }
    if (inventory_.HasItem(prev_index) == false) {
        return false;
    }
    Inventory::Slot prev = inventory_.Pop(prev_index);
    if (inventory_.HasItem(new_index)) {
        Inventory::Slot now = inventory_.Pop(new_index);
        if (now.item->GetItemType() != Item::Type::kEquip && (now.item->GetItemId() == prev.item->GetItemId())) {
            prev.count += now.count;
        } else {
            inventory_.Push(prev_index, now.item, now.count);
        }
    }
    inventory_.Push(new_index, prev.item, prev.count);
    return true;
}

bool Character::DestoryItem(int32_t inventory_index)
{
    std::unique_lock lock(inventory_guard_);
    if (inventory_index < 0 || inventory_index >= inventory_.inventory_size) {
        return false;
    }
    if (inventory_.HasItem(inventory_index) == false) {
        return false;
    }
    auto desert = inventory_.Pop(inventory_index);
    return true;
}

std::array<QuickSlot, 10> Character::GetQuickSlot() const
{
    std::shared_lock lock(quick_slot_guard_);
    return quick_slot_;
}

const std::string& Character::GetName() const
{
    return name_;
}

int32_t Character::GetLevel() const
{
    return lv_;
}

int32_t Character::GetMapId() const
{
    return map_id_;
}

int32_t Character::GetGender() const
{
    return gender_;
}

int32_t Character::GetHair() const
{
    return hair_id_;
}

int32_t Character::GetFace() const
{
    return face_id_;
}

#undef GetJob
int32_t Character::GetJob() const
{
    return job_;
}

int32_t Character::GetStr() const
{
    return str_;
}

int32_t Character::GetDex() const
{
    return dex_;
}

int32_t Character::GetIntel() const
{
    return intel_;
}

int32_t Character::GetGold() const
{
    return gold_;
}

float Character::GetStamina() const
{
    return stamina_;
}

float Character::GetMaxStamina() const
{
    return max_stamina_;
}

const Equipment& Character::GetEquipment() const
{
    return equipment_;
}

const Inventory& Character::GetInventory() const
{
    return inventory_;
}

void Character::Write(OutputStream& output) const
{
}

void Character::Read(InputStream& input)
{
}
