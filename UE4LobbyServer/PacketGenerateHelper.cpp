#include "PacketGenerateHelper.hpp"
#include "UE4DevelopmentLibrary/Nio.hpp"


void PacketGenerateHelper::WriteLobbyCharacterInfo(OutputStream& output, const LobbyCharacterInfo& info)
{
    output << info.slot;
    output << std::string(info.name);
    output << info.gender;
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
}
