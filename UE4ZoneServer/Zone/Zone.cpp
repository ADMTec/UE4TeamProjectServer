#include "Zone.hpp"

void Zone::Initialize()
{
}

void Zone::Update()
{
    while (true)
    {
        Clock clock;
        for (int32_t i = 0; i < Enum::Cast(ZoneObject::Template::Count); ++i)
        {
            std::shared_lock lock(object_map_guard_[i]);
            for (const auto& pair : object_[i]) {
                pair.second->Update(clock);
            }
        }
    }
}

void Zone::Release()
{
}

std::shared_ptr<ZoneObject> Zone::Find(ZoneObject::Template tmp, ZoneObject::oid_t oid) const
{
    std::shared_lock lock(object_map_guard_[Enum::Cast(tmp)]);
    return object_[Enum::Cast(tmp)].at(oid);
}

size_t Zone::Remove(ZoneObject::Template tmp, ZoneObject::oid_t oid)
{
    std::unique_lock lock(object_map_guard_[Enum::Cast(tmp)]);
    return object_[Enum::Cast(tmp)].erase(oid);
}

std::vector<std::shared_ptr<ZoneObject>> Zone::GetCopy(ZoneObject::Template tmp) const
{
    std::vector<std::shared_ptr<ZoneObject>> vec;

    std::shared_lock lock(object_map_guard_[Enum::Cast(tmp)]);
    vec.reserve(object_[Enum::Cast(tmp)].size());
    for (const auto& pair : object_[Enum::Cast(tmp)]) {
        vec.emplace_back(pair.second);
    }
    return vec;
}