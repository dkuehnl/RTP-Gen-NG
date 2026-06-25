//
// Created by dkueh on 25.06.2026.
//

#include "PacketBuilder.h"

namespace rtp::builder {
    std::vector<uint8_t> generate_rtp_packet(const StreamState& config) {
        const uint32_t payload_size = (config.current_clockrate * config.current_ptime_in_packet) / 1000;
        std::vector<uint8_t> packet(12 + payload_size, 0x00);

        // Byte 0: V=2, P=0, X=0, CC=0
        packet[0] = 0x80;
        // Byte 1: M=0, PT
        packet[1] = config.current_codec;
        // Bytes 2-3: Sequence Number
        packet[2] = (config.current_seq >> 8) & 0xFF;
        packet[3] = config.current_seq & 0xFF;
        // Bytes 4-7: Timestamp
        packet[4] = (config.current_timestamp >> 24) & 0xFF;
        packet[5] = (config.current_timestamp >> 16) & 0xFF;
        packet[6] = (config.current_timestamp >> 8)  & 0xFF;
        packet[7] = config.current_timestamp         & 0xFF;
        // Bytes 8-11: SSRC
        packet[8] = (config.current_ssrc >> 24) & 0xFF;
        packet[9] = (config.current_ssrc >> 16) & 0xFF;
        packet[10] = (config.current_ssrc >> 8) & 0xFF;
        packet[11] = config.current_ssrc        & 0xFF;

        return packet;
    }
}