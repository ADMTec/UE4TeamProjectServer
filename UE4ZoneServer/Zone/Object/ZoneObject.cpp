#include "ZoneObject.hpp"

ZoneObject::ZoneObject(const ZoneObject& rhs)
{
    ZoneObject::operator=(rhs);
}

void ZoneObject::operator=(const ZoneObject& rhs)
{
    template_ = rhs.template_;
    object_id_ = rhs.object_id_;
    location_ = rhs.location_;
}

ZoneObject::ZoneObject(ZoneObject&& rhs) noexcept
{
    ZoneObject::operator=(std::move(rhs));
}

void ZoneObject::operator=(ZoneObject&& rhs) noexcept
{
    template_ = rhs.template_;
    object_id_ = rhs.object_id_; rhs.object_id_ = -1;
    location_ = rhs.location_;
}

ZoneObject::ZoneObject(ZoneObject::Template template_id, oid_t object_id)
    : template_(static_cast<int32_t>(template_id)), object_id_(object_id)
{
}

ZoneObject::~ZoneObject()
{
}

void ZoneObject::Initialize()
{
}

void ZoneObject::Update(const Clock& clock)
{
}

void ZoneObject::Release()
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

Rotation& ZoneObject::GetRotation()
{
    return rotation_;
}

ZoneObject::Template ZoneObject::GetTemplate() const
{
    return static_cast<ZoneObject::Template>(template_);
}

void ZoneObject::Write(OutputStream& output) const
{
    output << template_;
    output << object_id_;
    output << location_;
    output << rotation_;
}

void ZoneObject::Read(InputStream& input)
{
    input >> template_;
    input >> object_id_;
    input >> location_;
    input >> rotation_;
}
