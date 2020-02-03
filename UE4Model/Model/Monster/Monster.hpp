#pragma once
#include "../PawnObject.hpp"
#include <mutex>


class Monster : public PawnObject
{
public:
    Monster(const Monster& rhs);
    void operator=(const Monster& rhs);
    Monster();

    bool IsDead() const;
    int32_t GetState() const;
    void SetState(int32_t value);

    std::recursive_mutex mutex_;
private:
    
    bool is_dead_;
    int32_t state_;
};