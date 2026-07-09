//
// Created by dkueh on 31.05.2026.
//

#include "ScenarioEngine.h"

#include <stdexcept>
#include <algorithm>

template<typename T>
std::deque<T> to_deque(const std::vector<T>& vec) {
    return std::deque<T>(vec.begin(), vec.end());
}

template<typename T>
void sort_by_trigger(std::deque<T>& deque) {
    std::sort(deque.begin(), deque.end(),
        [](const T& a, const T& b) { return a.trigger_value < b.trigger_value; }
    );
}

template<typename T>
bool ScenarioEngine::trigger_reached(const T& event) {
    if (m_state.trigger_type == TriggerType::AfterPackets) {
        return m_state.packet_count >= event.trigger_value;
    }
    return m_state.elapsed_ms >= event.trigger_value;
}

ScenarioEngine::ScenarioEngine(const StreamOptions& opts) {
    init(opts);
}

void ScenarioEngine::init(const StreamOptions& opts) {
    extract_initial_state_values(opts);
    extract_changes(opts);
    sort_change_deques();
}

void ScenarioEngine::extract_changes(const StreamOptions& opts) {
    m_ssrc_changes = to_deque(opts.ssrc_changes);
    m_timestamp_changes = to_deque(opts.timestamp_changes);
    m_codec_changes = to_deque(opts.codec_changes);
    m_sequence_changes = to_deque(opts.sequence_changes);
    m_pause_stream = to_deque(opts.pause_stream);
    m_transport_changes = to_deque(opts.transport_changes);
}

void ScenarioEngine::sort_change_deques() {
    sort_by_trigger(m_ssrc_changes);
    sort_by_trigger(m_timestamp_changes);
    sort_by_trigger(m_codec_changes);
    sort_by_trigger(m_sequence_changes);
    sort_by_trigger(m_pause_stream);
    sort_by_trigger(m_transport_changes);
}

void ScenarioEngine::extract_initial_state_values(const StreamOptions& opts) {
    m_state.current_ssrc = opts.start_ssrc.value();
    m_state.current_seq = opts.start_seq.value();
    m_state.current_timestamp = opts.start_timestamp.value();
    m_state.current_timestamp_step_size = opts.timestamp_step_size.value();
    m_state.current_ptime_in_packet = opts.ptime_in_packet.value();
    m_state.current_codec = opts.start_codec.value();
    m_state.current_clockrate = opts.start_clockrate.value();
    m_state.trigger_type = opts.trigger_type.value();

    if (opts.control_channel == ControlChannelType::Unix) {
        m_state.current_dest_ip = opts.dest_ip;
        m_state.current_dest_port = opts.dest_port.value();
    }
    m_state.current_src_port = opts.source_port.value();
}

void ScenarioEngine::set_rtp_destination(const std::string& dst_ip, const uint16_t dst_port) {
    if (dst_ip.empty()) return;
    m_state.current_dest_ip = dst_ip;
    m_state.current_dest_port = dst_port;
}

const StreamState& ScenarioEngine::get_state() const {
    return m_state;
}

const StreamState& ScenarioEngine::tick(uint64_t delta_ms) {
    if (m_state.ms_to_pause > 0 ) {
        m_state.elapsed_ms += m_state.ms_to_pause;
        m_state.is_paused = false;
        m_state.ms_to_pause = 0;
    }

    m_state.current_seq++;
    m_state.current_timestamp += m_state.current_timestamp_step_size;
    m_state.packet_count++;
    m_state.elapsed_ms += delta_ms;

    while (!m_ssrc_changes.empty() && trigger_reached(m_ssrc_changes.front())) {
        apply(m_ssrc_changes.front());
        m_ssrc_changes.pop_front();
    }

    while (!m_timestamp_changes.empty() && trigger_reached(m_timestamp_changes.front())) {
        apply(m_timestamp_changes.front());
        m_timestamp_changes.pop_front();
    }

    while (!m_codec_changes.empty() && trigger_reached(m_codec_changes.front())) {
        apply(m_codec_changes.front());
        m_codec_changes.pop_front();
    }

    while (!m_sequence_changes.empty() && trigger_reached(m_sequence_changes.front())) {
        apply(m_sequence_changes.front());
        m_sequence_changes.pop_front();
    }

    while (!m_pause_stream.empty() && trigger_reached(m_pause_stream.front())) {
        apply(m_pause_stream.front());
        m_pause_stream.pop_front();
    }

    while (!m_transport_changes.empty() && trigger_reached(m_transport_changes.front())) {
        apply(m_transport_changes.front());
        m_transport_changes.pop_front();
    }

    return m_state;
}

