#include "PacketHelper.hpp"
#include "UE4DevelopmentLibrary/Nio.hpp"
#include "Model/Character/Character.hpp"
#include "Model/Item/EquipItem.hpp"
#include "Model/Monster/Monster.hpp"
#include "Model/Zone.hpp"


void PacketHelper::WriteMapData(OutputStream& out, const Zone& zone, int64_t except_chr_oid)
{
    out << zone.GetMapId();
    const auto& chrs = zone.GetCharacterCopy();
    const auto& mobs = zone.GetMonsterCopy();

    out << (int32_t)chrs.size();
    for (const auto& chr : chrs) {
        if (except_chr_oid != -1 && chr->GetObjectId() != except_chr_oid) {
            WriteBaseCharacter(out, *chr);
        }
    }
    out << (int32_t)mobs.size();
    for (const auto& mob : mobs) {
        WriteMonsterData(out, mob);
    }

    // NPC
    // out << npc.size();
    //out << (int32_t)0;

    // Reactor
    // out << reactor.size();
    //out << (int32_t)0;
}

void PacketHelper::WriteBaseCharacter(OutputStream& out, const Character& chr)
{
    out << chr.GetObjectId();
    WriteCharacterPosition(out, chr);
    WriteCharacterStat(out, chr);
    WriteCharacterEquipment(out, chr);
}

void PacketHelper::WriteCharacterPosition(OutputStream& out, const Character& chr)
{
    out << chr.GetLocation(); // float 3
    out << chr.GetRotation(); // float 3
}

void PacketHelper::WriteCharacterStat(OutputStream& out, const Character& chr)
{
    out << chr.GetHP();
    out << chr.GetMaxHP();
    out << chr.GetAttackMin();
    out << chr.GetAttackMax();
    out << chr.GetAttackRange();
    out << chr.GetAttackSpeed();
    out << chr.GetDefence();
    out << chr.GetSpeed();
    out << chr.GetName();
    out << chr.GetLevel();
    out << chr.GetGender();
    out << chr.GetFace();
    out << chr.GetHair();
    out << chr.GetStr();
    out << chr.GetDex();
    out << chr.GetIntel();
    out << chr.GetStamina();
    out << chr.GetMaxStamina();
}

void PacketHelper::WriteCharacterEquipment(OutputStream& out, const Character& chr)
{
    const auto& data = chr.GetEquipmentData();
    for (size_t i = 0; i < data.size(); ++i)
    {
        int32_t has_item = data[i].operator bool();
        if (has_item) {
            WriteItem(out, data[i].get());
            out << (int32_t)1;
        } else {
            out << has_item;
        }
    }
}

void PacketHelper::WriteCharacterInventory(OutputStream& out, const Character& chr)
{
    out << chr.GetGold();
    const auto& data = chr.GetInventoryData();
    for (int i = 0; i < Inventory::inventory_size; ++i)
    {
        int32_t has_item = data[i].has_value();
        if (has_item) {
            WriteItem(out, data[i]->item.get());
            out << data[i]->count;
        } else {
            out << has_item;
        }
    }
}

void PacketHelper::WriteMonsterData(OutputStream& out, const Monster& mob)
{
    out << mob.GetTemplateId();
    out << mob.GetObjectId();
    out << mob.GetLocation();
    out << mob.GetRotation();
    out << mob.GetHP();
    out << mob.GetMaxHP();
    out << mob.GetAttackMin();
    out << mob.GetAttackMax();
    out << mob.GetAttackRange();
    out << mob.GetAttackSpeed();
    out << mob.GetDefence();
    out << mob.GetSpeed();
}

void PacketHelper::WriteItem(OutputStream& out, const Item* item)
{
    out << static_cast<int32_t>(item->GetItemType());
    out << item->GetItemId();
    if (item->GetItemType() == Item::Type::kEquip) {
        auto equip = reinterpret_cast<const EquipItem*>(item);
        if (equip) {
            out << equip->GetAddATK();
            out << equip->GetAddDEF();
            out << equip->GetAddStr();
            out << equip->GetAddDex();
            out << equip->GetAddInt();
        }
    }
}


void PacketHelper::WriteQuickSlot(OutputStream& out, const QuickSlot& slot)
{
    out << static_cast<int8_t>(slot.type);
    if (slot.type != QuickSlot::Type::kNull) {
        out << slot.id;
    }
}