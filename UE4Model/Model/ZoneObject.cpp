#include "ZoneObject.hpp"


ZoneObject::ZoneObject(const ZoneObject& rhs)
{
    ZoneObject::operator=(rhs);
}

void ZoneObject::operator=(const ZoneObject& rhs)
{
    type_ = rhs.type_;
    template_id_ = rhs.template_id_;
    object_id_ = rhs.object_id_;
    location_ = rhs.location_;
    rotation_ = rhs.rotation_;
}

ZoneObject::ZoneObject(ZoneObject&& rhs) noexcept
{
    ZoneObject::operator=(std::move(rhs));
}

void ZoneObject::operator=(ZoneObject&& rhs) noexcept
{
    type_ = rhs.type_;
    template_id_ = rhs.template_id_; rhs.template_id_ = -1;
    object_id_ = rhs.object_id_; rhs.object_id_ = -1;
    location_ = std::move(rhs.location_);
    rotation_ = std::move(rhs.rotation_);
}

ZoneObject::ZoneObject(ZoneObject::Type template_id, oid_t object_id)
    : type_(static_cast<int32_t>(template_id)), object_id_(object_id)
{
}

ZoneObject::~ZoneObject()
{
}

ZoneObject::oid_t ZoneObject::GetObjectId() const
{
    return object_id_;
}

void ZoneObject::SetObjectId(oid_t oid)
{
    object_id_ = oid;
}

Location& ZoneObject::GetLocation()
{
    return location_;
}

const Location& ZoneObject::GetLocation() const
{
    return location_;
}

Rotation& ZoneObject::GetRotation()
{
    return rotation_;
}

const Rotation& ZoneObject::GetRotation() const
{
    return rotation_;
}

ZoneObject::Type ZoneObject::GetType() const
{
    return static_cast<ZoneObject::Type>(type_);
}

ZoneObject::template_id_t ZoneObject::GetTemplateId() const
{
    return template_id_;
}

void ZoneObject::SetTemplateId(template_id_t id)
{
    template_id_ = id;
}

void ZoneObject::Write(OutputStream& output) const
{
    output << type_;
    output << template_id_;
    output << object_id_;
    output << location_;
    output << rotation_;
}

void ZoneObject::Read(InputStream& input)
{
    input >> type_;
    input >> template_id_;
    input >> object_id_;
    input >> location_;
    input >> rotation_;
}
