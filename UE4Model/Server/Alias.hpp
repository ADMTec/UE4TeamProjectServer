#pragma once
#include "UE4DevelopmentLibrary/Server/UE4EventHandler.hpp"
#include "UE4DevelopmentLibrary/Server/UE4IntermediateHandler.hpp"
#include "UE4DevelopmentLibrary/Server/UE4OutPacket.hpp"
#include "UE4DevelopmentLibrary/Server/UE4PacketCipher.hpp"
#include "UE4DevelopmentLibrary/Server/RemoteSessionInfo.hpp"
#include "UE4DevelopmentLibrary/Server/RemoteServerInfo.hpp"
#include "ZoneClient.hpp"


template<typename session>
struct ServerTemplateParameter
{
    using InPacket = NioInPacket;
    using Buffer = NioBuffer<InPacket>;
    using OutPacket = UE4OutPacket;

    using Cipher = UE4PacketCipher<session, Buffer, InPacket, OutPacket>;
    using Handler = UE4EventHandler<session, InPacket, ZoneServer>;

    constexpr static bool Strand = true;
    constexpr static int InternalBufferLength = 1024;
    constexpr static int RecvBufferLength = 2048;
};

using Session = TNioSession<ServerTemplateParameter>;
using IoServer = TNioServer<Session>;
using Client = ZoneClient<Session>;


template<typename Session>
struct IntermediateTemplateParameter
{
    using InPacket = NioInPacket;
    using Buffer = NioBuffer<InPacket>;
    using OutPacket = UE4OutPacket;

    using Cipher = UE4PacketCipher<Session, Buffer, InPacket, OutPacket>;
    using Handler = UE4IntermediateHandler<Session, InPacket>;

    constexpr static bool Strand = true;
    constexpr static int InternalBufferLength = 1024;
    constexpr static int RecvBufferLength = 1024;
};
using IntermediateSession = TNioSession<IntermediateTemplateParameter>;