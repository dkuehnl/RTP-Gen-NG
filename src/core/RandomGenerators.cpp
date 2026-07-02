//
// Created by dkueh on 06.05.2026.
//

#include "RandomGenerators.h"
#include <random>
#include <limits>

namespace {
    std::mt19937 engine{std::random_device{}()};

    std::uniform_int_distribution<uint32_t> dist32 {
        0, std::numeric_limits<uint32_t>::max()
    };
    std::uniform_int_distribution<uint16_t>  dist16 {
        0, std::numeric_limits<uint16_t>::max()
    };
}

namespace rg::rtp {
    uint32_t generate_ssrc() {
        return dist32(engine);
    }

    uint32_t generate_timestamp() {
        return dist32(engine);
    }

    uint16_t generate_sequence() {
        return dist16(engine);
    }
}