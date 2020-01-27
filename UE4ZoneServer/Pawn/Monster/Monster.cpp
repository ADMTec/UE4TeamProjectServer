#include "Monster.hpp"



Monster::Monster(const Monster& rhs)
    : PawnObject(rhs)
{
}

void Monster::operator=(const Monster& rhs)
{
    PawnObject::operator=(rhs);
}

Monster::Monster()
    : PawnObject(Zone::Object::Type::kMonster)
{
}