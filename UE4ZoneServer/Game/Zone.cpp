#include "Zone.hpp"
#include "Character/Character.hpp"
#include "UE4DevelopmentLibrary/Server/UE4Client.hpp"


void Zone::SpawnPlayer(const std::shared_ptr<class Character>& chr)
{
    std::unique_lock lock(chr_map_guard_);
    // oid 할당
    // broadcast - chr data
    // chr의 clint에다가 map 데이터 전송
    // map.emplace
}

void Zone::AttackMonster(const Character& chr, int64_t mobid)
{
}

void Zone::BroadCast(NioOutPacket& outpacket)
{
    std::shared_lock lock(chr_map_guard_);
    for (const auto& chr : chr_map_) {
        auto client = chr.second->GetWeakClient().lock();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void Zone::BroadCast(NioOutPacket& outpacket, int64_t except_chr_oid)
{
    std::shared_lock lock(chr_map_guard_);
    for (const auto& chr : chr_map_) {
        if (chr.first == except_chr_oid)
            continue;
        auto client = chr.second->GetWeakClient().lock();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}