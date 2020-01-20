#include "UE4PacketCipher.hpp"

int64_t UE4PacketCipher::GetHead(NioInternalBuffer& buffer)
{
	if (buffer.GetReceiveLength() < 2) {
		return -1;
	}
	return buffer.ReadInt16();
}

int64_t UE4PacketCipher::GetPacketLength(int64_t head)
{
	return head;
}

void UE4PacketCipher::Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet)
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

void UE4PacketCipher::Encode(NioSession& session, NioOutPacket& out_packet)
{
}
