#pragma once
#include <cstdint>

enum class IntermediateServerSendPacket : int16_t
{
    kNotifySessionAuthorityInfo,
    kReactUserMigation,
};

enum class IntermediateServerReceivePacket : int16_t
{
    kRegisterRemoteServer,
    kUpdateServerConnection,
    kRequestUserMigration,
    kReactSessionAuthorityInfo,
    kNotifyUserLogout,
};

enum class ServerType : int16_t
{
    kLoginServer,
    kLobbyServer,
    kZoneServer,
    kCount,
};