//
// Created by dkueh on 30.04.2026.
//

#include "StreamOptionValidator.h"

namespace {
    ValidationResult validate_basics(StreamOptions& opts) {
        ValidationResult result{true, {}, {}};

        if (opts.dest_ip.empty()) {
            result.errors.push_back("No dest_ip set");
            result.ok = false;
        }
        if (!opts.dest_port.has_value()) {
            result.errors.push_back("No destination-port set");
            result.ok = false;
        }
        if (opts.dest_port.has_value() && opts.dest_port.value() <= 1024) {
            result.errors.push_back("Invalid destination-port set");
            result.ok = false;
        }
        if (opts.source_port.has_value() && opts.source_port <= 1024) {
            result.errors.push_back("Invalid source-port set");
            result.ok = false;
        }

        return result;
    }

    void apply_defaults(StreamOptions& opts) {
        if (!opts.source_port.has_value()) opts.source_port = 30000;

        if (!opts.ptime_in_packet.has_value()) opts.ptime_in_packet = 20;
        if (!opts.ptime_btw_packet.has_value()) opts.ptime_btw_packet = 20;

        if (!opts.start_ssrc.has_value()) opts.start_ssrc = 0x112233;
        if (!opts.start_timestamp.has_value()) opts.start_timestamp = 0;
        if (!opts.start_codec.has_value()) opts.start_codec = 8;
        if (!opts.start_seq.has_value()) opts.start_seq = 0;
        if (!opts.seq_steps.has_value()) opts.seq_steps = 1;
        if (!opts.start_clockrate.has_value()) opts.start_clockrate = 8000;

        if (!opts.timestamp_step_size.has_value()) {
            opts.timestamp_step_size = (opts.start_clockrate.value() * opts.ptime_in_packet.value()) / 1000;
        }
    }

    ValidationResult validate_scenarios(StreamOptions& opts) {
        ValidationResult result{true, {}, {}};

        if (!opts.ssrc_changes.empty()) {
            for (const auto& entry : opts.ssrc_changes) {
                if (entry.trigger.value == 0)
                    result.warnings.push_back("No trigger-time for SSRC-change set, Event will be ignored.");
                if (!entry.new_ssrc.has_value())
                    result.warnings.push_back("No new SSRC for SSRC-Change-Event configured. Event will be ignored.");
            }

            std::erase_if(opts.ssrc_changes, [](const SSRCChange& e) {
                return e.trigger.value == 0 || !e.new_ssrc.has_value();
            });
        }

        return result;
    }
}

namespace sov {
    ValidationResult check_configuration(StreamOptions& opts) {
        apply_defaults(opts);
        auto result = validate_basics(opts);
        auto scenarios = validate_scenarios(opts);

        result.errors.insert(result.errors.end(), scenarios.errors.begin(), scenarios.errors.end());
        result.warnings.insert(result.warnings.end(), scenarios.warnings.begin(), scenarios.warnings.end());
        result.ok = result.errors.empty();
        return result;
    }
}