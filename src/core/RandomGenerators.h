//
// Created by dkueh on 06.05.2026.
//

#ifndef RTPGEN_NG_RANDOMGENERATORS_H
#define RTPGEN_NG_RANDOMGENERATORS_H

#include <cstdint>

namespace rg::rtp {
    /**
     * @brief Generates a cryptographically seeded random SSRC value.
     * @return Random uint32_t suitable for use as RTP SSRC.
     */
    uint32_t generate_ssrc();

    /**
     * @brief Generates a random RTP timestamp value.
     * @return Random uint32_t suitable for use as RTP timestamp.
     */
    uint32_t generate_timestamp();

    /**
     * @brief Generates a random RTP sequence number.
     * @return Random uint16_t suitable for use as RTP sequence number.
     */
    uint16_t generate_sequence();
}

#endif //RTPGEN_NG_RANDOMGENERATORS_H
