//
// Created by dkueh on 06.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "YamlParser.h"


class YamlParserTest : public ::testing::Test {
protected:
    std::string wrong_filepath = "invalid/file.yaml";
    std::string wrong_file_type = std::string(TEST_DATA_DIR) + "test.txt";

    std::string complete_config = std::string(TEST_DATA_DIR) + "complete_config.yaml";
    std::string no_event_config = std::string(TEST_DATA_DIR) + "no_event_config.yaml";
    std::string minimal_config = std::string(TEST_DATA_DIR) + "minimal_config.yaml";
    std::string unknown_change_event = std::string(TEST_DATA_DIR) + "unknown_change_event.yaml";
};

TEST_F(YamlParserTest, ThrowExceptionIfFileNotFound) {
    try {
        yaml::parse(wrong_filepath);
        FAIL() << "Expected YamlFileNotFound";
    } catch (const YamlFileNotFound& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("No such file found"));
    }
}

TEST_F(YamlParserTest, ThrowExceptionIfWrongFileType) {
    try {
        yaml::parse(wrong_file_type);
        FAIL() << "Expected YamlFileNotFound";
    } catch (const WrongFileFormat& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("Invalid file type"));
    }
}

TEST_F(YamlParserTest, ParseConnectionDetails) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.dest_ip, "192.168.178.1");
    EXPECT_THAT(opts.dest_port, 34000);
    EXPECT_THAT(opts.source_port, 35000);
    EXPECT_FALSE(opts.use_tcp.value());
}

TEST_F(YamlParserTest, ParseStreamStartValues) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.start_ssrc, 0x112233);
    EXPECT_THAT(opts.start_timestamp, 160);
    EXPECT_THAT(opts.timestamp_step_size, 150);
    EXPECT_THAT(opts.start_seq, 10);
    EXPECT_THAT(opts.seq_steps, 1);
    EXPECT_THAT(opts.start_codec, 9);
    EXPECT_THAT(opts.start_clockrate, 8000);
    EXPECT_THAT(opts.ptime_in_packet, 20);
    EXPECT_THAT(opts.ptime_btw_packet, 20);
}

TEST_F(YamlParserTest, ParseTriggerTypeValue) {
    EXPECT_NO_THROW({
        auto opts = yaml::parse(complete_config);
    });
}

TEST_F(YamlParserTest, SetEveryEventToTriggerType) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.trigger_type, TriggerType::AfterPackets);
}

TEST_F(YamlParserTest, ParseSsrcChangeEvent) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.ssrc_changes[0].trigger_value, 100);
    EXPECT_THAT(opts.ssrc_changes[0].new_ssrc, 0x445566);
    EXPECT_FALSE(opts.ssrc_changes[0].continue_seq.value());
    EXPECT_THAT(opts.ssrc_changes[0].seq_to_continue, 300);
    EXPECT_FALSE(opts.ssrc_changes[0].continue_timestamp.value());
    EXPECT_THAT(opts.ssrc_changes[0].timestamp_to_continue, 320);
}

TEST_F(YamlParserTest, ParseTimestampChangeEvent) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.timestamp_changes[0].trigger_value, 20);
    EXPECT_THAT(opts.timestamp_changes[0].new_timestamp, 320);
    EXPECT_THAT(opts.timestamp_changes[0].steps_to_jump, 100);
    EXPECT_FALSE(opts.timestamp_changes[0].continue_seq.value());
    EXPECT_THAT(opts.timestamp_changes[0].seq_to_continue, 30);
}

TEST_F(YamlParserTest, ParseCodecChangeEvent) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.codec_changes[0].trigger_value, 500);
    EXPECT_THAT(opts.codec_changes[0].new_codec, 12);
    EXPECT_THAT(opts.codec_changes[0].new_clockrate, 16000);
    EXPECT_FALSE(opts.codec_changes[0].continue_ssrc.value());
    EXPECT_THAT(opts.codec_changes[0].ssrc_to_continue, 0x334455);
    EXPECT_FALSE(opts.codec_changes[0].continue_seq.value());
    EXPECT_THAT(opts.codec_changes[0].seq_to_continue, 32000);
    EXPECT_FALSE(opts.codec_changes[0].continue_timestamp.value());
    EXPECT_THAT(opts.codec_changes[0].timestamp_to_continue, 26123);
}

TEST_F(YamlParserTest, ParseSequenceChangeEvent) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.sequence_changes[0].trigger_value, 500);
    EXPECT_THAT(opts.sequence_changes[0].seq_to_jump, 6000);
}

TEST_F(YamlParserTest, ParsePauseStreamEvent) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.pause_stream[0].trigger_value, 500);
    EXPECT_THAT(opts.pause_stream[0].ms_to_pause, 1000);
}

TEST_F(YamlParserTest, ParseTransportChangeEvent) {
    auto opts = yaml::parse(complete_config);

    EXPECT_THAT(opts.transport_changes[0].trigger_value, 1000);
    EXPECT_THAT(opts.transport_changes[0].new_dest_ip, "192.168.178.2");
    EXPECT_THAT(opts.transport_changes[0].new_dest_port, 37000);
    EXPECT_FALSE(opts.transport_changes[0].use_random_new_source_port.value());
    EXPECT_THAT(opts.transport_changes[0].new_source_port, 2000);
}

TEST_F(YamlParserTest, NoExceptionWithOnlyStartConfig) {
    EXPECT_NO_THROW({
        auto opts = yaml::parse(no_event_config);
    });
}

TEST_F(YamlParserTest, NoExceptionWithMinimalConfig) {
    EXPECT_NO_THROW({
        auto opts = yaml::parse(minimal_config);
    });
}

TEST_F(YamlParserTest, ThrowExceptionIfUnknownChangeEvent) {
    try {
        yaml::parse(unknown_change_event);
        FAIL() << "Expected YamlUnknownChangeEvent";
    } catch (const YamlUnknownChangeEvent& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("Unknown Change-Event: invalidC"));
    }
}