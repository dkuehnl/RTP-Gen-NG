//
// Created by dkueh on 29.04.2026.
//

#ifndef RTPGEN_NG_STREAMOPTIONS_H
#define RTPGEN_NG_STREAMOPTIONS_H
#include <cstdint>
#include <vector>
#include <optional>


enum class TriggerType { AfterPackets, AfterSeconds };

/**
 * @brief Defines the trigger condition for a stream change-event
 * @param type Type of trigger (AfterPackets or AfterSeconds)
 * @param value Threshold value (packet or seconds)
 */
struct Trigger {
    TriggerType type{};     ///< Whether to trigger after a packet count or elapsed seconds.
    uint64_t value{};       ///< Threshold value (packet count or seconds).
};

/**
 * @brief Describes an SSRC substitution event.
 *
 * Dropped by check_configuration() if trigger.value == 0 or new_ssrc is absent.
 * continue_seq defaults to true if absent; if false and seq_to_continue is empty, a random seq is generated into seq_to_continue.
 * continue_timestamp defaults to true if absent; if false and timestamp_to_continue is empty, a random timestamp is generated into timestamp_to_continue.
 */
struct SSRCChange {
    Trigger trigger{};
    std::optional<uint32_t> new_ssrc{};                 ///< New SSRC; randomized if absent.

    std::optional<bool>  continue_seq{};                ///< If true (default), seq continues from current value.
    std::optional<uint16_t> seq_to_continue{};          ///< Explicit seq start if continue_seq is false. If empty a random value will be generated.

    std::optional<bool>  continue_timestamp{};          ///< If true (default), timestamp continues from current value.
    std::optional<uint32_t> timestamp_to_continue{};    ///< Explicit timestamp start if continue_timestamp is false. If empty a random value will be generated.
};

/**
 * @brief Describes a timestamp manipulation event.
 *
 * Dropped by check_configuration() if trigger.value == 0 or both new_timestamp and steps_to_jump are absent.
 * Exactly one of new_timestamp (absolute) or steps_to_jump (relative, negative = backwards) should be set.
 * continue_seq defaults to true if absent; if false and seq_to_continue is empty, a random seq is generated into seq_to_continue.
 */
struct TimestampChange {
    Trigger trigger{};
    std::optional<uint32_t> new_timestamp{};    ///< Jump to absolute timestamp value.
    std::optional<int16_t> steps_to_jump{};     ///< Relative jump in timestamp steps (negative = backwards).
    //TODO: Step-Size ändern?

    std::optional<bool>  continue_seq{};
    std::optional<uint16_t> seq_to_continue{};
};

/**
 * @brief Describes a codec substitution event.
 *
 * Dropped by check_configuration() if trigger.value == 0 or both new_codec and new_clockrate are absent.
 * continue_ssrc defaults to false; if false and ssrc_to_continue is absent, a random SSRC is generated.
 * continue_ssrc = true is accepted but triggers a warning (codec switch within same SSRC).
 * continue_seq defaults to false; if false and seq_to_continue is absent, a random seq is generated into seq_to_continue.
 * continue_timestamp defaults to false; if false and timestamp_to_continue is absent, a random timestamp is generated into timestamp_to_continue.
 */
struct CodecChange {
    Trigger trigger{};
    std::optional<uint8_t> new_codec{};
    std::optional<uint16_t> new_clockrate{};

    std::optional<bool>  continue_ssrc{};               ///< Default: false. true produces a validator warning.
    std::optional<uint32_t> ssrc_to_continue{};         ///< Randomized if empty and continue_seq is false.

    std::optional<bool>  continue_seq{};                ///< Default: false.
    std::optional<uint16_t> seq_to_continue{};          ///< Randomized if empty and continue_seq is false.

    std::optional<bool>  continue_timestamp{};          ///< Default: false.
    std::optional<uint32_t> timestamp_to_continue{};    ///< Randomized if empty and continue_seq is false.
};

/**
 * @brief Describes a sequence number jump event.
 *
 * Dropped by check_configuration() if trigger.value == 0 or seq_to_jump is absent.
 */
struct SequenceChange {
    Trigger trigger{};
    std::optional<int16_t> seq_to_jump{};   ///< Relative jump (negative = backwards).
};

/**
 * @brief Describes a stream pause (no packets sent for the specified duration).
 *
 * Dropped by check_configuration() if trigger.value == 0 or ms_to_pause is absent.
 */
struct PauseStream {
    Trigger trigger{};
    std::optional<uint32_t> ms_to_pause{};
};

/**
 * @brief Describes a transport-layer redirect event.
 *
 * Dropped by check_configuration() if trigger.value == 0 or none of new_dest_ip, new_dest_port,
 * new_source_port, or use_random_new_source_port(=true) is set.
 * use_random_new_source_port defaults to false; if true, new_source_port is ignored.
 */
struct TransportChange {
    Trigger trigger{};
    std::optional<std::string> new_dest_ip{};
    std::optional<uint16_t> new_dest_port{};

    std::optional<bool>  use_random_new_source_port{};  ///< Default: false. If true, new_source_port is ignored.
    std::optional<uint16_t> new_source_port{};
};

/**
 * @brief Top-level configuration for a single RTP stream session.
 *
 * Only dest_ip and dest_port are mandatory. All other fields fall back to
 * defaults applied by check_configuration().
 */
struct StreamOptions {
    std::string input_file{};
    bool save_config{false};

    std::string dest_ip{};                                  ///< Mandatory. Must not be empty.
    ///TODO: Erase Port-Check, Port 0 should be valid for testing but generate warning. Current: Error-Message and validation fails
    std::optional<uint16_t> dest_port{};                    ///< Mandatory. std::optional because port 0 is a valid test value. Must be > 1024.
    std::optional<uint16_t> source_port{};                  ///< Default: 30000. Must be > 1024 if set.
    std::optional<bool> use_tcp{};                          ///< Default: false (UDP).

    std::optional<uint8_t> ptime_in_packet{};               ///< Packetization time declared in packet [ms]. Default: 20.
    std::optional<uint8_t> ptime_btw_packet{};              ///< Actual send interval between packets [ms]. Default: 20.

    std::optional<uint32_t> start_ssrc{};                   ///< Default: 0x112233.
    std::vector<SSRCChange> ssrc_changes{};                 ///< Empty by default

    std::optional<uint32_t> start_timestamp{};              ///< Default: 0.
    std::optional<uint32_t> timestamp_step_size{};          ///< Default: (clockrate * ptime_in_packet) / 1000.
    std::vector<TimestampChange> timestamp_changes{};       ///< Empty by default

    std::optional<uint8_t> start_codec{};                   ///< Payload type. Default: 8 (PCMA).
    std::optional<uint16_t> start_clockrate{};              ///< Default: 8000 Hz.
    std::vector<CodecChange> codec_changes{};               ///< Empty by default

    std::optional<uint16_t> start_seq{};                    ///< Default: 0.
    std::optional<uint16_t> seq_steps{};                    ///< Sequence-Increment per packet. Default: 1.
    std::vector<SequenceChange> sequence_changes{};         ///< Empty by default

    std::vector<PauseStream> pause_stream{};                ///< Empty by default
    std::vector<TransportChange> transport_changes{};       ///< Empty by default
};

#endif //RTPGEN_NG_STREAMOPTIONS_H