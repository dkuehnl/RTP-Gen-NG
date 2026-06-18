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
        [](const T& a, const T& b) { return a.trigger.value < b.trigger.value; }
    );
}

ScenarioEngine::ScenarioEngine(const StreamOptions& opts) {
    init(opts);
}

const StreamState& ScenarioEngine::tick(uint64_t delta_ms) {
    m_state.current_seq++;
    m_state.current_timestamp += m_state.current_timestamp_step_size;
    return m_state;
}

void ScenarioEngine::init(const StreamOptions& opts) {
    extract_initial_state_values(opts);
    extract_changes(opts);
    sort_change_deques();
}

void ScenarioEngine::extract_changes(const StreamOptions& opts) {
    m_ssrc_changes = to_deque(opts.ssrc_changes);
    m_timestamp_changes = to_deque(opts.timestamp_changes);
    m_codec_change = to_deque(opts.codec_changes);
    m_sequence_change = to_deque(opts.sequence_changes);
    m_pause_stream = to_deque(opts.pause_stream);
    m_transport_changes = to_deque(opts.transport_changes);
}

void ScenarioEngine::sort_change_deques() {
    sort_by_trigger(m_ssrc_changes);
    sort_by_trigger(m_timestamp_changes);
    sort_by_trigger(m_codec_change);
    sort_by_trigger(m_sequence_change);
    sort_by_trigger(m_pause_stream);
    sort_by_trigger(m_transport_changes);
}

void ScenarioEngine::extract_initial_state_values(const StreamOptions& opts) {
    m_state.current_ssrc = opts.start_ssrc.value();
    m_state.current_seq = opts.start_seq.value();
    m_state.current_timestamp = opts.start_timestamp.value();
    m_state.current_timestamp_step_size = opts.timestamp_step_size.value();
    m_state.current_codec = opts.start_codec.value();
    m_state.current_clockrate = opts.start_clockrate.value();

    m_state.current_dest_ip = opts.dest_ip;
    m_state.current_dest_port = opts.dest_port.value();
    m_state.current_src_port = opts.source_port.value();
}

const StreamState& ScenarioEngine::get_state() const {
    return m_state;
}
