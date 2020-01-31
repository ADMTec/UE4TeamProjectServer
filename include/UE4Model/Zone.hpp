#pragma once
#include "UE4Model.hpp"
#include "Monster/Monster.hpp"
#include "Character/Character.hpp"


UE4MODEL_DLLEXPORT template class UE4MODEL_DLLCLASS std::shared_ptr<Character>;

class Character;
class ZoneImpl;
class ZoneSystemImpl;

class UE4MODEL_DLLCLASS Zone
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
    class System;
private:
    Zone(const Zone&);
    void operator=(const Zone&);
public:
    Zone();
    explicit Zone(ZoneImpl* impl);
    ~Zone();

    void StartUp();
    void Update();
    void Exit();

    void UpdateCharacterPosition(Character& chr, int32_t value);

    void SpawnPlayer(const std::shared_ptr<Character>& chr);
    void AddMonster(Monster& mob, Location location, Rotation rotation);
    void SpawnMonster(Monster& mob, Location location, Rotation rotation);
    void AddNPC(int32_t npc, Location location, Rotation rotation);
    void SpawnNpc(int32_t npc, Location location, Rotation rotation);
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
    void SetPlayerSpawn(Location spawn);
private:
    int64_t GetNewObjectId();
private:
    ZoneImpl* impl_;
};