//
// Created by dkueh on 31.05.2026.
//

#include "ScenarioEngine.h"

ScenarioEngine::ScenarioEngine(const StreamOptions& opts) {
    extract_inital_state_values(opts);

}

void ScenarioEngine::extract_inital_state_values(const StreamOptions& opts) {
    m_state.current_ssrc = opts.start_ssrc.value();
    m_state.current_seq = opts.start_seq.value();
    m_state.current_timestamp = opts.start_timestamp.value();
    m_state.current_codec = opts.start_codec.value();
    m_state.current_clockrate = opts.start_clockrate.value();

    m_state.current_dest_ip = opts.dest_ip;
    m_state.current_dest_port = opts.dest_port.value();
    m_state.current_src_port = opts.source_port.value();
}

const StreamState &ScenarioEngine::get_state() const {
    return m_state;
}
