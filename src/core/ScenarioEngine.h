//
// Created by dkueh on 31.05.2026.
//

#ifndef RTPGEN_NG_SCENARIOENGINE_H
#define RTPGEN_NG_SCENARIOENGINE_H
#include <deque>
#include <string>

#include "StreamOptions.h"

struct StreamState {
    uint32_t current_ssrc{};
    uint16_t current_seq{};
    uint32_t current_timestamp{};
    uint8_t current_codec{};
    uint16_t current_clockrate{};

    uint64_t packet_count{0};
    uint64_t elapsed_ms{0};
    bool is_paused{false};

    std::string current_dest_ip{};
    uint16_t current_dest_port{};
    uint16_t current_src_port{};
};

class ScenarioEngine {
public:
    explicit ScenarioEngine(const StreamOptions& opts);
    [[nodiscard]] const StreamState& get_state() const;

private:
    StreamState m_state{};
    StreamState m_prev_state{};

    std::deque<SSRCChange> m_ssrc_changes{};

    std::deque<TimestampChange> m_timestamp_changes{};
    std::deque<CodecChange> m_codec_change{};
    std::deque<SequenceChange> m_sequence_change{};
    std::deque<PauseStream> m_pause_stream{};
    std::deque<TransportChange> m_transport_changes{};

    void extract_inital_state_values(const StreamOptions& opts);
};


#endif //RTPGEN_NG_SCENARIOENGINE_H