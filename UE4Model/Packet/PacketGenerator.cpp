#include "PacketGenerator.hpp"
#include "../NetworkOpcode.h"
#include "UE4DevelopmentLibrary/nio.hpp"
#include "PacketHelper.hpp"
#include "Model/Character/Character.hpp"
#include "Model/Monster/Monster.hpp"
#include "Model/Zone.hpp"


void PacketGenerator::UserEnterTheMap(OutputStream& out, Zone& zone, const Character& chr)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kUserEnterTheMap));
    PacketHelper::WriteMapData(out, zone, chr.GetObjectId());

    PacketHelper::WriteBaseCharacter(out, chr);
    PacketHelper::WriteCharacterInventory(out, chr);
    
    // QuickSlot
    for (const auto& quick_slot : chr.GetQuickSlot()) {
        PacketHelper::WriteQuickSlot(out, quick_slot);
    }

    // Skill
    // Quest
}

void PacketGenerator::SpawnCharacter(OutputStream& out, const Character& chr)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kSpawnCharacter));
    PacketHelper::WriteBaseCharacter(out, chr);
}

void PacketGenerator::CharacterLocation(OutputStream& out, const Character& chr, int add)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kUpdateCharacterPosition));
    out << chr.GetObjectId();
    out << add;
    out << chr.GetLocation();
    out << chr.GetRotation();
}

void PacketGenerator::SetMonsterController(OutputStream& out, bool value)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kSetMonsterController));
    out.WriteInt8(value);
}

enum InventoryUpdatetype {
    kAcquire = 0,
    kDelete = 1,
    kSlotUpdate = 2,
};

void PacketGenerator::ItemAcquire(OutputStream& out, int32_t inventory_slot, Item* item, int32_t count)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kInventoryUpdate));
    out.WriteInt8(InventoryUpdatetype::kAcquire);
}

void PacketGenerator::ItemDelete(OutputStream& out, int32_t inventory_slot)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kInventoryUpdate));
    out.WriteInt8(InventoryUpdatetype::kDelete);
}

void PacketGenerator::InventoryUpdate(OutputStream& out, bool is_equip1, int32_t slot1, Item* item1, int32_t count1, bool is_equip2, int32_t slot2, Item* item2, int32_t count2)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kInventoryUpdate));
    out.WriteInt8(InventoryUpdatetype::kSlotUpdate);
    out.WriteInt8(!is_equip1);
    out.WriteInt32(slot1);
    PacketHelper::WriteItem(out, item1);
    out.WriteInt32(count1);

    out.WriteInt8(!is_equip2);
    out.WriteInt32(slot2);
    PacketHelper::WriteItem(out, item2);
    out.WriteInt32(count2);
}

void PacketGenerator::SpawnMonster(OutputStream& out, const Monster& mob)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kSpawnMonster));
    PacketHelper::WriteMonsterData(out, mob);
}

void PacketGenerator::UpdateMonsterAction(OutputStream& out, const Monster& mob)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kUpdateMonsterAction));
    out << mob.GetObjectId();
    out << mob.GetState();
    out << mob.GetLocation();
    out << mob.GetRotation();
}

void PacketGenerator::CharacterAttack(OutputStream& out, const Character& chr, int32_t id)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kNotifyCharacterAttack));
    out << chr.GetObjectId();
    out << id;
    out << chr.GetLocation();
    out << chr.GetRotation();
}

void PacketGenerator::NotifyMatchResult(OutputStream& out)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kNotifyMatchResult));
}

void PacketGenerator::NotifyMatchCanceled(OutputStream& out)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kNotifyMatchCanceled));
}
