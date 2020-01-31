#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"


class MatchingSystem : public TSingleton<MatchingSystem>
{
    friend class TSingleton<MatchingSystem>;
    MatchingSystem();
public:

private:
};