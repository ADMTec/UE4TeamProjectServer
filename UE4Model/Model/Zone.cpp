#include "Zone.hpp"
#include "UE4DevelopmentLibrary/Server.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "Packet/PacketGenerator.hpp"


Zone::Zone()
    : state_(Zone::State::kPreparing), instance_id_(-1), map_id_(-1), type_(Zone::Type::kCount), last_object_id_(0)
{
}

void Zone::StartUp()
{
}

void Zone::Update()
{
    Clock clock;
    int64_t clock_int64 = clock.to_int64_t();
    time_sum_ += clock_int64 - last_update_time_;

    if (time_sum_ >= 1000)
    {
        std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
        for (const auto& chr_pair : chrs_) {
            chr_pair.second->RecoveryPerSecond();
        }

        time_sum_ -= 1000;
    }
    last_update_time_ = clock_int64;
}

void Zone::Exit()
{
}

void Zone::AddMonster(Monster& mob, Location location, Rotation rotation)
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

void Zone::AddNPC(int32_t npc, Location location, Rotation rotation)
{
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kNpc)]);
}

void Zone::UpdateCharacterPosition(Character& chr, int32_t value)
{
    UE4OutPacket out(UE4OutPacket::AllocBufferLength::k64Bytes);
    PacketGenerator::CharacterLocation(out, chr, value);
    out.MakePacketHead();
    this->BroadCast(out, chr.GetObjectId());
}

void Zone::SpawnCharacter(const std::shared_ptr<class Character>& chr)
{
    chr->SetObjectId(GetNewObjectId());
    {
        std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
        UE4OutPacket map_init(UE4OutPacket::AllocBufferLength::k1024Bytes);
        PacketGenerator::UserEnterTheMap(map_init, *this, *chr);
        map_init.MakePacketHead();
        auto client = chr->GetClient();
        if (client) {
            client->GetSession()->AsyncSend(map_init, false, true);
        }
        chrs_.emplace(chr->GetObjectId(), chr);
    }
    // Remote
    UE4OutPacket out(UE4OutPacket::AllocBufferLength::k256Bytes);
    PacketGenerator::SpawnCharacter(out, *chr);
    out.MakePacketHead();
    this->BroadCastNoLock(out, chr->GetObjectId());
}

void Zone::RemoveCharacter(int64_t object_id)
{
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    chrs_.erase(object_id);
}

void Zone::UpdateCharacterPosition(const std::shared_ptr<Character>& chr, int state)
{
    UE4OutPacket out;
    PacketGenerator::CharacterLocation(out, *chr, state);
    out.MakePacketHead();
    this->BroadCast(out, chr->GetObjectId());
}

void Zone::SpawnMonster(Monster& mob, Location location, Rotation rotation)
{
    AddMonster(mob, location, rotation);
    // notify

    UE4OutPacket out;
    PacketGenerator::SpawnMonster(out, mob);
    out.MakePacketHead();

    BroadCast(out);
}

void Zone::SpawnNPC(int32_t npc, Location location, Rotation rotation)
{

}

void Zone::AttackMonster(const Character& chr, int64_t mob_obj_id)
{
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kNpc)]);
}

void Zone::BroadCast(class NioOutPacket& outpacket)
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    BroadCastNoLock(outpacket);
}

void Zone::BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid)
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    BroadCastNoLock(outpacket, except_chr_oid);
}

Zone::State Zone::GetState() const
{
    return state_;
}

void Zone::SetState(Zone::State state)
{
    state_ = state;
    if (state == Zone::State::kActive) {
        Clock clock;
        last_update_time_ = clock.to_int64_t();
    }
}

int64_t Zone::GetInstanceId() const
{
    return instance_id_;
}

void Zone::SetInstanceId(int64_t id)
{
    instance_id_ = id;
}

int32_t Zone::GetMapId() const
{
    return map_id_;
}

void Zone::SetMapId(int32_t map_id)
{
    map_id_ = map_id;
}

Zone::Type Zone::GetType() const
{
    return type_;
}

void Zone::SetType(Zone::Type type)
{
    type_ = type;
}

Location Zone::GetPlayerSpawn() const
{
    return player_spawn_;
}

void Zone::SetPlayerSpawn(Location location)
{
    player_spawn_ = location;
}

std::vector<std::shared_ptr<Character>> Zone::GetThreadSafeCharacterCopy() const
{
    std::vector<std::shared_ptr<Character>> chr;
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    chr.reserve(chrs_.size());
    for (const auto& element : chrs_) {
        chr.emplace_back(element.second);
    }
    return chr;
}

std::vector<std::shared_ptr<Character>> Zone::GetCharacterCopy() const
{
    std::vector<std::shared_ptr<Character>> chr;
    chr.reserve(chrs_.size());
    for (const auto& element : chrs_) {
        chr.emplace_back(element.second);
    }
    return chr;
}

std::vector<Monster> Zone::GetThreadSafeMonsterCopy() const
{
    std::vector<Monster> mob;
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kMonster)]);
    mob.reserve(mobs_.size());
    for (const auto& element : mobs_) {
        mob.emplace_back(element.second);
    }
    return mob;
}

std::vector<Monster> Zone::GetMonsterCopy() const
{
    std::vector<Monster> mob;
    mob.reserve(mobs_.size());
    for (const auto& element : mobs_) {
        mob.emplace_back(element.second);
    }
    return mob;
}

int64_t Zone::GetNewObjectId()
{
    return last_object_id_.fetch_add(1);
}

void Zone::BroadCastNoLock(NioOutPacket& outpacket)
{
    for (const auto& chr : chrs_) {
        auto client = chr.second->GetClient();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void Zone::BroadCastNoLock(NioOutPacket& outpacket, int64_t except_chr_oid)
{
    for (const auto& chr : chrs_) {
        if (chr.first == except_chr_oid)
            continue;
        auto client = chr.second->GetClient();
        if (client) {
            client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}
