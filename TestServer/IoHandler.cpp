#include "IoHandler.hpp"
#include "TestServer.hpp"
#include <iostream>

int64_t PacketCipher::GetHead(NioInternalBuffer& buffer)
{
	if (buffer.GetReceiveLength() < 2) {
		return -1;
	}
	return buffer.ReadInt16();
}

int64_t PacketCipher::GetPacketLength(int64_t head)
{
	return head;
}

void PacketCipher::Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet)
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

void PacketCipher::Encode(NioSession& session, NioOutPacket& out_packet)
{
}


void IoHandler::OnSessionActive(NioSession& session)
{
    TestServer::Instance().ActiveClient(session);
}

void IoHandler::OnSessionError(NioSession& session, int error_code, const char* message)
{
    std::cout << "GameServer] session[" << session.GetId() << " error: " << error_code << " msg:" << message << std::endl;
}

void IoHandler::OnSessionClose(NioSession& session)
{
    TestServer::Instance().CloseClient(session.GetClientKey());
}

void IoHandler::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    in_packet->SetAccesOffset(2);
    TestServer::Instance().ProcessPacket(session, in_packet);
}

OutPacket::OutPacket(NioOutPacket::AllocBufferLength length)
	: NioOutPacket(length)
{
	this->SetAccesOffset(2);
}

void OutPacket::MakePacketHead()
{
	*reinterpret_cast<uint16_t*>(this->buffer_) = GetAccessOffset();
}
