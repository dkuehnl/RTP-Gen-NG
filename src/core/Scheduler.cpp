//
// Created by dkueh on 27.06.2026.
//

#include "Scheduler.h"
#include "PacketBuilder.h"

#include <thread>

Scheduler::Scheduler(ScenarioEngine& engine, Sender& sender, uint64_t start_interval)
    : m_engine(engine), m_sender(sender), m_ptime_btw_packets(start_interval) {}

void Scheduler::start_stream() {
    m_active = true;

    while (m_active) {
        auto stream_state = m_engine.tick(m_ptime_btw_packets);

        if (stream_state.is_paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(stream_state.ms_to_pause));
            continue;
        }

        auto rtp_packet = rtp::builder::generate_rtp_packet(stream_state);
        m_sender.send(rtp_packet, stream_state);


        std::this_thread::sleep_for(std::chrono::milliseconds(m_ptime_btw_packets));
    }
}

void Scheduler::end_stream() {
    m_active = false;
}
