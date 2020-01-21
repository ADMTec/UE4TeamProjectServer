#pragma once
#include "Object/ZoneObject.hpp"
#include <shared_mutex>
#include <unordered_map>
#include <array>
#include <thread>
#include <atomic>

class InputStream;
using std::unordered_map;
using std::shared_ptr;

class Zone
{
public:
    using zone_object_map_t = unordered_map<ZoneObject::oid_t, shared_ptr<ZoneObject>>;
public:
    Zone(const Zone&) = delete;
    void operator=(const Zone&) = delete;
    Zone(Zone&&) noexcept = delete;
    void operator=(Zone&&) noexcept = delete;
public:
    Zone(int64_t mapid);
    void Initialize(const std::vector<shared_ptr<ZoneObject>> Characters);
    void Run();
    void Stop();
    void Exit();

    void AddPlayerInput(ZoneObject::oid_t oid, const shared_ptr<InputStream>& input);

    void SpawnPlayerCharacter();
    shared_ptr<ZoneObject> LeavePlayerCharacter();
private:
    void UpdatePlayerInput();
    void SwapInputVector();
    void SimulateZone();
private:
    std::atomic<bool> exit_flag_;

    int input_vec_index_;
    std::mutex input_vector_index_guard_;
    std::vector<std::pair<ZoneObject::oid_t, std::shared_ptr<InputStream>>> input_vec[2];

    std::array<std::shared_mutex, ZoneObject::Template::kCount> object_guard_;
    unordered_map<ZoneObject::oid_t, shared_ptr<ZoneObject>> Characters;
    //unordered_map<ZoneObject::oid_t, shared_ptr<ZoneObject>> Monsters;
    //unordered_map<ZoneObject::oid_t, shared_ptr<ZoneObject>> Npcs;
    //unordered_map<ZoneObject::oid_t, shared_ptr<ZoneObject>> Reactors;
};