#include "ZoneEventHandler.hpp"
#include "ZoneServer.hpp"


void ZoneServerEventHandler::OnSessionActive(NioSession& session)
{
    ZoneServer::Instance().ActiveClient(session);
}

void ZoneServerEventHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{
    // later...
}

void ZoneServerEventHandler::OnSessionClose(NioSession& session)
{
    ZoneServer::Instance().CloseClient(session.GetClientKey());
}

void ZoneServerEventHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    ZoneServer::Instance().ProcessPacket(session, in_packet);
}
