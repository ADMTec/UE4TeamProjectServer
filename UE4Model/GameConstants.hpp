#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include <optional>
#include <array>

struct GameConstants : public TSingleton<const GameConstants>
{
private:
    friend class TSingleton<const GameConstants>;
    GameConstants() = default;
public:
    void Initialize();
public:

public:

};

#define GameConstant GameConstants::Instance()