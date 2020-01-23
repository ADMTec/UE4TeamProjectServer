#include "Zone.hpp"
#include "Character/Character.hpp"
#include "UE4DevelopmentLibrary/Server/UE4Client.hpp"
#include "Data/ZoneDataProvider.hpp"
#include "Data/MonsterTemplateProvider.hpp"


std::shared_ptr<Zone> Zone::CreateInstance(int64_t map_id)
{
    auto zone_data = ZoneDataProvider::Instance().GetData(map_id);
    if (!zone_data)
        return nullptr;

    const auto& data_ptr = (*zone_data);

    std::shared_ptr<Zone> zone = std::make_shared<Zone>();
    zone->map_id_ = data_ptr->map_id;
    zone->type_ = static_cast<Zone::Type>(data_ptr->type);
    zone->player_spawn_ = data_ptr->player_spawn;
    for (const auto& spawn_point : data_ptr->spawn_point) {
        auto type = static_cast<ZoneData::SpawnPoint::Type>(spawn_point.type);
        if (type == ZoneData::SpawnPoint::Type::kMonster) {
            auto opt_mob = MonsterTemplateProvider::Instance().GetData(spawn_point.id);
            if (opt_mob) {
                Monster mob = *opt_mob;
                mob.SetTemplateId(spawn_point.id);
                mob.SetObjectId(zone->GetNewObjectId());
                mob.GetLocation() = spawn_point.location;
                mob.GetRotation() = spawn_point.rotation;
                zone->monsters_.emplace(mob.GetObjectId(), mob);
            }
        } else if (type == ZoneData::SpawnPoint::Type::kNpc) {
            // ...
        } else if (type == ZoneData::SpawnPoint::Type::kReactor) {
            // ...
        }
    }
    return zone;
}

void Zone::SpawnPlayer(const std::shared_ptr<Character>& chr)
{`  `
    
    // oid 할당
    chr->SetObjectId(GetNewObjectId());
    std::unique_lock lock(character_map_guard_);

    // broadcast - chr data
    // chr의 clint에다가 map 데이터 전송
    // map.emplace
    characters_.emplace(chr->GetObjectId(), chr);
}

bool Zone::SpawnMonster(int32_t mobid, Location location, Rotation rotation)
{
    // mob 생성
    auto opt_mob = MonsterTemplateProvider::Instance().GetData(mobid);
    if (opt_mob)
    {
        Monster mob = *opt_mob;
        Zone::SpawnMonster(mob, location, rotation);
        return true;
    }
    return false;
}

void Zone::SpawnMonster(Monster& mob, Location location, Rotation rotation)
{
    mob.SetObjectId(GetNewObjectId());
    std::unique_lock lock(monster_map_guard_);
    // emplace
    // notify
}

void Zone::SpawnNpc(int32_t npcid, Location location, Rotation rotation)
{
}

void Zone::AttackMonster(const Character& chr, int64_t mobid)
{

}

void Zone::BroadCast(NioOutPacket& outpacket)
{
    std::shared_lock lock(character_map_guard_);
    for (const auto& chr : characters_) {
        auto client = chr.second->GetWeakClient().lock();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void Zone::BroadCast(NioOutPacket& outpacket, int64_t except_chr_oid)
{
    std::shared_lock lock(character_map_guard_);
    for (const auto& chr : characters_) {
        if (chr.first == except_chr_oid)
            continue;
        auto client = chr.second->GetWeakClient().lock();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

int64_t Zone::GetNewObjectId()
{
    return last_object_id_.fetch_add(1);
}
