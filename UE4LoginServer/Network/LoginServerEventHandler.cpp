#include "LoginServerEventHandler.hpp"

void LoginServerEventHandler::OnSessionActive(NioSession& session)
{
}

void LoginServerEventHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{
}

void LoginServerEventHandler::OnSessionClose(NioSession& session)
{
}

void LoginServerEventHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
}
