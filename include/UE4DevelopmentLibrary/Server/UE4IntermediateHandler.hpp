#pragma once
#include <cstdint>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <functional>


template<class Session, class InPacket>
class UE4IntermediateHandler
{
public:
    static void OnActive(Session& session) {
        std::cout << "Intermediate Channel Active!\n";
    }
    static void OnError(int ec, const char* message) {
        std::cout << "Intermediate Channel Error [" << ec << ":" << message << "]" <<  std::endl;
    }
    static void OnClose(Session& session) {
        std::cout << "Intermediate Channel Closed\n";
    }
    static void ProcessPacket(Session& session, const std::shared_ptr<InPacket>& in_packet) {
        in_packet->SetAccesOffset(2);
        try {
            auto iter = binded_.find(in_packet->ReadInt16());
            if (iter != binded_.end()) {
                iter->second(session, *in_packet);
            }
        } catch (const std::exception & e) {
            std::cout << e.what() << std::endl;
        }
    }

    static void BindFunction(uint16_t opcode, const std::function<void(Session&, InPacket&)>& func)
    {
        binded_[opcode] = func;
    }
private:
    static std::unordered_map<uint16_t, std::function<void(Session&, InPacket&)>> binded_;
};


template<class Session, class InPacket>
std::unordered_map<uint16_t, std::function<void(Session&, InPacket&)>> UE4IntermediateHandler<Session, InPacket>::binded_;