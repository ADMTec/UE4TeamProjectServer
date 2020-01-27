#pragma once
#include "../Nio.hpp"

template<typename UE4Server>
class UE4EventHandler : public NioEventHandler
{
public:
    virtual void OnSessionActive(NioSession& session) override {
        UE4Server::Instance().ActiveClient(session);
    }
    virtual void OnSessionError(NioSession& session, int error_code, const char* message) override {
        // later...
    }
    virtual void OnSessionClose(NioSession& session) override {
        UE4Server::Instance().CloseClient(session.GetClientKey());
    }
    virtual void ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet) override {
        UE4Server::Instance().ProcessPacket(session, in_packet);
    }
};