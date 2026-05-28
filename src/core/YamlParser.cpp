//
// Created by dkueh on 06.05.2026.
//

#include <filesystem>

#include "YamlParser.h"
#include "yaml-cpp/yaml.h"

namespace fs = std::filesystem;

namespace {
    void check_filepath(const std::string& filepath) {
        if (!fs::is_regular_file(filepath)) {
            throw YamlFileNotFound("No such file found: " + filepath);
        }

        std::string ext = fs::path(filepath).extension().string();
        if (ext != ".yaml" && ext != ".yml") {
            throw WrongFileFormat("Invalid file type: " + ext);
        }
    }

    template<typename T>
    void set_if_defined(const YAML::Node& node, const std::string& key, std::optional<T>& target) {
        if (node[key].IsDefined()) {
            target = node[key].as<T>();
        }
    }

    TriggerType get_trigger_type(const std::string& type) {
        if (type == "AfterPackets") return TriggerType::AfterPackets;
        if (type == "AfterSeconds") return TriggerType::AfterSeconds;
        throw YamlUnknownTriggerType("Unknown Trigger type: " + type);
    }

    SSRCChange parse_ssrc_change(const TriggerType& type, const YAML::Node& change) {
        SSRCChange sc;

        sc.trigger.type = type;
        sc.trigger.value = change["trigger"]["value"].as<uint64_t>();
        set_if_defined(change, "newSsrc", sc.new_ssrc);
        set_if_defined(change, "continueSeq", sc.continue_seq);
        set_if_defined(change, "seqToContinue", sc.seq_to_continue);
        set_if_defined(change, "continueTimestamp", sc.continue_timestamp);
        set_if_defined(change, "timestampToContinue", sc.timestamp_to_continue);

        return sc;
    }

    TimestampChange parse_timestamp_change(const TriggerType& type, const YAML::Node& change) {
        TimestampChange tc;

        tc.trigger.type = type;
        tc.trigger.value = change["trigger"]["value"].as<uint64_t>();
        set_if_defined(change, "stepsToJump", tc.steps_to_jump);
        set_if_defined(change, "newTimestamp", tc.new_timestamp);
        set_if_defined(change, "continueSeq", tc.continue_seq);
        set_if_defined(change, "seqToContinue", tc.seq_to_continue);

        return tc;
    }

    CodecChange parse_codec_change(const TriggerType& type, const YAML::Node& change) {
        CodecChange cc;

        cc.trigger.type = type;
        cc.trigger.value = change["trigger"]["value"].as<uint64_t>();
        set_if_defined(change, "newCodec", cc.new_codec);
        set_if_defined(change, "newClockrate", cc.new_clockrate);
        set_if_defined(change, "continueSsrc", cc.continue_ssrc);
        set_if_defined(change, "ssrcToContinue", cc.ssrc_to_continue);
        set_if_defined(change, "continueSeq", cc.continue_seq);
        set_if_defined(change, "seqToContinue", cc.seq_to_continue);
        set_if_defined(change, "continueTimestamp", cc.continue_timestamp);
        set_if_defined(change, "timestampToContinue", cc.timestamp_to_continue);

        return cc;
    }

    SequenceChange parse_sequence_change(const TriggerType& type, const YAML::Node& change) {
        SequenceChange sc;

        sc.trigger.type = type;
        sc.trigger.value = change["trigger"]["value"].as<uint64_t>();
        set_if_defined(change, "seqToJump", sc.seq_to_jump);

        return sc;
    }

    PauseStream parse_pause_stream_change(const TriggerType& type, const YAML::Node& change) {
        PauseStream ps;

        ps.trigger.type = type;
        ps.trigger.value = change["trigger"]["value"].as<uint64_t>();
        set_if_defined(change, "msToPause", ps.ms_to_pause);

        return ps;
    }

    TransportChange parse_transport_change(const TriggerType& type, const YAML::Node& change) {
        TransportChange tc;

        tc.trigger.type = type;
        tc.trigger.value = change["trigger"]["value"].as<uint64_t>();
        set_if_defined(change, "newDestIp", tc.new_dest_ip);
        set_if_defined(change, "newDestPort", tc.new_dest_port);
        set_if_defined(change, "useRandomNewSourcePort", tc.use_random_new_source_port);
        set_if_defined(change, "newSourcePort", tc.new_source_port);

        return tc;
    }

    void parse_connection_details(const YAML::Node& config, StreamOptions& opt) {
        const auto& connection = config["connectionDetails"];

        opt.dest_ip = connection["destinationIP"].as<std::string>();
        opt.dest_port = connection["destinationPort"].as<uint16_t>();
        set_if_defined(connection, "sourcePort", opt.source_port);
        set_if_defined(connection, "useTCP", opt.use_tcp);
    }

    void parse_stream_start(const YAML::Node& config, StreamOptions& opt) {
        if (!config["streamStartValues"].IsDefined()) {
            return;
        }

        const auto& stream_start = config["streamStartValues"];

        set_if_defined(stream_start, "ssrc", opt.start_ssrc);
        set_if_defined(stream_start, "timestamp", opt.start_timestamp);
        set_if_defined(stream_start, "timestampStep", opt.timestamp_step_size);
        set_if_defined(stream_start, "seq", opt.start_seq);
        set_if_defined(stream_start, "seqStep", opt.seq_steps);
        set_if_defined(stream_start, "codec", opt.start_codec);
        set_if_defined(stream_start, "startClockrate", opt.start_clockrate);
        set_if_defined(stream_start, "ptimeInPacket", opt.ptime_in_packet);
        set_if_defined(stream_start, "ptimeBtwPacket", opt.ptime_btw_packet);
    }

    void parse_change_events(const YAML::Node& config, StreamOptions& opt) {
        if (!config["changes"].IsDefined()) {
            return;
        }

        for (const auto& change : config["changes"]) {
            auto event_type = change["type"].as<std::string>();
            auto trigger_type = change["trigger"]["type"].as<std::string>();
            auto type = get_trigger_type(trigger_type);

            if (event_type == "ssrcChange") {
                opt.ssrc_changes.push_back(parse_ssrc_change(type, change));
            } else if (event_type == "timestampChange") {
                opt.timestamp_changes.push_back(parse_timestamp_change(type, change));
            } else if (event_type == "codecChange") {
                opt.codec_changes.push_back(parse_codec_change(type, change));
            } else if (event_type == "sequenceChange") {
                opt.sequence_changes.push_back(parse_sequence_change(type, change));
            }  else if (event_type == "pauseStream") {
                opt.pause_stream.push_back(parse_pause_stream_change(type, change));
            } else if (event_type == "transportChange") {
                opt.transport_changes.push_back(parse_transport_change(type, change));
            } else {
                throw YamlUnknownChangeEvent("Unknown Change-Event: " + event_type);
            }

        }
    }
}

namespace yaml {
    StreamOptions parse(const std::string& filepath) {

        check_filepath(filepath);

        YAML::Node config = YAML::LoadFile(filepath);
        StreamOptions opt;

        parse_connection_details(config, opt);
        parse_stream_start(config, opt);
        parse_change_events(config, opt);

        return opt;
    }
}