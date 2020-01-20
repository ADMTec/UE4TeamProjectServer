#pragma once
#include "UE4DevelopmentLibrary/Time.hpp"
#include "Location.hpp"
#include "Rotation.hpp"
#include <cstdint>

class Enum
{
public:
    template<typename T>
    static constexpr int32_t Cast(T value) {
        return static_cast<int32_t>(value);
    }
};

class ZoneObject : public SerializeInterface
{
public:
    using oid_t = int64_t;
    enum class Template : int32_t {
        Character = 0,
        Monster,
        Npc,
        Item,
        Reactor,
        Count,
    };
public:
    ZoneObject(const ZoneObject& rhs);
    void operator=(const ZoneObject& rhs);
    ZoneObject(ZoneObject&& rhs) noexcept;
    void operator=(ZoneObject&& rhs) noexcept;
public:
    ZoneObject(ZoneObject::Template template_id, oid_t object_id = -1);
    virtual ~ZoneObject();

    virtual void Initialize();
    virtual void Update(const Clock& clock);
    virtual void Release();

    ZoneObject::Template GetTemplate() const;
    oid_t GetObjectId() const;
    void SetObjectId(oid_t oid);
    Location& GetLocation();
    Rotation& GetRotation();
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    int32_t template_;
    oid_t object_id_;
    Location location_;
    Rotation rotation_;
};