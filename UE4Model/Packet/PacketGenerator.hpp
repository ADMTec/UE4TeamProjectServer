#pragma once
#include <cstdint>

class OutputStream;
class Item;
class Zone;
class Character;
class Monster;

class PacketGenerator
{
public:
    // kUserEnterTheMap
    static void UserEnterTheMap(OutputStream& out, Zone& zone, const Character& chr);

    // SpawnCharacter
    static void SpawnCharacter(OutputStream& out, const Character& chr);
    static void CharacterLocation(OutputStream& out, const Character& chr, int add);

    // kSetMonsterController
    static void SetMonsterController(OutputStream& out, bool value);

    // kInventoryUpdate
    static void ItemAcquire(OutputStream& out, int32_t inventory_slot, Item* item, int32_t count);
    static void ItemDelete(OutputStream& out, int32_t inventory_slot);
    static void InventoryUpdate(OutputStream& out, 
        bool is_equip1, int32_t slot1, Item* item1, int32_t count1,
        bool is_equip2, int32_t slot2, Item* item2, int32_t count2);

    // kSpawnMonster
    static void SpawnMonster(OutputStream& out, const Monster& mob);

    // kUpdateMonsterAction
    static void UpdateMonsterAction(OutputStream& out, const Monster& mob);
};