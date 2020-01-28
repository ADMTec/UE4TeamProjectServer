#include "PacketGenerateHelper.hpp"
#include "UE4DevelopmentLibrary/Nio.hpp"


void PacketGenerateHelper::WriteLobbyCharacterInfo(OutputStream& output, const LobbyCharacterInfo& info)
{
    output << info.slot;
    output << std::string(info.name);
    output << info.level;
    output << info.str;
    output << info.dex;
    output << info.intel;
    output << info.job;
    output << info.face;
    output << info.hair;
    output << info.gold;
    output << info.zone;
    output << info.x;
    output << info.y;
    output << info.z;
    output << info.armor_itemid;
    output << info.hand_itemid;
    output << info.shoes_itemid;
    output << info.weapon_itemid;
    output << info.sub_weapon_itemid;
    output << info.gender;
}


void PacketGenerateHelper::ReadLobbyCharacterInfo(InputStream& input, LobbyCharacterInfo& info)
{
    std::string name;
    input >> info.slot;
    input >> name;
    input >> info.level;
    input >> info.str;
    input >> info.dex;
    input >> info.intel;
    input >> info.job;
    input >> info.face;
    input >> info.hair;
    input >> info.gold;
    input >> info.zone;
    input >> info.x;
    input >> info.y;
    input >> info.z;
    input >> info.armor_itemid;
    input >> info.hand_itemid;
    input >> info.shoes_itemid;
    input >> info.weapon_itemid;
    input >> info.sub_weapon_itemid;
    input >> info.gender;

    memset(info.name, 0, 100);
    memcpy(info.name, name.c_str(), name.length());
}