#include "LobbyEventHandler.hpp"
#include "LobbyServer.hpp"


void LobbyEventHandler::OnSessionActive(NioSession& session)
{
    LobbyServer::Instance().ActiveClient(session);
}

void LobbyEventHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{
    // later...
}

void LobbyEventHandler::OnSessionClose(NioSession& session)
{
    LobbyServer::Instance().CloseClient(session.GetClientKey());
}

void LobbyEventHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    in_packet->SetAccesOffset(2);
    LobbyServer::Instance().ProcessPacket(session, in_packet);
}