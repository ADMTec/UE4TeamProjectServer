#pragma once
#include <cstdint>

template<class Session, class InternalBUffer, class InPacket, class OutPacket>
class NioCipher
{
public:
    int64_t GetHead(InternalBUffer& buffer) {
        if (buffer.GetBufferLength() < 2) {
            return -1;
        }
        return buffer.ReadInt16();
    }
    int64_t GetPacketLength(int64_t head) {
        return head;
    }
    void Decode(Session& session, InternalBUffer& buffer, InPacket& packet) {
        int64_t head = this->GetHead(buffer);
        uint64_t packet_length = GetPacketLength(head);
        if (head < 0 || head < packet_length || (buffer.GetReadableLength() < packet_length)) {
            buffer.ResetReadOffset();
            return;
        }
        buffer.InitializePacket(packet, packet_length);
        buffer.MarkReadIndex(packet_length);
    }
    void Encode(Session& session, OutPacket& out_packet) {

    }
};