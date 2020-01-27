#include "Character.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Server/UE4Client.hpp"
#include "Provider/DatabaseTable.hpp"


Character::Character(int accid, int cid)
    : PawnObject(ZoneObject::Type::kCharacter), accid_(accid), cid_(cid)
    , skill_(*this), inventory_(*this), quick_slot_(*this)
{
}

void Character::Initialize()
{
    // DB에서 stat, skill, inventory, quick_slot 초기화하기.
    auto con = ODBCConnectionPool::Instance().GetConnection();
    auto ps = con->GetPreparedStatement();
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
        zone_id_ = chr_data.zone_id;
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
        inventory_.Initialize(cid_, *con);
    } else {
        throw StackTraceException(ExceptionType::kSQLError, "CharacterTable no data");
    }
    UpdatePawnStat();
}

void Character::UpdatePawnStat()
{
    SetAttackMin(inventory_.GetTotalAttackMin());
    SetAttackMax(inventory_.GetTotalAttackMax());
    //SetAttackRange();
    //SetAttackSpeed();
    SetDefence(inventory_.GetTotalAttackMin());
    SetSpeed(inventory_.GetTotalSpeed());
}

void Character::EquipItem(int32_t item_inventory_index)
{
    // 해당 아이템의 착용 조건이 만족되지 않으면 return
    auto item = inventory_.GetItem(item_inventory_index);
    if (item.second && CanEquipItem(*item.second)) 
    {
        auto opt_equip_pos = GameConstant.GetEquipPositionFromItemId(item.second->GetItemId());
        if (opt_equip_pos) 
        {
            auto off_item = inventory_.ExtractEquipItem(*opt_equip_pos);
            inventory_.SetEquipItem(*opt_equip_pos, item.second);
            if (off_item) {
                inventory_.SetItem(item_inventory_index, off_item, 1);
            }
            inventory_.NotifyEquipAndInventorySlotUpdate(*opt_equip_pos, item_inventory_index);
        }
        UpdatePawnStat();
    }
}

void Character::UnequipItem(int32_t equip_position)
{
    // 인벤토리에 빈 공간이 없을 시 X
    std::optional<int32_t> slot = inventory_.GetEmptryInventorySpaceIndex();
    if (!slot) {
        return;
    }
    auto equip_pos = GameConstant.GetEquipPositionFromEquipPosInt(equip_position);
    if (!equip_pos) {
        return;
    }
    auto item = inventory_.ExtractEquipItem(*equip_pos);
    inventory_.SetItem(*slot, item, 1);
    UpdatePawnStat();
}

void Character::ConsumeItem(int32_t item_inventory_index)
{
}

void Character::ChangeInventoryItemPosition(int32_t item_inventory_index, int32_t new_index)
{
}

void Character::DestoryItem(int32_t item_inventory_index)
{
}

void Character::SetWeakClient(const std::weak_ptr<class UE4Client>& client)
{
    weak_client_ = client;
}

const std::weak_ptr<class UE4Client>& Character::GetWeakClient() const
{
    return weak_client_;
}

bool Character::CanEquipItem(BaseItem& item)
{
    int32_t itemid = item.GetItemId();
    auto type = GameConstant.GetItemTypeFromItemId(itemid);
    if (type != GameConstants::ItemType::kEquip) {
        return false;
    }
    // 아이템 데이터 Provider 불러온 후 req stat 비교 후 케릭터 스텟이 더 적으면 ㅂㅂ
    return true;
}

void Character::Write(OutputStream& output) const
{
}

void Character::Read(InputStream& input)
{
}
