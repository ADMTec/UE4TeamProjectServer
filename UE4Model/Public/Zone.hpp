#pragma once
#include "UE4Model.hpp"
#include "Monster/Monster.hpp"


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
    class System;
private:
    Zone(const Zone&);
    void operator=(const Zone&);
public:
    Zone();
    explicit Zone(ZoneImpl* impl);
    ~Zone();

    void SpawnPlayer(const std::shared_ptr<class Character>& chr);
    void AddMonster(Monster& mob, Location location, Rotation rotation);
    void SpawnMonster(Monster& mob, Location location, Rotation rotation);
    void AddNPC(int32_t npc, Location location, Rotation rotation);
    void SpawnNpc(int32_t npc, Location location, Rotation rotation);
    void AttackMonster(const Character& chr, int64_t mob_obj_id);

    void BroadCast(class NioOutPacket& outpacket);
    void BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid);

    int64_t GetInstanceId() const;
    void SetInstanceId(int64_t id);
    int32_t GetMapId() const;
    void SetMapId(int32_t map_id);
    Zone::Type GetType() const;
    void SetType(Zone::Type type);
    void SetPlayerSpawn(Location spawn);
private:
    int64_t GetNewObjectId();
private:
    ZoneImpl* impl_;
};