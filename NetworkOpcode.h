#pragma once
#include <cstdint>

enum class ENetworkCSOpcode : uint16_t
{
    // LoginServer
    kCreateAccountRequest,
    kLoginRequest,

    // LobbyServer
    kCharacterListRequest,
    kCharacterCreateRequest,
    kCharacterDeleteRequest,
    kCharacterSelectionRequest,

    // ZoneServer
};


enum class ENetworkSCOpcode : uint16_t
{
    // LoginServer
    kCreateAccountResult,
    kLoginResult,

    // LobbyServer
    kCharacterListNotify,
    kMigrateZoneNotify,

    // ZoneServer
};