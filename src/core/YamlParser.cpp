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
}

namespace yaml {
    StreamOptions parse(const std::string& filepath) {

        check_filepath(filepath);

        YAML::Node config = YAML::LoadFile(filepath);
        StreamOptions opt;

        const auto& connection = config["connectionDetails"];
        opt.dest_ip = connection["destinationIP"].as<std::string>();
        opt.dest_port = connection["destinationPort"].as<uint16_t>();
        set_if_defined(connection, "sourcePort", opt.source_port);
        set_if_defined(connection, "useTCP", opt.use_tcp);

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

        for (const auto& change : config["changes"]) {
            auto event_type = change["type"].as<std::string>();
            auto trigger_type = change["trigger"]["type"].as<std::string>();
            auto type = get_trigger_type(trigger_type);

            if (event_type == "ssrcChange") {
                SSRCChange sc;
                sc.trigger.type = type;
                sc.trigger.value = change["trigger"]["value"].as<uint64_t>();
                set_if_defined(change, "newSsrc", sc.new_ssrc);
                set_if_defined(change, "continueSeq", sc.continue_seq);
                set_if_defined(change, "seqToContinue", sc.seq_to_continue);
                set_if_defined(change, "continueTimestamp", sc.continue_timestamp);
                set_if_defined(change, "timestampToContinue", sc.timestamp_to_continue);

                opt.ssrc_changes.push_back(sc);
            } else if (event_type == "timestampChange") {
                TimestampChange tc;

                tc.trigger.type = type;
                tc.trigger.value = change["trigger"]["value"].as<uint64_t>();
                set_if_defined(change, "stepsToJump", tc.steps_to_jump);
                set_if_defined(change, "newTimestamp", tc.new_timestamp);
                set_if_defined(change, "continueSeq", tc.continue_seq);
                set_if_defined(change, "seqToContinue", tc.seq_to_continue);

                opt.timestamp_changes.push_back(tc);
            } else if (event_type == "codecChange") {
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

                opt.codec_changes.push_back(cc);
            } else if (event_type == "sequenceChange") {
                SequenceChange sc;

                sc.trigger.type = type;
                sc.trigger.value = change["trigger"]["value"].as<uint64_t>();
                sc.seq_to_jump = change["seqToJump"].as<int16_t>();

                opt.sequence_changes.push_back(sc);
            }  else if (event_type == "pauseStream") {
                PauseStream ps;

                ps.trigger.type = type;
                ps.trigger.value = change["trigger"]["value"].as<uint64_t>();
                ps.ms_to_pause = change["msToPause"].as<uint32_t>();

                opt.pause_stream.push_back(ps);
            } else if (event_type == "transportChange") {
                TransportChange tc;

                tc.trigger.type = type;
                tc.trigger.value = change["trigger"]["value"].as<uint64_t>();
                tc.new_dest_ip = change["newDestIp"].as<std::string>();
                tc.new_dest_port = change["newDestPort"].as<uint16_t>();
                tc.use_random_new_source_port = change["useRandomNewSourcePort"].as<bool>();
                tc.new_source_port = change["newSourcePort"].as<uint16_t>();

                opt.transport_changes.push_back(tc);
            }

        }

        return opt;
    }
}