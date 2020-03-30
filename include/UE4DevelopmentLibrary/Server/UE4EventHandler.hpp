#pragma once
#include <memory>

template<class Session, class InPacket, class Server>
class UE4EventHandler
{
public:
    static void OnActive(Session& session) {
        Server::Instance().OnActive(session);
    }
    static void OnError(int ec, const char* message) {
        Server::Instance().OnError(ec, message);
    }
    static void OnClose(Session& session) {
        Server::Instance().OnClose(session);
    }
    static void ProcessPacket(Session& session, const std::shared_ptr<InPacket>& in_packet) {
        Server::Instance().ProcessPacket(session, in_packet);
    }
};
