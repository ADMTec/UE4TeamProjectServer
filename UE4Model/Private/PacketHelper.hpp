#pragma once
#include <cstdint>


class OutputStream;
class Character;
class Equipment;
class Inventory;
class ZoneImpl;
class Monster;
class Item;
struct QuickSlot;

class PacketHelper
{
public:
    static void WriteMapData(OutputStream& out, const ZoneImpl& zone);
    static void WriteBaseCharacter(OutputStream& out, const Character& chr);
    static void WriteCharacterPosition(OutputStream& out, const Character& chr);
    static void WriteCharacterStat(OutputStream& out, const Character& chr);
    static void WriteCharacterEquipment(OutputStream& out, const Equipment& equipment);
    static void WriteCharacterInventory(OutputStream& out, const Inventory& inven, int32_t gold);

    // Monster
    static void WriteMonsterData(OutputStream& out, const Monster& mob);

    // Item
    static void WriteItem(OutputStream& out, const Item* item);

    // QuickSlot
    static void WriteQuickSlot(OutputStream& out, const QuickSlot& slot);
};