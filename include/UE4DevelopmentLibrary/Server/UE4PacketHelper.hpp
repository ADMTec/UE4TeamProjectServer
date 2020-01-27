#pragma once
#include "RemoteSessionInfo.hpp"


class NioOutPacket;
class InputStream;

class UE4PacketHelper
{
public:
    static void WriteUserMigrationInfo(NioOutPacket& out, int16_t server_type, RemoteSessionInfo& session_info);
    static void ReadUserMigrationInfo(InputStream& input, int16_t* server_type, RemoteSessionInfo* session_info);
};