//
// Created by dkueh on 31.05.2026.
//

#ifndef RTPGEN_NG_SCENARIOENGINE_H
#define RTPGEN_NG_SCENARIOENGINE_H
#include <deque>
#include <string>


#include "StreamOptions.h"

/**
 * @brief Snapshot of all mutable RTP stream parameters at a given point in time.
 *
 * Returned by reference from ScenarioEngine::tick() and ScenarioEngine::get_state().
 * Acts as the single source of truth consumed by Scheduler and PacketBuilder.
 *
 * elapsed_ms continues to increment during a pause; packet_count does not.
 * When is_paused is true, the Scheduler must suppress packet transmission for
 * ms_to_pause milliseconds before calling tick() again.
 */
struct StreamState {
    uint32_t        current_ssrc{};
    uint16_t        current_seq{};
    uint32_t        current_timestamp{};
    uint32_t        current_timestamp_step_size{};
    uint8_t         current_codec{};
    uint16_t        current_clockrate{};
    uint8_t         current_ptime_in_packet{};
    TriggerType     trigger_type{};                         ///< Determines whether events fire on packet_count or elapsed_ms.

    uint64_t        packet_count{0};
    uint64_t        elapsed_ms{0};
    bool            is_paused{false};                       ///< True for exactly one tick cycle after a PauseStream event fires.
    uint32_t        ms_to_pause{};                          ///< Duration the Scheduler must wait before the next tick(); reset to 0 after elapsed_ms is advanced.

    std::string     current_dest_ip{};
    uint16_t        current_dest_port{};
    uint16_t        current_src_port{};
};


/**
 * @brief Pure state machine for a single RTP stream scenario.
 *
 * Consumes a validated StreamOptions on construction and advances the stream
 * state one packet at a time via tick(). Owns no threads and performs no I/O.
 *
 * Change events (SSRCChange, TimestampChange, etc.) are sorted by trigger_value
 * at construction and consumed from the front of their respective deques as their
 * trigger threshold is crossed. Events are never re-evaluated after consumption.
 *
 * The caller (Scheduler) is responsible for the event loop, timing, and pause
 * handling. ScenarioEngine only decides *what* state the next packet carries.
 *
 * @note StreamOptions passed to the constructor must already be validated, and
 *       have all optional fields populated (i.e., sov::check_configuration() must
 *       have been called beforehand).
 */
class ScenarioEngine {
public:

    /**
     * @brief Constructs the engine and initialises StreamState from opts.
     * @param opts Fully validated StreamOptions. All optional fields must have values.
     */
    explicit ScenarioEngine(const StreamOptions& opts);

    /**
     * @brief Advances the stream by one packet and fires any pending change events.
     *
     * Each call increments current_seq, current_timestamp, packet_count, and
     * elapsed_ms. Change events whose trigger threshold is crossed are applied in
     * the order: SSRC → Timestamp → Codec → Sequence → Pause → Transport.
     *
     * If the previous tick set is_paused, elapsed_ms is fast-forwarded by ms_to_pause
     * at the start of this call before the normal increment, and is_paused/ms_to_pause
     * is reset.
     *
     * @param delta_ms Elapsed wall-clock time since the last tick() call [ms].
     * @return Const reference to the updated StreamState.
     */
    const StreamState& tick(uint64_t delta_ms);

    /**
     * @brief Returns the current StreamState without advancing it.
     * @return Const reference to the internal state (valid until next tick()).
     */
    [[nodiscard]] const StreamState& get_state() const;

    /**
    * @brief Exposes the internal SSRC change deque for unit testing only.
    * @return Const reference to the sorted, unconsumed SSRC change deque.
    */
    const std::deque<SSRCChange>& get_ssrc_changes_for() const { return m_ssrc_changes; };

private:
    StreamState m_state{};

    std::deque<SSRCChange>      m_ssrc_changes{};
    std::deque<TimestampChange> m_timestamp_changes{};
    std::deque<CodecChange>     m_codec_changes{};
    std::deque<SequenceChange>  m_sequence_changes{};
    std::deque<PauseStream>     m_pause_stream{};
    std::deque<TransportChange> m_transport_changes{};

    void init(const StreamOptions& opts);
    void extract_initial_state_values(const StreamOptions& opts);
    void extract_changes(const StreamOptions& opts);
    void sort_change_deques();

    void apply(const SSRCChange& ssrc_change);
    void apply(const TimestampChange& timestamp_change);
    void apply(const CodecChange& codec_change);
    void apply(const SequenceChange& sequence_change);
    void apply(const PauseStream& pause_stream);
    void apply(const TransportChange& transport_change);

    /// @brief Returns true if the event's trigger threshold has been reached,
    ///        based on the active TriggerType (packet count or elapsed ms).
    template<class T>
    bool trigger_reached(const T &event);
};


#endif //RTPGEN_NG_SCENARIOENGINE_H