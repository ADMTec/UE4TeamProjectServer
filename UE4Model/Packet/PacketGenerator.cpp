#include "PacketGenerator.hpp"
#include "../NetworkOpcode.h"
#include "UE4DevelopmentLibrary/nio.hpp"
#include "PacketHelper.hpp"
#include "Model/Character/Character.hpp"


void PacketGenerator::UserEnterTheMap(OutputStream& out, Zone& zone, Character& chr)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kUserEnterTheMap));
    PacketHelper::WriteBaseCharacter(out, chr);
    PacketHelper::WriteCharacterInventory(out, chr.GetInventory(), chr.GetGold());
    
    // QuickSlot
    for (const auto& quick_slot : chr.GetQuickSlot()) {
        PacketHelper::WriteQuickSlot(out, quick_slot);
    }

    // Skill
    // Quest

    PacketHelper::WriteMapData(out, zone, chr.GetObjectId());
}

void PacketGenerator::SpawnCharacter(OutputStream& out, Character& chr)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kSpawnCharacter));
    PacketHelper::WriteBaseCharacter(out, chr);
}

void PacketGenerator::CharacterLocation(OutputStream& out, Character& chr, int add)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kUpdateCharacterPosition));
    out << chr.GetObjectId();
    out << add;
    out << chr.GetLocation();
    out << chr.GetRotation();
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

void PacketGenerator::SpawnMonster(OutputStream& out, Monster& mob)
{
    out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kSpawnMonster));
    PacketHelper::WriteMonsterData(out, mob);
}
