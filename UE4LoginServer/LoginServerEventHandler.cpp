#include "LoginServerEventHandler.hpp"
#include "LoginServer.hpp"


void LoginServerEventHandler::OnSessionActive(NioSession& session)
{
    LoginServer::Instance().ActiveClient(session);
}

void LoginServerEventHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{
    // later...
}

void LoginServerEventHandler::OnSessionClose(NioSession& session)
{
    LoginServer::Instance().CloseClient(session.GetClientKey());
}

void LoginServerEventHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    in_packet->SetAccesOffset(2);
    LoginServer::Instance().ProcessPacket(session, in_packet);
}