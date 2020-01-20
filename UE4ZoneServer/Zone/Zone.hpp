#pragma once
#include "Object/ZoneObject.hpp"
#include <shared_mutex>
#include <unordered_map>
#include <array>

class Zone
{
public:
    using zone_object_map_t = std::unordered_map<ZoneObject::oid_t, std::shared_ptr<ZoneObject>>;
public:
    Zone(const Zone&) = delete;
    void operator=(const Zone&) = delete;
    Zone(Zone&&) noexcept = delete;
    void operator=(Zone&&) noexcept = delete;
public:
    Zone(int64_t mapid);

    virtual void Initialize();
    virtual void Update();
    virtual void Release();

    std::shared_ptr<ZoneObject> Find(ZoneObject::Template tmp, ZoneObject::oid_t oid) const;
    size_t Remove(ZoneObject::Template tmp, ZoneObject::oid_t oid);
    std::vector<std::shared_ptr<ZoneObject>> GetCopy(ZoneObject::Template tmp) const;
private:
    std::array<std::shared_mutex, Enum::Cast(ZoneObject::Template::Count)> object_map_guard_;
    std::array<zone_object_map_t, Enum::Cast(ZoneObject::Template::Count)> object_;
};