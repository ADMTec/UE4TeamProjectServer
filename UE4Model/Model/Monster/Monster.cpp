#include "Monster.hpp"


Monster::Monster(const Monster& rhs)
    : PawnObject(rhs)
{
    is_dead_ = rhs.is_dead_;
    state_ = rhs.state_;
}

void Monster::operator=(const Monster& rhs)
{
    PawnObject::operator=(rhs);
    is_dead_ = rhs.is_dead_;
    state_ = rhs.state_;
}

Monster::Monster()
    : PawnObject(ZoneObject::Type::kMonster), is_dead_(false)
{

}

bool Monster::IsDead() const
{
    return is_dead_;
}

int32_t Monster::GetState() const
{
    return state_;
}

void Monster::SetState(int32_t value)
{
    state_ = value;
}
