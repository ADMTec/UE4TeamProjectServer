#pragma once
#include "../PawnObject.hpp"
#include <mutex>
#include <string>
#include <cstdint>


class Monster : public PawnObject
{
public:
    enum class State : int32_t {
        kSpawn,
        kAlive,
        kDead,
    };
public:
    Monster(const Monster& rhs);
    void operator=(const Monster& rhs);
    Monster();
    std::string GetDebugString() const;

    int32_t GetState() const;
    void SetState(int32_t value);

    std::recursive_mutex mutex_;
private:
    int32_t state_;
};