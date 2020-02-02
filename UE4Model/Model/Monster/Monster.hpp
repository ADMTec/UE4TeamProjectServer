#pragma once
#include "../PawnObject.hpp"


class Monster : public PawnObject
{
public:
    Monster();

    bool IsDead() const;
    int32_t GetState() const;
    void SetState(int32_t value);
private:
    bool is_dead_;
    int32_t state_;
};