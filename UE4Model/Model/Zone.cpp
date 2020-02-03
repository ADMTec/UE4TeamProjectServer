#include "Zone.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "Packet/PacketGenerator.hpp"
#include "Server/Alias.hpp"
#include "Server/ZoneServer.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"



Zone::Zone()
    : state_(Zone::State::kPreparing), instance_id_(-1), map_id_(-1), type_(Zone::Type::kCount), last_object_id_(0), monster_controller_id_(MobControllerNullValue)
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

void Zone::AddNPC(int32_t npc, Location location, Rotation rotation)
{
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kNpc)]);
}

void Zone::SpawnCharacter(const std::shared_ptr<Character>& chr)
{
    std::shared_ptr<Client> client = nullptr;
    {
        std::lock_guard lock(chr->mutex_);
        chr->GetLocation() = this->GetPlayerSpawn();
        chr->GetLocation().x += ((rand() % 5) * 50);
        chr->GetLocation().y += ((rand() % 5) * 50);
        chr->GetRotation() = { 0.0f, 0.0f, 0.0f };
        chr->SetObjectId(this->GetNewObjectId());
        chr->SetZone(this->shared_from_this());
        client = chr->GetClientFromWeak();
    }
    {
        std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);

        UE4OutPacket map_init(UE4OutPacket::AllocBufferLength::k1024Bytes);
        PacketGenerator::UserEnterTheMap(map_init, *this, *chr);
        map_init.MakePacketHead();
        if (client) {
            client->GetSession()->Send(map_init, true, false);
        }
        chrs_.emplace(chr->GetObjectId(), chr);
        int64_t expected{ MobControllerNullValue };
        if (monster_controller_id_.compare_exchange_strong(expected, chr->GetObjectId()))
        {
            UE4OutPacket out;
            PacketGenerator::SetMonsterController(out, true);
            out.MakePacketHead();
            client->GetSession()->Send(out, true, false);
        }
        // Remote
        UE4OutPacket out(UE4OutPacket::AllocBufferLength::k256Bytes);
        PacketGenerator::SpawnCharacter(out, *chr);
        out.MakePacketHead();
        this->BroadCastNoLock(out, chr->GetObjectId());
    }
}

void Zone::RemoveCharacter(int64_t object_id)
{
    bool controller_is_null{ false };
    {
        std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
        chrs_.erase(object_id);
        controller_is_null = (monster_controller_id_ == object_id);
        if (controller_is_null) {
            monster_controller_id_.store(MobControllerNullValue);
        }
    }
    if (controller_is_null)
    {
        auto chr_copy = GetCharacterCopyThreadSafe();
        for (const auto& chr : chr_copy) {
            std::lock_guard lock(chr->mutex_);
            auto client = chr->GetClientFromWeak();
            auto zone = chr->GetZone();
            if (client && zone && zone->GetInstanceId() == this->GetInstanceId()) 
            {
                int64_t expected{ MobControllerNullValue };
                if (monster_controller_id_.compare_exchange_strong(expected, chr->GetObjectId()))
                {
                    UE4OutPacket out;
                    PacketGenerator::SetMonsterController(out, true);
                    out.MakePacketHead();
                    client->GetSession()->Send(out, true, false);

                }
                break;
            }
        }
    }
}

void Zone::AddMonster(const std::shared_ptr<Monster>& mob, Location location, Rotation rotation)
{
    int64_t oid = GetNewObjectId();
    {
        std::lock_guard lock(mob->mutex_);
        mob->SetObjectId(oid);
        mob->GetLocation().x = location.x;
        mob->GetLocation().y = location.y;
        mob->GetLocation().z = location.z;
        mob->GetRotation().x = rotation.x;
        mob->GetRotation().y = rotation.y;
        mob->GetRotation().z = rotation.z;
    }
    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kMonster)]);
    mobs_.emplace(oid, mob);
}

void Zone::SpawnMonster(const std::shared_ptr<Monster>& mob, Location location, Rotation rotation)
{
    AddMonster(mob, location, rotation);
    // notify

    UE4OutPacket out;
    PacketGenerator::SpawnMonster(out, *mob);
    out.MakePacketHead();
    BroadCast(out);
}

void Zone::SpawnNPC(int32_t npc, Location location, Rotation rotation)
{

}

//void Zone::CharacterAttackMob(int64_t attacker_oid, int64_t mob_oid, int32_t id)
//{
//    // 1, 2, 3, 4 일반 어택
//    std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kNpc)]);
//
//}
//
//void Zone::MobAttackCharacter(int64_t attacker_oid, int64_t mob_oid, Location lo, Rotation ro)
//{
//
//}

void Zone::BroadCast(class UE4OutPacket& outpacket)
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    BroadCastNoLock(outpacket);
}

void Zone::BroadCast(class UE4OutPacket& outpacket, int64_t except_chr_oid)
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

std::vector<std::shared_ptr<Character>> Zone::GetCharacterCopyThreadSafe() const
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

std::vector<std::shared_ptr<Monster>> Zone::GetMonsterCopyThreadSafe() const
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kMonster)]);
    return GetMonsterCopy();
}

std::vector<std::shared_ptr<Monster>> Zone::GetMonsterCopy() const
{
    std::vector<std::shared_ptr<Monster>> mob;
    mob.reserve(mobs_.size());
    for (const auto& element : mobs_) {
        mob.emplace_back(element.second);
    }
    return mob;
}

std::shared_ptr<Character> Zone::GetCharacterThreadSafe(int64_t oid) const
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
    return GetCharacter(oid);
}

std::shared_ptr<Character> Zone::GetCharacter(int64_t oid) const
{
    auto iter = chrs_.find(oid);
    if (iter != chrs_.end()) {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<Monster> Zone::GetMonsterThreadSafe(int64_t oid) const
{
    std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kMonster)]);
    return GetMonster(oid);
}

std::shared_ptr<Monster> Zone::GetMonster(int64_t oid) const
{
    auto iter = mobs_.find(oid);
    if (iter != mobs_.end()) {
        return iter->second;
    }
    return nullptr;
}

int64_t Zone::GetMobControllerOid() const
{
    return monster_controller_id_;
}

int64_t Zone::GetNewObjectId()
{
    return last_object_id_.fetch_add(1);
}

void Zone::BroadCastNoLock(UE4OutPacket& outpacket)
{
    for (const auto& chr : chrs_) {
        auto client = chr.second->GetClientFromWeak();
        if (client) {
            client->GetSession()->Send(outpacket, true, false);
        }
    }
}

void Zone::BroadCastNoLock(UE4OutPacket& outpacket, int64_t except_chr_oid)
{
    for (const auto& chr : chrs_) {
        if (chr.first == except_chr_oid)
            continue;
        auto client = chr.second->GetClientFromWeak();
        if (client) {
            client->GetSession()->Send(outpacket, true, false);
        }
    }
}
