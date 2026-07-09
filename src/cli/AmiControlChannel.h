//
// Created by dkueh on 09.07.2026.
//

#ifndef RTPGEN_NG_AMICONTROLCHANNEL_H
#define RTPGEN_NG_AMICONTROLCHANNEL_H
#include <atomic>
#include <map>

#include "IControlChannel.h"

/**
 * @brief IControlChannel implementation backed by an Asterisk Manager Interface (AMI) connection.
 *
 * Connects to Asterisk via AMI (TCP), authenticates, and subscribes to call-state events.
 * A Newstate event with ChannelStateDesc "Up" triggers a Getvar roundtrip to resolve
 * CHANNEL(rtp,dest), which is forwarded via the start-stream handler. Hangup/DialEnd
 * events trigger the end-stream handler.
 *
 * Unlike UnixSocketControlChannel, the connection is established synchronously in the
 * constructor; listen() only runs the event-receive loop on the already-connected socket.
 */
class AmiControlChannel : public IControlChannel {
public:
    /**
     * @brief Connects to Asterisk AMI and performs the login handshake.
     * @param host AMI host (IPv4 address).
     * @param port AMI TCP port (typically 5038).
     * @param user AMI manager username.
     * @param secret AMI manager secret.
     * @throws std::runtime_error If socket creation, connection, or login fails.
     */
    explicit AmiControlChannel(std::string host, uint16_t port, std::string user, std::string secret);

    /// @brief Sends a Logoff action (best effort) and closes the socket.
    ~AmiControlChannel() override;

    void on_start_stream(StartHandler handler) override { m_start_handler = std::move(handler); };
    void on_end_stream(EndHandler handler) override { m_end_handler = std::move(handler); };
    void on_update_dest(UpdateHandler handler) override { m_update_handler = std::move(handler); };

    /**
     * @brief Runs the event-receive loop until stop() is called.
     *
     * Polls the AMI socket (200ms timeout) and dispatches complete messages as they arrive.
     * Terminates early if the connection is lost or a read error occurs.
     */
    void listen() override;

    /// @brief Signals listen() to terminate; takes effect on the next poll() timeout.
    void stop() override;

private:
    std::string m_host{};
    std::string m_user{}, m_secret{};
    uint16_t m_port{};
    int m_sockfd{-1};
    std::atomic_bool m_active{false};

    StartHandler m_start_handler;
    EndHandler m_end_handler;
    UpdateHandler m_update_handler;

    /**
     * @brief Consumes the Asterisk welcome banner and performs the Login action.
     * @throws std::runtime_error If the write fails or the login response does not indicate success.
     */
    void connect_and_login() const;

    /**
     * @brief Reads from the socket until a complete CRLF-terminated AMI message is received.
     * @return Raw message content, including the trailing blank-line terminator.
     * @throws std::runtime_error If the read fails or the connection is closed.
     */
    std::string read_message() const;

    /**
     * @brief Parses a raw AMI message into its Key: Value fields.
     * @param raw Raw message as returned by read_message().
     * @return Parsed fields. Malformed lines (no colon) are skipped.
     */
    std::map<std::string, std::string> parse_message(const std::string& raw);

    /**
     * @brief Routes a parsed AMI message to the appropriate handler.
     *
     * Ignores messages without an Event field (i.e. Action responses). On Newstate/Up,
     * resolves the RTP destination via getvar() and invokes the start-stream handler.
     * On Hangup/DialEnd, invokes the end-stream handler.
     *
     * @param fields Parsed message fields.
     */
    void dispatch(const std::map<std::string, std::string>& fields);

    /**
     * @brief Sends a Getvar action and returns the resolved value.
     * @param channel AMI channel identifier.
     * @param var Variable expression to resolve (e.g. "CHANNEL(rtp,dest)").
     * @return The resolved value, or an empty string on failure.
     */
    std::string getvar(const std::string& channel, const std::string& var);
};


#endif //RTPGEN_NG_AMICONTROLCHANNEL_H