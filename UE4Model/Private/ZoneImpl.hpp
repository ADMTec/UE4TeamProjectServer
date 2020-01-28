#pragma once
#include "Public/Zone.hpp"
#include "Public/Character/Character.hpp"
#include "Public/Monster/Monster.hpp"
#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <array>


class ZoneImpl
{
    friend class ZoneSystemImpl;
public:
    void AddMonster(Monster& mob, Location location, Rotation rotation);
    void AddNPC(int32_t npc, Location location, Rotation rotation);

    void SpawnPlayer(const std::shared_ptr<class Character>& chr);
    void SpawnMonster(Monster& mob, Location location, Rotation rotation);
    void SpawnNPC(int32_t npc, Location location, Rotation rotation);
    void AttackMonster(const Character& chr, int64_t mob_obj_id);

    void BroadCast(class NioOutPacket& outpacket);
    void BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid);

    int64_t GetInstanceId() const;
    void SetInstanceId(int64_t id);
    int32_t GetMapId() const;
    void SetMapId(int32_t map_id);
    Zone::Type GetType() const;
    void SetType(Zone::Type type);
    void SetPlayerSpawn(Location location);

    int64_t GetNewObjectId();
private:
    int64_t instance_id_;
    int32_t map_id_;
    Zone::Type type_;
    Location player_spawn_;
    std::atomic<int64_t> last_object_id_;
    mutable std::array<std::shared_mutex, ToInt32(ZoneObject::Type::kCount)> object_guard_;
    std::unordered_map<ZoneObject::oid_t, std::shared_ptr<Character>> chrs_;
    std::unordered_map<ZoneObject::oid_t, Monster> mobs_;
};