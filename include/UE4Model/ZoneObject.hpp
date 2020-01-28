#pragma once
#include "UE4Model.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "FVector.hpp"
#include <cstdint>


class UE4MODEL_DLLCLASS ZoneObject : public SerializeInterface
{
public:
    using oid_t = int64_t;
    using template_id_t = int32_t;
    enum class Type : int32_t {
        kCharacter = 0,
        kMonster,
        kNpc,
        kReactor,
        kCount,
    };
public:
    ZoneObject(const ZoneObject& rhs);
    void operator=(const ZoneObject& rhs);
    ZoneObject(ZoneObject&& rhs) noexcept;
    void operator=(ZoneObject&& rhs) noexcept;
public:
    ZoneObject(ZoneObject::Type template_id, oid_t object_id = -1);
    virtual ~ZoneObject();

    ZoneObject::Type GetType() const;
    template_id_t GetTemplateId() const;
    void SetTemplateId(template_id_t id);
    oid_t GetObjectId() const;
    void SetObjectId(oid_t oid);
    Location& GetLocation();
    Rotation& GetRotation();
public:
    virtual void Write(OutputStream& output) const override;
    virtual void Read(InputStream& input) override;
private:
    int32_t type_;
    template_id_t template_id_;
    oid_t object_id_;
    Location location_;
    Rotation rotation_;
};