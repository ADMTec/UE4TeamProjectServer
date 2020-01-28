#include "ZoneImpl.hpp"
#include "UE4DevelopmentLibrary/Server.hpp"


void ZoneImpl::AddMonster(Monster& mob, Location location, Rotation rotation)
{
    mob.SetObjectId(GetNewObjectId());
    mob.GetLocation().x = location.x;
    mob.GetLocation().y = location.y;
    mob.GetLocation().z = location.z;
    mob.GetRotation().x = rotation.x;
    mob.GetRotation().y = rotation.y;
    mob.GetRotation().z = rotation.z;
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kMonster)]);
    mobs_.emplace(mob.GetObjectId(), mob);
}

void ZoneImpl::AddNPC(int32_t npc, Location location, Rotation rotation)
{
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kNpc)]);
}

void ZoneImpl::SpawnPlayer(const std::shared_ptr<class Character>& chr)
{
    chr->SetObjectId(GetNewObjectId());
    // chr에 location set;
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);

    // broadcast - chr data
    // chr의 clint에다가 map 데이터 전송
    // map.emplace
    chrs_.emplace(chr->GetObjectId(), chr);
}

void ZoneImpl::SpawnMonster(Monster& mob, Location location, Rotation rotation)
{
    AddMonster(mob, location, rotation);
    // notify
}

void ZoneImpl::SpawnNPC(int32_t npc, Location location, Rotation rotation)
{

}

void ZoneImpl::AttackMonster(const Character& chr, int64_t mob_obj_id)
{
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kNpc)]);
}

void ZoneImpl::BroadCast(class NioOutPacket& outpacket)
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    for (const auto& chr : chrs_) {
        auto client = chr.second->GetClient();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void ZoneImpl::BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid)
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    for (const auto& chr : chrs_) {
        if (chr.first == except_chr_oid)
            continue;
        auto client = chr.second->GetClient();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

int64_t ZoneImpl::GetInstanceId() const
{
    return instance_id_;
}

void ZoneImpl::SetInstanceId(int64_t id)
{
    instance_id_ = id;
}

int32_t ZoneImpl::GetMapId() const
{
    return map_id_;
}

void ZoneImpl::SetMapId(int32_t map_id)
{
    map_id_ = map_id;
}

Zone::Type ZoneImpl::GetType() const
{
    return type_;
}

void ZoneImpl::SetType(Zone::Type type)
{
    type_ = type;
}

void ZoneImpl::SetPlayerSpawn(Location location)
{
    player_spawn_ = location;
}

int64_t ZoneImpl::GetNewObjectId()
{
    return last_object_id_.fetch_add(1);
}