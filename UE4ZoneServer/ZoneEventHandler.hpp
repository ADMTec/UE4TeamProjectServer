#pragma once
#include "UE4DevelopmentLibrary/Nio.hpp"

class ZoneServerEventHandler : public NioEventHandler
{
public:
    virtual void OnSessionActive(NioSession& session);
    virtual void OnSessionError(NioSession& session, int error_code, const char* message);
    virtual void OnSessionClose(NioSession& session);
    virtual void ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet);
};