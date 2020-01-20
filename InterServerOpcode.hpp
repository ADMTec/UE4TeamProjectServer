#pragma once
#include <cstdint>

enum class ToLoginServer : int16_t
{
    UserCloseNotify,
    UserMigrationNotifyAck,
};

enum class OutLoginServer : int16_t
{
    UserCloseNotify,
    UserCloseAck,
    UserMigrationNotifyAck
};


enum class ToLobbyServer : int16_t
{
    UserMigrationNotify,
    UserMigrationNotifyAck
};

enum class ToZoneServer : int16_t
{
    UserMigrationNotify,
};