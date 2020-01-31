#pragma once
#include <iostream>

template<class T, class InPacket>
class NioEventHandler
{
public:
    static void OnActive(T& session) {
        std::cout << "sessionid: " << session.GetId() << " - Active" << std::endl;
    }
    static void OnError(int error_code, const char* message) {
        std::cout << "sessionid: " << session.GetId()
            << ", error_code: " << error_code
            << ", message: " << message << std::endl;
    }
    static void OnClose(T& session) {

    }
    static void ProcessPacket(T& session, const std::shared_ptr<InPacket>& in_packet) {
        try
        {
            std::cout << "sessionid: " << session.GetId()
                << " packet: " << in_packet->GetDebugString() << std::endl;
        } catch (std::exception & e)
        {
            std::cout << e.what() << std::endl;
        }
    }
};