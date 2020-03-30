#pragma once
#include "Nio/NioCipher.hpp"
#include "Nio/NioEventHandler.hpp"
#include "Nio/NioBuffer.hpp"
#include "Nio/NioInPacket.hpp"
#include "Nio/NioOutPacket.hpp"
#include "Nio/NioSession.hpp"
#include "Nio/NioServer.hpp"


template<typename Session>
struct TemplateParameter
{
    using InPacket = NioInPacket;
    using Buffer = NioBuffer<InPacket>;
    using OutPacket = NioOutPacket;

    using Cipher = NioCipher<Session, Buffer, InPacket, OutPacket>;
    using Handler = NioEventHandler<Session, InPacket>;

    constexpr static bool Strand = true;
    constexpr static int InternalBufferLength = 2048;
    constexpr static int RecvBufferLength = 2048;
};

using ExampleNioSessin = TNioSession<TemplateParameter>;
using ExampleNioServer = TNioServer<ExampleNioSessin>;