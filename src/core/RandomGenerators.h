//
// Created by dkueh on 06.05.2026.
//

#ifndef RTPGEN_NG_RANDOMGENERATORS_H
#define RTPGEN_NG_RANDOMGENERATORS_H

#include <cstdint>

namespace rg::rtp {
    uint32_t generate_ssrc();
    uint32_t generate_timestamp();
    uint16_t generate_sequence();
}

#endif //RTPGEN_NG_RANDOMGENERATORS_H
