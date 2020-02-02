#include "Monster.hpp"


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
