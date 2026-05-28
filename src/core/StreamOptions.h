//
// Created by dkueh on 29.04.2026.
//

#ifndef RTPGEN_NG_STREAMOPTIONS_H
#define RTPGEN_NG_STREAMOPTIONS_H
#include <cstdint>
#include <vector>
#include <optional>

enum class TriggerType { AfterPackets, AfterSeconds };

struct Trigger {
    TriggerType type{};
    uint64_t value{};
};

struct SSRCChange {
    Trigger trigger{};
    std::optional<uint32_t> new_ssrc{};

    std::optional<bool>  continue_seq{};
    std::optional<uint16_t> seq_to_continue{};

    std::optional<bool>  continue_timestamp{};
    std::optional<uint32_t> timestamp_to_continue{};
};

struct TimestampChange {
    Trigger trigger{};
    std::optional<uint32_t> new_timestamp{};
    std::optional<int16_t> steps_to_jump{};
    //Step-Size ändern?

    std::optional<bool>  continue_seq{};
    std::optional<uint16_t> seq_to_continue{};
};

struct CodecChange {
    Trigger trigger{};
    std::optional<uint8_t> new_codec{};
    std::optional<uint16_t> new_clockrate{};

    std::optional<bool>  continue_ssrc{};
    std::optional<uint32_t> ssrc_to_continue{};

    std::optional<bool>  continue_seq{};
    std::optional<uint16_t> seq_to_continue{};

    std::optional<bool>  continue_timestamp{};
    std::optional<uint32_t> timestamp_to_continue{};
};

struct SequenceChange {
    Trigger trigger{};
    std::optional<int16_t> seq_to_jump{};
};

struct PauseStream {
    Trigger trigger{};
    std::optional<uint32_t> ms_to_pause{};
};

struct TransportChange {
    Trigger trigger{};
    std::optional<std::string> new_dest_ip{};
    std::optional<uint16_t> new_dest_port{};

    std::optional<bool>  use_random_new_source_port{};
    std::optional<uint16_t> new_source_port{};
};

//Only dest_ip and dest_port are mandatory to provide. For all other options will be a default-setter.
struct StreamOptions {
    std::string input_file{};
    bool save_config{false};
    std::string dest_ip{};
    //It's mandatory to set dest-port. std::optional is used because dest_port=0 is a valid use-case for this tool
    std::optional<uint16_t> dest_port{};
    std::optional<uint16_t> source_port{};
    std::optional<bool> use_tcp{};

    std::optional<uint8_t> ptime_in_packet{};
    std::optional<uint8_t> ptime_btw_packet{};

    std::optional<uint32_t> start_ssrc{};
    std::vector<SSRCChange> ssrc_changes{};

    std::optional<uint32_t> start_timestamp{};
    std::optional<uint32_t> timestamp_step_size{};
    std::vector<TimestampChange> timestamp_changes{};

    std::optional<uint8_t> start_codec{};
    std::optional<uint16_t> start_clockrate{};
    std::vector<CodecChange> codec_changes{};

    std::optional<uint16_t> start_seq{};
    std::optional<uint16_t> seq_steps{};
    std::vector<SequenceChange> sequence_changes{};

    std::vector<PauseStream> pause_stream{};
    std::vector<TransportChange> transport_changes{};
};

#endif //RTPGEN_NG_STREAMOPTIONS_H