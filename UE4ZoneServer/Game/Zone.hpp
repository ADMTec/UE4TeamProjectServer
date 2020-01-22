#pragma once
#include "ZoneObject.hpp"
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
    // ObjectId 할당 추가.
    friend class ZoneFactory;
public:
    using zone_object_map_t = unordered_map<ZoneObject::oid_t, shared_ptr<ZoneObject>>;
public:
    Zone(const Zone&) = delete;
    void operator=(const Zone&) = delete;
    Zone(Zone&&) noexcept = delete;
    void operator=(Zone&&) noexcept = delete;
private:
    Zone();
public:
    void SpawnPlayer(const std::shared_ptr<class Character>& chr);
    void AttackMonster(const Character& chr, int64_t mobid);
public:
    void BroadCast(class NioOutPacket& outpacket);
    void BroadCast(class NioOutPacket& outpacket, int64_t except_chr_oid);
private:
    std::shared_mutex chr_map_guard_;
    std::unordered_map<int64_t, std::shared_ptr<class Character>> chr_map_;
};