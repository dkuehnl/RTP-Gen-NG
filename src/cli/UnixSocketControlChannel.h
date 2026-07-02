//
// Created by dkueh on 02.07.2026.
//

#ifndef RTPGEN_NG_UNIXSOCKETCONTROLCHANNEL_H
#define RTPGEN_NG_UNIXSOCKETCONTROLCHANNEL_H
#include "IControlChannel.h"

#include <atomic>


/**
 * @brief Unix domain socket implementation of IControlChannel.
 *
 * Binds a stream socket at construction and accepts single-shot connections
 * in listen(), reading one message per connection and dispatching it to the
 * matching registered handler. Uses poll() with a 200ms timeout instead of a
 * blocking accept(), so stop() can unblock listen() without relying on
 * platform-specific shutdown()-on-listening-socket behavior.
 *
 * @note dispatch() currently recognizes message prefixes ("start_stream",
 *       "stop_stream", "update_stream"), but payload parsing (dest_ip/dest_port
 *       extraction into StartStreamMsg/UpdateDestMsg) is not yet implemented;
 *       only end_stream is fully wired.
 */
class UnixSocketControlChannel : public IControlChannel {
public:
    /**
     * @brief Creates, binds, and listens on a Unix domain stream socket.
     * @param socket_path Filesystem path for the socket; unlinked before bind
     *                     if a stale file exists, and unlinked again on destruction.
     * @throws std::runtime_error If socket creation, bind, or listen fails.
     */
    explicit UnixSocketControlChannel(std::string socket_path);

    /// @brief Stops the listener (if active), closes the socket, unlinks the path.
    ~UnixSocketControlChannel() override;

    void on_start_stream(StartHandler handler) override { m_start_handler = std::move(handler); }
    void on_end_stream(EndHandler handler) override { m_end_handler = std::move(handler); }
    void on_update_dest(UpdateHandler handler) override { m_update_handler = std::move(handler); }

    /**
     * @brief Blocking accept loop; polls m_sockfd with a 200ms timeout, so
     *        m_active is re-checked periodically after stop().
     *
     * Each accepted connection is read once (single message per connection,
     * up to 511 bytes), dispatched, then closed.
     */
    void listen() override;

    /// @brief Sets m_active to false; listen() exits within one poll() cycle (~200ms).
    void stop() override;

private:
    std::string m_socket_path;
    int m_sockfd{-1};
    std::atomic_bool m_active{false};

    StartHandler m_start_handler;
    EndHandler m_end_handler;
    UpdateHandler m_update_handler;

    /**
     * @brief Routes a raw received message to the matching handler by prefix.
     * @param raw_message Raw bytes read from the accepted connection.
     * @todo Parse dest_ip/dest_port payload for start_stream/update_stream
     *       instead of invoking with a default-constructed struct.
     */
    void dispatch(const std::string& raw_message);
};


#endif //RTPGEN_NG_UNIXSOCKETCONTROLCHANNEL_H