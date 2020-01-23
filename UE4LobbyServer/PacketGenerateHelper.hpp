#pragma once
#include <cstdint>

class PacketGenerateHelper
{
public:
    struct LobbyCharacterInfo
    {
        int32_t slot;
        char name[100]; // std::string
        int32_t gender;
        int32_t level;
        int32_t str;
        int32_t dex;
        int32_t intel;
        int32_t job;
        int32_t face;
        int32_t hair;
        int32_t gold;
        int32_t zone;
        float x;
        float y;
        float z;
        int32_t armor_itemid = -1;
        int32_t hand_itemid = -1;
        int32_t shoes_itemid = -1;
        int32_t weapon_itemid = -1;
        int32_t sub_weapon_itemid = -1;
    };
public:
    static void WriteLobbyCharacterInfo(class OutputStream& output, const LobbyCharacterInfo& info);
};

