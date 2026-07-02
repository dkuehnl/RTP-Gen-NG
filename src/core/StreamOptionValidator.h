//
// Created by dkueh on 30.04.2026.
//

#ifndef RTPGEN_NG_STREAMOPTIONVALIDATOR_H
#define RTPGEN_NG_STREAMOPTIONVALIDATOR_H

#include <string>

#include "StreamOptions.h"

/**
 * @brief Result of a check_configuration() call.
 *
 * ok is false if any hard errors were found (missing mandatory fields, invalid ports).
 * errors will always be shown.
 * warnings indicate ignored/no-op events and are only visible in -v-mode.
 * info contains applied defaults and informational messages, only visible in -vv-mode.
 */
struct ValidationResult {
    bool ok{};
    std::vector<std::string> errors{};      ///< Hard errors; stream cannot be started.
    std::vector<std::string> warnings{};    ///< Non-fatal issues; affected events are dropped.
    std::vector<std::string> info{};        ///< Applied defaults and informational messages.
};

namespace sov {

    /**
     * @brief Validates and normalizes a StreamOptions instance.
     *
     * Applies defaults to all absent optional fields, validates mandatory fields
     * (dest_ip, dest_port), checks port ranges, and validates all change events.
     * Invalid or no-op change events are silently dropped from opts, but warning-message
     * will be generated and returned.
     *
     * @param opts StreamOptions to validate and mutate in-place.
     * @return ValidationResult with ok=false if any hard error was found.
     */
    ValidationResult check_configuration(StreamOptions& opts);
} // sov

#endif //RTPGEN_NG_STREAMOPTIONVALIDATOR_H