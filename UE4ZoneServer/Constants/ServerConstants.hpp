#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"


struct ServerConstants : public TSingleton<const ServerConstants>
{
private:
    friend class TSingleton<const ServerConstants>;
    ServerConstants() = default;
public:
    void Initialize();

public:
    const char* data_root_dir = "Data";
    const char* map_data_root = "Map";
};

#define ServerConstant ServerConstants::Instance()