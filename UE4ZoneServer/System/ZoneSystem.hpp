#pragma once
#include "UE4DevelopmentLibrary/Stream.hpp"
#include "UE4DevelopmentLibrary/Utility/UUID.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>
#include <memory>
#include <atomic>
#include <array>
#include "UE4Model/Character/Character.hpp"
#include "Pawn/Monster/Monster.hpp"


class Character;

using std::unordered_map;
using std::shared_mutex;
using std::shared_ptr;
using std::unique_ptr;
using std::array;

class Zone
{
    enum class Type : int32_t {
        kTown,
        kDungeon,
        kCount,
    };
public:
    class System;
public:
    Zone(const Zone&) = delete;
    void operator=(const Zone&) = delete;
private:
    Zone();
public:
    void SpawnPlayer(const std::shared_ptr<class Character>& chr);
    bool SpawnMonster(int32_t mobid, Location location, Rotation rotation);
    void SpawnMonster(Monster& mob, Location location, Rotation rotation);
    void SpawnNpc(int32_t npcid, Location location, Rotation rotation);
    void AttackMonster(const Character& chr, int64_t mobid);

    void BroadCast(class NioOutPacket& outpacket);
    void BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid);

    int64_t GetInstanceId() const;
    int32_t GetZoneId() const;
    Zone::Type GetType() const;
private:
    int64_t GetNewObjectId();
private:
    int64_t instance_id_;
    int32_t zone_id;
    Zone::Type type_;
    Location player_spawn_;
    std::atomic<int64_t> last_object_id_;
    mutable array<shared_mutex, ZoneObject::Type::kCount> object_guard_;
    unordered_map<ZoneObject::oid_t, shared_ptr<Character>> chrs_;
    unordered_map<ZoneObject::oid_t, Monster> mobs_;
};

class Zone::System {
public:
    static void Initialize();
    // return zone key
    static shared_ptr<Zone> CreateNewDungeon(int32_t dungeon_id);
    static void DestroyDungoen(int64_t instance_id);
    static shared_ptr<Zone> GetTown(int64_t town_id);
    static shared_ptr<Zone> GetDungeon(int64_t instance_id);
private:
    static Zone* CreateNewZone(int32_t zone_id);
private:
    static array<shared_mutex, static_cast<int16_t>(Zone::Type::kCount)> zone_guard_;
    static unordered_map<int64_t, shared_ptr<Zone>> town_;
    static unordered_map<int64_t, shared_ptr<Zone>> dungeon_;
};