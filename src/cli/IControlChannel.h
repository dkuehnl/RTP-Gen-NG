//
// Created by dkueh on 02.07.2026.
//

#ifndef RTPGEN_NG_ICONTROLCHANNEL_H
#define RTPGEN_NG_ICONTROLCHANNEL_H

#include <functional>
#include <string>
#include <cstdint>

/// @brief Payload for a start_stream control message.
struct StartStreamMsg {
    std::string dest_ip;        ///< RTP destination IP, retrieved via ${CHANNEL(rtp,dest)} post-Answer.
    uint16_t dest_port;         ///< RTP destination port.
};

/// @brief Payload for an update_dest control message (re-INVITE scenarios).
struct UpdateDestMsg {
    std::string dest_ip;
    uint16_t dest_port;
};

/**
 * @brief Strategy interface for transport-agnostic stream control.
 *
 * Decouples RtpEngine from the concrete signaling transport (Unix socket, TCP,
 * signal handler, ...). Implementations receive start_stream/end_stream/
 * update_dest messages from an external peer (e.g. Asterisk via AMI-triggered
 * dialplan) and invoke the registered handler for each.
 *
 * @note listen() is expected to block; callers run it in a dedicated thread.
 *       stop() must be safely callable from a different thread to unblock it.
 */
class IControlChannel {
public:
    virtual ~IControlChannel() = default;

    using StartHandler  = std::function<void(const StartStreamMsg&)>;
    using EndHandler    = std::function<void()>;
    using UpdateHandler = std::function<void(const UpdateDestMsg&)>;

    /// @brief Registers the callback invoked on a received start_stream message.
    virtual void on_start_stream(StartHandler handler) = 0;

    /// @brief Registers the callback invoked on a received end_stream message.
    virtual void on_end_stream(EndHandler handler) = 0;

    /// @brief Registers the callback invoked on a received update_dest message.
    virtual void on_update_dest(UpdateHandler handler) = 0;

    /**
     * @brief Blocks and dispatches incoming control messages to registered handlers.
     * @note Intended to run in a dedicated thread; returns once stop() is called.
     */
    virtual void listen() = 0;

    /// @brief Unblocks a running listen() call from another thread. Thread-safe.
    virtual void stop() = 0;
};


#endif //RTPGEN_NG_ICONTROLCHANNEL_H