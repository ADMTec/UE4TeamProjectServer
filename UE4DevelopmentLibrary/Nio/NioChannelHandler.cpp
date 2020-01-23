#include "NioChannelHandler.hpp"
#include "NioInternalBuffer.hpp"
#include "NioInPacket.hpp"
#include "NioSession.hpp"
#include "Exception.hpp"
#include "../Time.hpp"
#include <shared_mutex>
#include <iostream>
#include <sstream>


int64_t NioChannelCipher::GetHead(NioInternalBuffer& buffer)
{
	if (buffer.GetReceiveLength() < 2) {
		return -1;
	}
	return buffer.ReadInt16();
}

int64_t NioChannelCipher::GetPacketLength(int64_t head)
{
	return head;
}

void NioChannelCipher::Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet)
{
	int64_t head = this->GetHead(buffer);
	int64_t packet_length = GetPacketLength(head);


	if (head < 0 || head < packet_length || (buffer.GetReceiveLength() < packet_length)) {
		buffer.ResetReadOffset();
		return;
	}
	buffer.InitializePacket(packet, packet_length);
	buffer.MarkReadIndex(packet_length);
}

void NioChannelCipher::Encode(NioSession& session, NioOutPacket& out_packet)
{
}

NioChannelEventHandler::NioChannelEventHandler()
{
}

NioChannelEventHandler::~NioChannelEventHandler()
{
}

void NioChannelEventHandler::BindFunction(int16_t identifier, const handler_t& handler)
{
	std::unique_lock lock(binded_function_guard_);
	binded_function_.emplace(identifier, handler);
}

void NioChannelEventHandler::OnSessionActive(NioSession& session)
{
	std::stringstream ss;
	Clock clock;
	ss << '[' << Calendar::DateTime(clock) << "] ";
	ss << "NioChannel Active: [" << session.GetRemoteAddress() << "]\n";
	std::cout << ss.str();
}

void NioChannelEventHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{
	std::stringstream ss;
	Clock clock;
	ss << '[' << Calendar::DateTime(clock) << "] ";
	ss << "NioChannel Error:" << session.GetRemoteAddress() << ", " << error_code << ": " << message << '\n';
	std::cout << ss.str();
}

void NioChannelEventHandler::OnSessionClose(NioSession& session)
{
	std::stringstream ss;
	Clock clock;
	ss << '[' << Calendar::DateTime(clock) << "] ";
	ss << "NioChannel Close: [" << session.GetRemoteAddress() << "]\n";
	std::cout << ss.str();
}

void NioChannelEventHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
	in_packet->SetAccesOffset(2);
	try {
		std::shared_lock lock(binded_function_guard_);
		binded_function_.at(in_packet->ReadInt16())(session, *in_packet);
	}
	catch (const std::exception & e) {
		std::cout << e.what();
	}
}
