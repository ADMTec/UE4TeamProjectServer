#include "NioCipher.hpp"
#include "NioInternalBuffer.hpp"


int64_t NioCipher::GetHead(NioInternalBuffer& buffer)
{
    if (buffer.GetBufferLength() < 2) {
        return -1;
    }
    return buffer.ReadInt16();
}

int64_t NioCipher::GetPacketLength(int64_t head)
{
    return head;
}

void NioCipher::Decode(NioSession& session, NioInternalBuffer& buffer, NioInPacket& packet)
{
    int64_t head = this->GetHead(buffer);
    uint64_t packet_length = GetPacketLength(head);
    if (head < 0 || head < packet_length || (buffer.GetReadableLength() < packet_length)) {
        buffer.ResetReadOffset();
        return;
    }
    buffer.InitializePacket(packet, packet_length);
    buffer.MarkReadIndex(packet_length);
}

void NioCipher::Encode(NioSession& session, NioOutPacket& out_packet)
{

}
