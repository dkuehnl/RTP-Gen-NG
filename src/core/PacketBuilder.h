//
// Created by dkueh on 25.06.2026.
//

#ifndef RTPGEN_NG_PACKETBUILDER_H
#define RTPGEN_NG_PACKETBUILDER_H
#include <cstdint>
#include <vector>

#include "ScenarioEngine.h"


namespace rtp::builder {

    /**
     * @brief Builds a ready-to-send RTP packet from the current stream state.
     *
     * Serializes a 12-byte fixed RTP header (V=2, no padding, no extension, no CSRC)
     * followed by a zero-filled payload. Payload size is derived from
     * (clockrate * ptime) / 1000. All multi-byte fields are written big-endian.
     *
     * @param state Current stream state; must originate from a validated StreamOptions.
     * @return Serialized RTP packet as a byte vector (12 + payload_size bytes).
     */
    std::vector<uint8_t> generate_rtp_packet(const StreamState& config);
};


#endif //RTPGEN_NG_PACKETBUILDER_H