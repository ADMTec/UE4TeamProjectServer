#include "ZoneSystem.hpp"
#include "Provider/ZoneDataProvider.hpp"
#include "Provider/MonsterTemplateProvider.hpp"
#include "Pawn/Character/Character.hpp"
#include "UE4DevelopmentLibrary/Server/UE4Client.hpp"
#include "GameConstants.hpp"


Zone::Zone()
    : instance_id_(-1), zone_id(-1), type_(Zone::Type::kCount), last_object_id_(0)
{
}

void Zone::SpawnPlayer(const std::shared_ptr<Character>& chr)
{
    chr->SetObjectId(GetNewObjectId());
    std::unique_lock lock(object_guard_[ZoneObject::Type::kCharacter]);

    // broadcast - chr data
    // chr의 clint에다가 map 데이터 전송
    // map.emplace
    chrs_.emplace(chr->GetObjectId(), chr);
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
    std::unique_lock lock(object_guard_[ZoneObject::Type::kMonster]);
    // emplace
    // notify
}

void Zone::SpawnNpc(int32_t npcid, Location location, Rotation rotation)
{
    std::unique_lock lock(object_guard_[ZoneObject::Type::kNpc]);
}

void Zone::AttackMonster(const Character& chr, int64_t mobid)
{

}

void Zone::BroadCast(NioOutPacket& outpacket)
{
    std::shared_lock lock(object_guard_[ZoneObject::Type::kCharacter]);
    for (const auto& chr : chrs_) {
        auto client = chr.second->GetWeakClient().lock();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void Zone::BroadCast(NioOutPacket& outpacket, int64_t except_chr_oid)
{
    std::shared_lock lock(object_guard_[ZoneObject::Type::kCharacter]);
    for (const auto& chr : chrs_) {
        if (chr.first == except_chr_oid)
            continue;
        auto client = chr.second->GetWeakClient().lock();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

int64_t Zone::GetInstanceId() const
{
    return instance_id_;
}

int32_t Zone::GetZoneId() const
{
    return zone_id;
}

Zone::Type Zone::GetType() const
{
    return type_;
}

int64_t Zone::GetNewObjectId() {
    return last_object_id_.fetch_add(1);
}

//----------------------------------------------------------------------------------------
void Zone::System::Initialize()
{
    for (const auto town_id : GameConstant.town_ids)
    {
        shared_ptr<Zone> town = shared_ptr<Zone>(CreateNewZone(town_id));
        if (town) {
            std::unique_lock lock(zone_guard_[static_cast<int32_t>(Zone::Type::kTown)]);
            town_.emplace(town_id, town);
        }
    }
}

shared_ptr<Zone> Zone::System::CreateNewDungeon(int32_t dungeon_id)
{
    Zone* raw_ptr = CreateNewZone(dungeon_id);
    if (raw_ptr) {
        return nullptr;
    }
    shared_ptr<Zone> dungeon = std::shared_ptr<Zone>(raw_ptr,
        [](Zone* zone) {
            Zone::System::DestroyDungoen(zone->GetInstanceId());
        });
    if (dungeon) {
        std::unique_lock lock(zone_guard_[static_cast<int32_t>(Zone::Type::kDungeon)]);
        dungeon_.emplace(dungeon->instance_id_, dungeon);
        return dungeon;
    }
    return nullptr;
}

void Zone::System::DestroyDungoen(int64_t instance_id)
{
    std::unique_lock lock(zone_guard_[static_cast<int16_t>(Zone::Type::kDungeon)]);
    dungeon_.erase(instance_id);
}

shared_ptr<Zone> Zone::System::GetTown(int64_t town_id)
{
    std::shared_lock lock(zone_guard_[static_cast<int32_t>(Zone::Type::kTown)]);
    auto iter = town_.find(town_id);
    if (iter != town_.end()) {
        return iter->second;
    }
    return nullptr;
}

shared_ptr<Zone> Zone::System::GetDungeon(int64_t instance_id)
{
    std::shared_lock lock(zone_guard_[static_cast<int32_t>(Zone::Type::kDungeon)]);
    auto iter = dungeon_.find(instance_id);
    if (iter != dungeon_.end()) {
        return iter->second;
    }
    return nullptr;
}

Zone* Zone::System::CreateNewZone(int32_t zone_id) {
    static std::atomic<int64_t> new_instance_id = 0;
#undef max
    int64_t new_id = new_instance_id.fetch_add(1) % std::numeric_limits<int64_t>::max();
    auto data = ZoneDataProvider::Instance().GetData(zone_id);
    if (!data) {
        nullptr;
    }
    const auto& data_ptr = *data;

    Zone* zone = new Zone();
    zone->instance_id_ = new_id;
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
                zone->mobs_.emplace(mob.GetObjectId(), mob);
            }
        } else if (type == ZoneData::SpawnPoint::Type::kNpc) {
            // ...
        } else if (type == ZoneData::SpawnPoint::Type::kReactor) {
            // ...
        }
    }
    return zone;
}