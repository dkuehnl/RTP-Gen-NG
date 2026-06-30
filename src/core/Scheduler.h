//
// Created by dkueh on 27.06.2026.
//

#ifndef RTPGEN_NG_SCHEDULER_H
#define RTPGEN_NG_SCHEDULER_H

#include <atomic>

#include "ScenarioEngine.h"
#include "Sender.h"

/**
 * @brief Drives the send loop for a single RTP stream.
 *
 * Repeatedly calls ScenarioEngine::tick() to advance the stream state, builds
 * the corresponding RTP packet via rtp::builder::generate_rtp_packet(), and
 * hands it to Sender::send(). Runs a blocking loop controlled by m_active;
 * start_stream() must be called from the thread that should block, while
 * end_stream() may be called from another thread or a signal handler to stop it.
 *
 * On a pause tick (StreamState::is_paused), no packet is sent; the thread
 * sleeps for ms_to_pause instead, then re-evaluates m_active.
 *
 * @note ScenarioEngine and Sender are held as reference members. The Scheduler
 *       does not own or copy them; the caller (e.g. main()) is responsible for
 *       keeping both objects alive for the lifetime of the Scheduler.
 */
class Scheduler {
public:
    /**
     * @brief Constructs the Scheduler bound to an existing engine and sender.
     * @param engine ScenarioEngine driving stream state; referenced, not copied.
     * @param sender Sender used for packet transmission; referenced, not copied.
     * @param start_interval Initial send interval between packets [ms], used both
     *                        as the tick() delta and the inter-packet sleep duration.
     */
    explicit Scheduler(ScenarioEngine& engine, Sender& sender, uint64_t start_interval);

    /**
     * @brief Starts the blocking send loop.
     *
     * Runs until m_active is set to false (via end_stream()). On each
     * iteration: tick the engine, send a packet (or sleep through a pause),
     * then sleep for m_ptime_btw_packets before the next iteration.
     */
    void start_stream();

    /**
     * @brief Signals the send loop to stop after its current iteration.
     *
     * Thread-safe; intended to be called from a different thread or a signal
     * handler than the one running start_stream().
     */
    void end_stream();

private:
    ScenarioEngine& m_engine;
    Sender& m_sender;

    std::atomic_bool m_active{false};
    uint64_t m_ptime_btw_packets{};
};


#endif //RTPGEN_NG_SCHEDULER_H