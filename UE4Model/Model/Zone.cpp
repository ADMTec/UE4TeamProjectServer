#include "Zone.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "Packet/PacketGenerator.hpp"
#include "Server/Alias.hpp"
#include "Server/ZoneServer.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "System/ZoneSystem.hpp"
#include <cmath>


Zone::Zone()
    : state_(Zone::State::kPreparing)
    , instance_id_(-1)
    , map_id_(-1)
    , type_(Zone::Type::kCount)
    , last_object_id_(0)
    , monster_controller_id_(MobControllerNullValue)
    , next_map_(-1)
    , time_sum_(0)
    , time_second_(0)
    , last_update_time_(0)
{
}

std::string Zone::GetDebugString() const
{
    auto chrs_copy = GetCharacterCopyThreadSafe();
    auto mobs_copy = GetMonsterCopyThreadSafe();
    std::stringstream ss;
    ss << "-----------------------------------------------" << '\n';
    ss << " Debug State [" << (type_ == Type::kTown  ? "Town" : "Dungeon") << "] Instance ID: " << instance_id_ << '\n';
    ss << " Mapid: " << map_id_ << '\n';
    ss << " PlayerSpawn: [" << player_spawn_.x << ", " << player_spawn_.y << ", " << player_spawn_.z << "]" << '\n';
    if (type_ != Type::kTown) {
        ss << " Portal Target Map: " << next_map_ << '\n';
        ss << " Protal Range: " << next_map_portal_range_ << '\n';
        ss << " Portal Location: [" << next_map_portal_.x << ", " << next_map_portal_.y << ", " << next_map_portal_.z << "]" << '\n';
    }
    ss << " Controller: " << monster_controller_id_ << '\n';
    ss << " Runtime MilliSec: " << time_sum_ << '\n';
    if (chrs_copy.empty()) {
        ss << " Character : NULL" << '\n';
    } else {
        for (const auto chr : chrs_copy) {
            std::lock_guard lock(chr->mutex_);
            ss << " Character OID: " << chr->GetObjectId() << ", Client UUID: ";
            auto client = chr->GetClientFromWeak();
            if (client) {
                ss << client->GetUUID().ToString();
            } else {
                ss << "NULL";
            }
            ss << '\n';
        }
    }
    if (mobs_copy.empty()) {
        ss << " Monster : NULL" << '\n';
    } else {
        for (const auto& mob : mobs_copy) {
            std::lock_guard lock(mob->mutex_);
            ss << mob->GetDebugString();
        }
    }
    ss << "-----------------------------------------------" << '\n';
    return ss.str();
}

void Zone::StartUp()
{
}

void Zone::Update()
{
    Clock clock;
    int64_t clock_int64 = clock.to_int64_t();
    time_second_ += clock_int64 - last_update_time_;

    if (time_second_ >= 1000)
    {
        std::shared_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
        for (const auto& chr_pair : chrs_) {
            //std::lock_guard lock(chr_pair.second->mutex_);
            //chr_pair.second->RecoveryPerSecond();
        }

        time_second_ -= 1000;
    }
    time_sum_ += time_second_;
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
    int64_t oid = this->GetNewObjectId();
    UE4OutPacket map_init(UE4OutPacket::AllocBufferLength::k1024Bytes);
    UE4OutPacket remote(UE4OutPacket::AllocBufferLength::k512Bytes);
    {
        std::lock_guard lock(chr->mutex_);
        chr->GetLocation() = this->GetPlayerSpawn();
        chr->GetRotation() = { 0.0f, 0.0f, 0.0f };
        chr->SetObjectId(oid);
        chr->SetZone(this->shared_from_this());
        client = chr->GetClientFromWeak();
    }
    {
        std::scoped_lock lock(chr->mutex_, object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);

        PacketGenerator::UserEnterTheMap(map_init, *this, *chr);
        map_init.MakePacketHead();
        if (client) {
            auto session = client->GetSession();
            session->Send(map_init, true, false);
        }
        chrs_.emplace(oid, chr);
        if (this->GetType() == Zone::Type::kDungeon)
        {
            int64_t expected{ MobControllerNullValue };
            if (monster_controller_id_.compare_exchange_strong(expected, oid))
            {
                UE4OutPacket out;
                PacketGenerator::SetMonsterController(out, true);
                out.MakePacketHead();
                client->GetSession()->Send(out, true, false);
            }
        }

        // Remote
        PacketGenerator::SpawnCharacter(remote, *chr);
        remote.MakePacketHead();
        this->BroadCastNoLock(remote, oid);
    }
}

