#include "PawnObject.hpp"

PawnObject::PawnObject(ZoneObject::Template template_id, oid_t object_id)
{
}

PawnObject::~PawnObject()
{
}

void PawnObject::Write(OutputStream& output) const
{
    output << hp_;
    output << max_hp;
    output << attack_min_;
    output << attack_max_;
    output << attack_range_;
    output << attack_speed_;
    output << defence_;
    output << speed_;
}

void PawnObject::Read(InputStream& input)
{
    input >> hp_;
    input >> max_hp;
    input >> attack_min_;
    input >> attack_max_;
    input >> attack_range_;
    input >> attack_speed_;
    input >> defence_;
    input >> speed_;
}