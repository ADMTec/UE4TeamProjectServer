#pragma once
#include <cstdint>

class OutputStream;
class Item;
class ZoneImpl;
class Character;
class Monster;

class PacketGenerator
{
public:
    // kUserEnterTheMap
    static void UserEnterTheMap(OutputStream& out, ZoneImpl& zone, Character& chr);

    // SpawnCharacter
    static void SpawnCharacter(OutputStream& out, Character& chr);
    static void CharacterLocation(OutputStream& out, Character& chr, int add);

    // kInventoryUpdate
    static void ItemAcquire(OutputStream& out, int32_t inventory_slot, Item* item, int32_t count);
    static void ItemDelete(OutputStream& out, int32_t inventory_slot);
    static void InventoryUpdate(OutputStream& out, 
        bool is_equip1, int32_t slot1, Item* item1, int32_t count1,
        bool is_equip2, int32_t slot2, Item* item2, int32_t count2);

    // kSpawnMonster
    static void SpawnMonster(OutputStream& out, Monster& mob);
};