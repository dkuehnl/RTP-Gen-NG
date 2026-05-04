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
                    result.warnings.emplace_back("No trigger-value for SSRC-change set, Event will be ignored.");
                if (!entry.new_ssrc.has_value())
                    result.warnings.emplace_back("No new SSRC for SSRC-Change-Event configured. Event will be ignored.");
            }

            std::erase_if(opts.ssrc_changes, [](const SSRCChange& e) {
                return e.trigger.value == 0 || !e.new_ssrc.has_value();
            });
        }

        if (!opts.timestamp_changes.empty()) {
            for (const auto& entry : opts.timestamp_changes) {
                if (entry.trigger.value == 0)
                    result.warnings.emplace_back("No trigger-value for Timestamp-change set, Event will be ignored.");
                if (!entry.new_timestamp.has_value() && !entry.steps_to_jump.has_value())
                    result.warnings.emplace_back("Neither new timestamp nor new step-size is set, Event will be ignored.");
            }

            std::erase_if(opts.timestamp_changes, [](const TimestampChange& e) {
                return e.trigger.value == 0 || (!e.new_timestamp.has_value() && !e.steps_to_jump.has_value());
            });
        }

        if (!opts.codec_changes.empty()) {
            for (const auto& entry : opts.codec_changes) {
                if (entry.trigger.value == 0)
                    result.warnings.emplace_back("No trigger-value for Codec-change set, Event will be ignored.");
                if (!entry.new_codec.has_value() && !entry.new_clockrate.has_value())
                    result.warnings.emplace_back("No Settings for codec-change set, Event will be ignored.");
                if (entry.use_new_ssrc && !entry.new_ssrc.has_value()) {
                    result.warnings.emplace_back("No new SSRC provided, but SSRC-change configured. Random new SSRC will be generated.");
                }
            }

            std::erase_if(opts.codec_changes, [](const CodecChange& e) {
                return e.trigger.value == 0 || (!e.new_codec.has_value() && !e.new_clockrate.has_value());
            });
        }

        if (!opts.sequence_changes.empty()) {
            for (const auto& entry : opts.sequence_changes) {
                if (entry.trigger.value == 0)
                    result.warnings.emplace_back("No trigger-value for Sequence-change set, Event will be ignored.");
                if (!entry.seq_to_jump.has_value())
                    result.warnings.emplace_back("No value for Sequence-Jump provided, Event will be ignored.");
            }

            std::erase_if(opts.sequence_changes, [](const SequenceChange& e) {
                return e.trigger.value == 0 || !e.seq_to_jump.has_value();
            });
        }

        if (!opts.pause_stream.empty()) {
            for (const auto& entry : opts.pause_stream) {
                if (entry.trigger.value == 0)
                    result.warnings.emplace_back("No trigger-value for Pause-change set, Event will be ignored.");
                if (!entry.ms_to_pause.has_value())
                    result.warnings.emplace_back("No value for Stream-Pause provided, Event will be ignored.");
            }

            std::erase_if(opts.pause_stream, [](const PauseStream& e) {
                return e.trigger.value == 0 || !e.ms_to_pause.has_value();
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