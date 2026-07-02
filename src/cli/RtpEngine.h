//
// Created by dkueh on 01.07.2026.
//

#ifndef RTPGEN_NG_RTPENGINE_H
#define RTPGEN_NG_RTPENGINE_H

#include <thread>

#include "Scheduler.h"
#include "Sender.h"
#include "ScenarioEngine.h"
#include "IControlChannel.h"

/// @brief Verbosity level for message output.
enum class DebugLevel { None, Verbose, MoreVerbose, Debug };

/**
 * @brief Owns and wires up a single RTP stream session end-to-end.
 *
 * Resolves raw StreamOptions (CLI-args, YAML file-path, or in-memory YAML
 * content from the interactive editor) into a fully parsed configuration,
 * validates it via sov::check_configuration(), and constructs Sender,
 * ScenarioEngine, and Scheduler from the validated result.
 *
 * Construction is all-or-nothing: on validation failure, a std::runtime_error
 * is thrown and no member is left partially/invalidly constructed.
 *
 * Stream lifecycle is decoupled from process lifecycle: run() only starts the
 * control-channel listener thread. The actual Scheduler send-loop thread is
 * started lazily inside the on_start_stream handler once an external
 * start_stream control message arrives. stop() ends both the send-loop and
 * the control-channel listener (full session teardown), then invokes the
 * optional shutdown handler (see on_shutdown()) to notify the caller.
 *
 * @note Sender, ScenarioEngine, and Scheduler are constructed directly in the
 *       initializer list (declaration order matters: m_control_channel,
 *       m_control_worker, m_shutdown_handler before m_opts before
 *       m_sender/m_engine/m_scheduler) since Scheduler holds reference
 *       members and none of the three types are default-constructible or
 *       assignable.
 * @todo No guard yet against a second start_stream message arriving while
 *       m_worker is still running (would reassign a live jthread -> terminate).
 */
class RtpEngine {
public:
    /**
     * @brief Resolves, validates, and wires up a complete RTP stream session.
     * @param raw_opts Raw options from CLI parsing or the interactive editor;
     *                  either input_file_path or input_content may be set.
     * @param debug_level Controls which validation messages are printed to stderr.
     * @param control_channel Transport used to receive start/end/update_dest
     *                         signals from an external peer (e.g. Asterisk).
     *                         Referenced, not owned; caller keeps it alive for
     *                         the lifetime of the RtpEngine.
     * @throws std::runtime_error If validation fails (see StreamOptionValidator).
     * @throws YamlFileNotFound, WrongFileFormat, YamlUnknownTriggerType,
     *         YamlUnknownChangeEvent, YAML::Exception If input_file_path/
     *         input_content parsing fails.
     */
    explicit RtpEngine(StreamOptions& raw_opts, DebugLevel debug_level, IControlChannel& control_channel);

    /**
     * @brief Starts the control-channel listener thread.
     *
     * Does not start the RTP send-loop; that begins only once the control
     * channel receives a start_stream message (see wire_control_channel()).
     */
    void run();

    /**
     * @brief Ends the Scheduler send-loop and stops the control-channel listener.
     *
     * Full session teardown; called from the registered end_stream handler
     * or directly for process shutdown (e.g. on SIGTERM in main()).
     */
    void stop();

    /**
     * @brief Registers a callback invoked after full session teardown.
     *
     * Fired from the end_stream handler after stop() completes. Intended for
     * the caller (e.g. main()) to signal process-level shutdown (e.g. wake a
     * condition_variable) without RtpEngine depending on caller internals.
     * @param handler Callback taking no arguments; stored in m_shutdown_handler.
     */
    using ShutdownHandler = std::function<void()>;
    void on_shutdown(ShutdownHandler handler) { m_shutdown_handler = std::move(handler); }

private:
    IControlChannel& m_control_channel;
    std::jthread m_control_worker;
    ShutdownHandler m_shutdown_handler;

    StreamOptions m_opts;
    Sender m_sender;
    ScenarioEngine m_engine;
    Scheduler m_scheduler;
    std::jthread m_worker;

    /**
     * @brief Resolves raw_opts into a parsed StreamOptions.
     *
     * Dispatches on which source is populated: input_file_path (YAML file),
     * input_content (in-memory YAML from the editor flow), or neither
     * (pure CLI-args, returned unchanged).
     *
     * @param raw_opts Raw options; exactly one of input_file_path/input_content
     *                 is expected to be set, or neither for CLI-only configs.
     * @return Parsed StreamOptions, not yet validated/defaulted.
     */
    static StreamOptions parse_opts(StreamOptions& raw_opts);

    /**
     * @brief Validates opts via sov::check_configuration(), logs messages
     *        according to debug_level, and returns opts on success.
     * @param opts Parsed StreamOptions to validate; taken by value (sink).
     * @param debug_level Controls which of errors/warnings/info are printed.
     * @return The validated and defaulted StreamOptions.
     * @throws std::runtime_error If validation fails (result.ok == false).
     */
    static StreamOptions validate_or_throw(StreamOptions opts, DebugLevel debug_level);

    /**
     * @brief Registers RtpEngine's handlers on m_control_channel.
     *
     * start_stream spawns m_worker to run Scheduler::start_stream(). end_stream
     * calls stop() (full teardown, including the control channel itself).
     */
    void wire_control_channel();
};


#endif //RTPGEN_NG_RTPENGINE_H