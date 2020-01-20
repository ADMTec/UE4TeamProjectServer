#include "NioEventHandler.hpp"
#include "NioSession.hpp"
#include "NioInternalBuffer.hpp"
#include "NioInPacket.hpp"
#include <iostream>
#include <vector>


NioEventHandler::NioEventHandler()
{
}

NioEventHandler::~NioEventHandler()
{
}

void NioEventHandler::OnSessionActive(NioSession& session)
{
    std::cout << "sessionid: " << session.GetId() << " - Active" << std::endl;
}

void NioEventHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{
    std::cout << "sessionid: " << session.GetId()
        << ", error_code: " << error_code
        << ", message: " << message << std::endl;
}

void NioEventHandler::OnSessionClose(NioSession& session)
{
}

void NioEventHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    try
    {
        std::cout << "sessionid: " << session.GetId() 
            << " packet: " << in_packet->GetDebugString() << std::endl;
    }
    catch (std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
}