void ScenarioEngine::apply(const SSRCChange& ssrc_change) {
    if (ssrc_change.new_ssrc.has_value()) {
        m_state.current_ssrc = ssrc_change.new_ssrc.value();
    }

    if (ssrc_change.continue_seq.has_value() && !ssrc_change.continue_seq.value()) {
        m_state.current_seq = ssrc_change.seq_to_continue.value();
    }

    if (ssrc_change.continue_timestamp.has_value() && !ssrc_change.continue_timestamp.value()) {
        m_state.current_timestamp = ssrc_change.timestamp_to_continue.value();
    }
}

void ScenarioEngine::apply(const TimestampChange& timestamp_change) {
    if (timestamp_change.new_timestamp.has_value()) {
        m_state.current_timestamp = timestamp_change.new_timestamp.value();
    }

    if (timestamp_change.steps_to_jump.has_value()) {
        m_state.current_timestamp += (timestamp_change.steps_to_jump.value() * m_state.current_timestamp_step_size);
    }

    if (timestamp_change.continue_seq.has_value() && !timestamp_change.continue_seq.value()) {
        m_state.current_seq = timestamp_change.seq_to_continue.value();
    }
}

void ScenarioEngine::apply(const CodecChange& codec_change) {
    if (codec_change.new_codec.has_value()) {
        m_state.current_codec = codec_change.new_codec.value();
    }

    if (codec_change.new_clockrate.has_value()) {
        m_state.current_clockrate = codec_change.new_clockrate.value();
    }

    if (codec_change.continue_ssrc.has_value() && !codec_change.continue_ssrc.value()) {
        m_state.current_ssrc = codec_change.ssrc_to_continue.value();
    }

    if (codec_change.continue_seq.has_value() && !codec_change.continue_seq.value()) {
        m_state.current_seq = codec_change.seq_to_continue.value();
    }

    if (codec_change.continue_timestamp.has_value() && !codec_change.continue_timestamp.value()) {
        m_state.current_timestamp = codec_change.timestamp_to_continue.value();
    }
}

void ScenarioEngine::apply(const SequenceChange& sequence_change) {
    if (sequence_change.seq_to_jump.has_value()) {
        m_state.current_seq += sequence_change.seq_to_jump.value();
    }
}

void ScenarioEngine::apply(const PauseStream& pause_stream) {
    if (pause_stream.ms_to_pause.has_value()) {
        m_state.is_paused = true;
        m_state.ms_to_pause = pause_stream.ms_to_pause.value();
    }
}

void ScenarioEngine::apply(const TransportChange& transport_change) {
    if (transport_change.new_dest_ip.has_value()) {
        m_state.current_dest_ip = transport_change.new_dest_ip.value();
    }

    if (transport_change.new_dest_port.has_value()) {
        m_state.current_dest_port = transport_change.new_dest_port.value();
    }

    if (!transport_change.use_random_new_source_port.value_or(false) && transport_change.new_source_port.has_value()) {
        m_state.current_src_port = transport_change.new_source_port.value();
    }

    if (transport_change.use_random_new_source_port.value_or(false)) {
        m_state.current_src_port = 0;
    }
}
