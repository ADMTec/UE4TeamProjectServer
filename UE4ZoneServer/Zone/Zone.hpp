#pragma once
#include "Monster/Monster.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <array>
#include <thread>
#include <atomic>


class InputStream;
using std::unordered_map;
using std::shared_ptr;

class Zone
{
public:
    enum class Type : int32_t {
        kTown,
        kDungeon,
    };
    using zone_object_map_t = unordered_map<ZoneObject::oid_t, shared_ptr<ZoneObject>>;
public:
    Zone(const Zone&) = delete;
    void operator=(const Zone&) = delete;
    Zone(Zone&&) noexcept = delete;
    void operator=(Zone&&) noexcept = delete;
private:
    Zone();
public:
    static std::shared_ptr<Zone> CreateInstance(int64_t map_id);

    void SpawnPlayer(const std::shared_ptr<class Character>& chr);
    bool SpawnMonster(int32_t mobid, Location location, Rotation rotation);
    void SpawnMonster(Monster& mob, Location location, Rotation rotation);
    void SpawnNpc(int32_t npcid, Location location, Rotation rotation);
    void AttackMonster(const Character& chr, int64_t mobid);

    void BroadCast(class NioOutPacket& outpacket);
    void BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid);
private:
    int64_t GetNewObjectId();
private:
    int64_t map_id_;
    Zone::Type type_;
    Location player_spawn_;

    std::atomic<int64_t> last_object_id_;
    std::shared_mutex character_map_guard_;
    std::unordered_map<int64_t, std::shared_ptr<class Character>> characters_;

    std::shared_mutex monster_map_guard_;
    std::unordered_map<int64_t, Monster> monsters_;


    std::shared_mutex npc_map_gaurd_;
    std::unordered_map<int64_t, int> npcs_;

    std::shared_mutex reactor_map_gaurd_;
    std::unordered_map<int64_t, int> reactor_;
};