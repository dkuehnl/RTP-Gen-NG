//
// Created by dkueh on 01.07.2026.
//

#ifndef RTPGEN_NG_RTPENGINE_H
#define RTPGEN_NG_RTPENGINE_H

#include <thread>

#include "Scheduler.h"
#include "Sender.h"
#include "ScenarioEngine.h"

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
 * @note Sender, ScenarioEngine, and Scheduler are constructed directly in the
 *       initializer list (declaration order matters: m_opts before m_sender/
 *       m_engine/m_scheduler) since Scheduler holds reference members and none
 *       of the three types are default-constructible or assignable.
 */
class RtpEngine {
public:
    /**
     * @brief Resolves, validates, and wires up a complete RTP stream session.
     * @param raw_opts Raw options from CLI parsing or the interactive editor;
     *                  either input_file_path or input_content may be set.
     * @param debug_level Controls which validation messages are printed to stderr.
     * @throws std::runtime_error If validation fails (see StreamOptionValidator).
     * @throws YamlFileNotFound, WrongFileFormat, YamlUnknownTriggerType,
     *         YamlUnknownChangeEvent, YAML::Exception If input_file_path/
     *         input_content parsing fails.
     */
    explicit RtpEngine(StreamOptions& raw_opts, DebugLevel debug_level);

    /**
     * @brief Starts the blocking send loop via Scheduler::start_stream().
     */
    void run();

    /**
     * @brief End the Scheduler-loop and stops producing RTP packets.
     */
    void stop();

private:
    std::jthread m_worker;
    StreamOptions m_opts;
    Sender m_sender;
    ScenarioEngine m_engine;
    Scheduler m_scheduler;

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
};


#endif //RTPGEN_NG_RTPENGINE_H