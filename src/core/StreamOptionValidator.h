//
// Created by dkueh on 30.04.2026.
//

#ifndef RTPGEN_NG_STREAMOPTIONVALIDATOR_H
#define RTPGEN_NG_STREAMOPTIONVALIDATOR_H

#include <string>

#include "StreamOptions.h"

struct ValidationResult {
    bool ok{};
    std::vector<std::string> errors{};
    std::vector<std::string> warnings{};
};

namespace sov {
    ValidationResult check_configuration(StreamOptions& opts);
} // sov

#endif //RTPGEN_NG_STREAMOPTIONVALIDATOR_H