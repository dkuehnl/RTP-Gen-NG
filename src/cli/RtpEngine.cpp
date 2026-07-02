//
// Created by dkueh on 01.07.2026.
//

#include "RtpEngine.h"

#include <iostream>

#include "StreamOptionValidator.h"
#include "YamlParser.h"

RtpEngine::RtpEngine(StreamOptions& raw_opts, DebugLevel debug_level)
    : m_opts(validate_or_throw(parse_opts(raw_opts), debug_level)),
      m_sender(m_opts.source_port.value_or(0)),
      m_engine(m_opts),
      m_scheduler(m_engine, m_sender, m_opts.ptime_btw_packet.value_or(20))
{}

StreamOptions RtpEngine::parse_opts(StreamOptions& raw_opts) {
    if (!raw_opts.input_file_path.empty()) {
        return yaml::parse(raw_opts.input_file_path);
    }

    if (!raw_opts.input_content.empty()) {
        return yaml::parse_from_string(raw_opts.input_content);
    }

    return raw_opts;
}

StreamOptions RtpEngine::validate_or_throw(StreamOptions opts, DebugLevel debug_level) {
    auto validate_result = sov::check_configuration(opts);

    if (debug_level >= DebugLevel::Verbose) {
        for (const auto& message : validate_result.errors) {
            std::cerr << message << std::endl;
        }
        if (debug_level >= DebugLevel::MoreVerbose) {
            for (const auto& message : validate_result.warnings) {
                std::cerr << message << std::endl;
            }
        }
        if (debug_level == DebugLevel::Debug) {
            for (const auto& message : validate_result.info) {
                std::cerr << message << std::endl;
            }
        }
    }

    if (!validate_result.ok) {
        throw std::runtime_error("Error while validating input.");
    }

    return opts;
}

void RtpEngine::run() {
    m_worker = std::jthread([this] { m_scheduler.start_stream(); });
}

void RtpEngine::stop() {
    m_scheduler.end_stream();
}
