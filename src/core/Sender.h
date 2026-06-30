//
// Created by dkueh on 27.06.2026.
//

#ifndef RTPGEN_NG_SENDER_H
#define RTPGEN_NG_SENDER_H
#include "ScenarioEngine.h"

/**
 * @brief Sends serialized RTP packets over a UDP socket.
 *
 * Owns a single UDP socket bound to a (possibly kernel-assigned) source port.
 * The socket is created and bound at construction; if state.current_src_port
 * diverges from the currently bound port on a send() call, the socket is
 * transparently rebound via rebind_if_needed().
 *
 * Rebind failures are non-fatal: the previous socket remains in use and the
 * failure is logged to stderr, so the stream continues sending on the old
 * port rather than aborting. The same applies to sendto() failures.
 *
 * @note No TCP support; UDP unicast only. No ISender interface is planned.
 */
class Sender {
public:
    /**
     * @brief Constructs the Sender and binds the underlying UDP socket.
     * @param src_port Source port to bind to. 0 (default) lets the kernel
     *                 assign an ephemeral port.
     * @throws std::runtime_error If socket creation or binding fails.
     */
    explicit Sender(uint16_t src_port = 0);

    /**
     * @brief Closes the underlying socket.
     */
    ~Sender();

    /**
     * @brief Sends a single RTP packet to the destination described by state.
     *
     * Triggers a rebind via rebind_if_needed() if state.current_src_port no
     * longer matches the currently bound source port. Send failures (sendto()
     * returning < 0) are logged to stderr; the stream is not interrupted.
     *
     * @param packet Serialized RTP packet bytes, as produced by rtp::builder::generate_rtp_packet().
     * @param state  Current stream state; provides destination IP/port and source port.
     */
    void send(const std::vector<uint8_t>& packet, const StreamState& state);

private:
    int m_sockfd{-1};
    uint16_t m_src_port{};

    /**
     * @brief Rebinds the socket to state.current_src_port.
     *
     * Creates and binds a new socket before closing the old one, so the
     * previously bound socket remains usable if creation or binding fails.
     * On success, the old socket is closed and m_sockfd/m_src_port are updated.
     * On failure, an error is logged to stderr and the old socket/port are
     * left unchanged; the caller continues sending on the previous port.
     *
     * @param state Current stream state; current_src_port is the rebind target.
     */
    void rebind_if_needed(const StreamState& state);
};

#endif //RTPGEN_NG_SENDER_H