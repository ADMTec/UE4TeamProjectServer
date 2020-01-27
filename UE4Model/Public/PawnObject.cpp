#include "PawnObject.hpp"

PawnObject::PawnObject(const PawnObject& rhs)
    : ZoneObject(rhs)
{
    hp_ = rhs.hp_;
    max_hp_ = rhs.max_hp_;
    attack_min_ = rhs.attack_min_;
    attack_max_ = rhs.attack_max_;
    attack_range_ = rhs.attack_range_;
    attack_speed_ = rhs.attack_speed_;
    defence_ = rhs.defence_;
    speed_ = rhs.speed_;
}

void PawnObject::operator=(const PawnObject& rhs)
{
    ZoneObject::operator=(rhs);
    hp_ = rhs.hp_;
    max_hp_ = rhs.max_hp_;
    attack_min_ = rhs.attack_min_;
    attack_max_ = rhs.attack_max_;
    attack_range_ = rhs.attack_range_;
    attack_speed_ = rhs.attack_speed_;
    defence_ = rhs.defence_;
    speed_ = rhs.speed_;
}

PawnObject::PawnObject(ZoneObject::Type type, oid_t object_id)
    : ZoneObject(type, object_id)
{
}

PawnObject::~PawnObject()
{
}

float PawnObject::GetHP() const
{
    return hp_;
}

float PawnObject::GetMaxHP() const
{
    return max_hp_;
}

float PawnObject::GetAttackMin() const
{
    return attack_min_;
}

float PawnObject::GetAttackMax() const
{
    return attack_max_;
}

float PawnObject::GetAttackRange() const
{
    return attack_range_;
}

float PawnObject::GetAttackSpeed() const
{
    return attack_speed_;
}

float PawnObject::GetDefence() const
{
    return defence_;
}

float PawnObject::GetSpeed() const
{
    return speed_;
}

void PawnObject::SetHP(float value)
{
    hp_ = value;
}

void PawnObject::SetMaxHP(float value)
{
    max_hp_ = value;
}

void PawnObject::SetAttackMin(float value)
{
    attack_min_ = value;
}

void PawnObject::SetAttackMax(float value)
{
    attack_max_ = value;
}

void PawnObject::SetAttackRange(float value)
{
    attack_range_ = value;
}

void PawnObject::SetAttackSpeed(float value)
{
    attack_speed_ = value;
}

void PawnObject::SetDefence(float value)
{
    defence_ = value;
}

void PawnObject::SetSpeed(float value)
{
    speed_ = value;
}

void PawnObject::Write(OutputStream& output) const
{
    output << hp_;
    output << max_hp_;
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
    input >> max_hp_;
    input >> attack_min_;
    input >> attack_max_;
    input >> attack_range_;
    input >> attack_speed_;
    input >> defence_;
    input >> speed_;
}