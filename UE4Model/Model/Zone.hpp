#pragma once
#include "EnumClassHelper.hpp"
#include "UE4DevelopmentLibrary/Stream.hpp"
#include "Character/Character.hpp"
#include "Monster/Monster.hpp"
#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <array>
#include <functional>


class Zone
{
public:
    enum class Type : int32_t {
        kTown,
        kDungeon,
        kCount,
    };
    enum class State : int32_t {
        kPreparing,
        kActive,
    };
    friend class ZoneSystemImpl;
public:
    Zone();

    void StartUp();
    void Update();
    void Exit();

    void RegisterTimer(const std::function<void(void)> func, int64_t deferred_time_milli);

    
    void AddMonster(Monster& mob, Location location, Rotation rotation);
    void AddNPC(int32_t npc, Location location, Rotation rotation);


    void UpdateCharacterPosition(Character& chr, int32_t value);

    void SpawnCharacter(const std::shared_ptr<class Character>& chr);
    void RemoveCharacter(int64_t object_id);
    void UpdateCharacterPosition(const std::shared_ptr<Character>& chr, int state);

    void SpawnMonster(Monster& mob, Location location, Rotation rotation);
    void SpawnNPC(int32_t npc, Location location, Rotation rotation);

    void AttackMonster(const Character& chr, int64_t mob_obj_id);

    void BroadCast(class NioOutPacket& outpacket);
    void BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid);

    Zone::State GetState() const;
    void SetState(Zone::State state);
    int64_t GetInstanceId() const;
    void SetInstanceId(int64_t id);
    int32_t GetMapId() const;
    void SetMapId(int32_t map_id);
    Zone::Type GetType() const;
    void SetType(Zone::Type type);
    Location GetPlayerSpawn() const;
    void SetPlayerSpawn(Location location);

    std::vector<std::shared_ptr<Character>> GetThreadSafeCharacterCopy() const;
    std::vector<std::shared_ptr<Character>> GetCharacterCopy() const;
    std::vector<Monster> GetThreadSafeMonsterCopy() const;
    std::vector<Monster> GetMonsterCopy() const;
public:
    int64_t GetNewObjectId();
private:
    void BroadCastNoLock(class NioOutPacket& outpacket);
    void BroadCastNoLock(class NioOutPacket& outpacket, int64_t except_chr_oid);
private:
    std::atomic<Zone::State> state_;
    int64_t instance_id_;
    int32_t map_id_;
    Zone::Type type_;
    Location player_spawn_;
    std::atomic<int64_t> last_object_id_;
    int64_t time_sum_;
    int64_t last_update_time_;

    mutable std::array<std::shared_mutex, ToInt32(ZoneObject::Type::kCount)> object_guard_;
    std::unordered_map<ZoneObject::oid_t, std::shared_ptr<Character>> chrs_;
    std::unordered_map<ZoneObject::oid_t, Monster> mobs_;
};