#pragma once
#include "../PawnObject.hpp"

class Monster : public PawnObject
{
    friend class MonsterTemplateProvider;
public:
    Monster(const Monster& rhs);
    void operator=(const Monster& rhs);
private:
    Monster();
public:

};