void Zone::RemoveCharacter(int64_t object_id)
{
    auto chr = this->GetCharacterThreadSafe(object_id);
    if (chr) {
        std::lock_guard lock(chr->mutex_);
        chr->SetZone(nullptr);
    }
    bool controller_is_null{ false };
    {
        std::unique_lock lock(object_guard_[ToInt32(ZoneObject::Type::kCharacter)]);
        int64_t expected = object_id;
        if (this->GetType() == Zone::Type::kDungeon)
        {
            if (monster_controller_id_.compare_exchange_strong(expected, MobControllerNullValue)) {
                controller_is_null = true;
                if (chr) {
                    auto client = chr->GetClientFromWeak();
                    if (client) {
                        UE4OutPacket out;
                        PacketGenerator::SetMonsterController(out, false);
                        out.MakePacketHead();
                        client->GetSession()->Send(out, true, false);
                    }
                }
            }
        }
        chrs_.erase(object_id);
    }
    if ((this->GetType() == Zone::Type::kDungeon) && controller_is_null)
    {
        auto chr_copy = GetCharacterCopyThreadSafe();
        for (const auto& chr : chr_copy) {
            std::lock_guard lock(chr->mutex_);
            auto client = chr->GetClientFromWeak();
            auto zone = chr->GetZoneFromWeak();
            if (client && zone && zone->GetInstanceId() == this->GetInstanceId()) 
            {
                int64_t expected{ MobControllerNullValue };
                if (monster_controller_id_.compare_exchange_strong(expected, chr->GetObjectId())){
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

#define DISTANCE(x1, y1, z1, x2, y2, z2) std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2) + std::pow(z2 - z1, 2))

void Zone::TryChangeZone()
{
    //auto mobs = GetMonsterCopyThreadSafe();
    //for (const auto& mob : mobs) {
    //    if (mob) {
    //        std::lock_guard lock(mob->mutex_);
    //        if (mob->GetState() != static_cast<int32_t>(Monster::State::kDead)) {
    //            return;
    //        }
    //    }
    //}
    auto chrs = GetCharacterCopyThreadSafe();
    for (const auto& chr : chrs) {
        if (chr) {
            Location chr_location;
            {
                std::lock_guard lock(chr->mutex_);
                chr_location = chr->GetLocation();
                std::stringstream ss;
                ss << "location: [" << chr_location.x << ", " << chr_location.y << ", " << chr_location.z << "]\n";;
                std::cout << ss.str();
            }
            float distance = DISTANCE(
                next_map_portal_.x, next_map_portal_.y, next_map_portal_.z, chr_location.x, chr_location.y, chr_location.z);
            if (distance > (next_map_portal_range_ + 200.0f))
                return;
        }
    }
    std::shared_ptr<Zone> new_zone = nullptr;
    if (next_map_ == 100) {
        new_zone = ZoneSystem::GetTown();
    } else {
        new_zone = ZoneSystem::CreateNewInstance(next_map_);
    }
    new_zone->SetState(Zone::State::kActive);

    for (const auto& chr : chrs) {
        auto client = chr->GetClientFromWeak();
        if (client) {
            int64_t oid = -1;
            {
                std::lock_guard lock(chr->mutex_);
                oid = chr->GetObjectId();
                chr->SetZone(nullptr);
            }
            if (this) {
                this->RemoveCharacter(oid);
            }
            new_zone->SpawnCharacter(chr);
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

std::pair<int32_t, Location> Zone::GetNextMapPortal() const
{
    return std::make_pair(next_map_, next_map_portal_);
}

void Zone::SetNextMapPortalInfo(int32_t mapid, float range, Location location)
{
    next_map_ = mapid;
    next_map_portal_range_ = range;
    next_map_portal_ = location;
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
