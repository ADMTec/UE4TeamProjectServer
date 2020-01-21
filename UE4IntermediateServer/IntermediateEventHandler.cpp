#include "IntermediateEventHandler.hpp"
#include "IntermediateServer.hpp"


void IntermediateEventHandler::OnSessionActive(NioSession& session)
{
    IntermediateServer::Instance().ActiveClient(session);
}

void IntermediateEventHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{

}

void IntermediateEventHandler::OnSessionClose(NioSession& session)
{
    IntermediateServer::Instance().CloseClient(session.GetClientKey());
}

void IntermediateEventHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    in_packet->SetAccesOffset(2);
    IntermediateServer::Instance().ProcessPacket(session, in_packet);
}