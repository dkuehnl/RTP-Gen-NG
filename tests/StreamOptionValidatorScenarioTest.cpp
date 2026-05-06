//
// Created by dkueh on 03.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "StreamOptionValidator.h"

/******************************************************************
 * Global
******************************************************************/

StreamOptions get_basic_opts() {
    StreamOptions opts{};
    opts.dest_ip = "127.0.0.1";
    opts.dest_port = 35000;
    opts.start_ssrc = 0x112233;
    opts.start_timestamp = 0;
    opts.timestamp_step_size = 160;
    opts.start_codec = 8;
    opts.start_clockrate = 8000;
    opts.start_seq = 0;
    opts.seq_steps = 1;

    return opts;
}

/******************************************************************
 * Testing SSRC Scenarios
******************************************************************/


class SoSsrcScenarioTest : public ::testing::Test {
protected:
};

TEST_F(SoSsrcScenarioTest, EmptyTriggerTimeGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = { .type = TriggerType::AfterPackets }
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No trigger-value for SSRC-change set, Event will be ignored."));
}

TEST_F(SoSsrcScenarioTest, EmptyNewSSRCGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = { TriggerType::AfterPackets, 100 }
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No new SSRC for SSRC-Change-Event configured. Event will be ignored."));
}

TEST_F(SoSsrcScenarioTest, RemoveEventIfInvalid) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = { TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.ssrc_changes.size(), 0);
}

TEST_F(SoSsrcScenarioTest, RemoveOnlyInvalid) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = { TriggerType::AfterPackets, 100},
        .new_ssrc = 0x334455
    });
    opts.ssrc_changes.push_back({
        .trigger = {}});

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.ssrc_changes.size(), 1);
}

TEST_F(SoSsrcScenarioTest, SetInfoIfSeqNotContinue) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_ssrc = 0x334455,
        .continue_seq = false
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.info, ::testing::Contains("SSRC change with new sequencing activated, random start-sequence will be generated."));
}

TEST_F(SoSsrcScenarioTest, GenerateRandomSeq) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_ssrc = 0x334455,
        .continue_seq = false
    });

    auto result = sov::check_configuration(opts);
    ASSERT_TRUE(opts.ssrc_changes[0].seq_to_continue.has_value());
}

TEST_F(SoSsrcScenarioTest, SetInfoIfTimestampNotContinue) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_ssrc = 0.334455,
        .continue_timestamp = false
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.info, ::testing::Contains("SSRC change with new timestamp activated, random start-timestamp will be generated."));
}

TEST_F(SoSsrcScenarioTest, GenerateRandomTimestamp) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_ssrc = 0.334455,
        .continue_timestamp = false
    });

    auto result = sov::check_configuration(opts);
    ASSERT_TRUE(opts.ssrc_changes[0].timestamp_to_continue.has_value());
}

/******************************************************************
 * Testing Timestamp Scenarios
******************************************************************/


class SoTimestampScenarioTest : public ::testing::Test {
protected:
};

TEST_F(SoTimestampScenarioTest, EmptyTriggerTimeGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.timestamp_changes.push_back({
        .trigger{}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No trigger-value for Timestamp-change set, Event will be ignored."));
}

TEST_F(SoTimestampScenarioTest, NoSettingGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.timestamp_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("Neither new timestamp nor new step-size is set, Event will be ignored."));
}

TEST_F(SoTimestampScenarioTest, NoWarningIfOneIsSet) {
    auto opts = get_basic_opts();
    opts.timestamp_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_timestamp = 4000
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(result.warnings.size(), 0);
}

TEST_F(SoTimestampScenarioTest, RemoveOnlyInvalid) {
    auto opts = get_basic_opts();
    opts.timestamp_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .steps_to_jump = 30
    });
    opts.timestamp_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.timestamp_changes.size(), 1);
}

TEST_F(SoTimestampScenarioTest, SetInfoIfSeqNotContinue) {
    auto opts = get_basic_opts();
    opts.timestamp_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_timestamp = 5000,
        .continue_seq = false
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.info, ::testing::Contains("Timestamp change with new sequence numbering activated, random start-sequence will be generated."));
}

TEST_F(SoTimestampScenarioTest, GenerateRandomSeq) {
    auto opts = get_basic_opts();
    opts.timestamp_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_timestamp = 5000,
        .continue_seq = false
    });

    auto result = sov::check_configuration(opts);
    ASSERT_TRUE(opts.timestamp_changes[0].seq_to_continue.has_value());
}

/******************************************************************
 * Testing Codec Scenarios
******************************************************************/


class SoCodecScenarioTest : public ::testing::Test {
protected:
};

TEST_F(SoCodecScenarioTest, EmptyTriggerTimeGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No trigger-value for Codec-change set, Event will be ignored."));
}

TEST_F(SoCodecScenarioTest, NoSettingGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No Settings for codec-change set, Event will be ignored."));
}

TEST_F(SoCodecScenarioTest, NoWarningIfConfigSet) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 8,
        .ssrc_to_continue = 0x223344
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(result.warnings.size(), 0);
}

TEST_F(SoCodecScenarioTest, RemoveOnlyInvalid) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 8,
        .ssrc_to_continue = 0x223344
    });
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.codec_changes.size(), 1);
}

TEST_F(SoCodecScenarioTest, SetWarningIfSsrcContinues) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_codec = 5,
        .continue_ssrc = true
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("Behavior not recommended in production. Codec-switch within one SSRC."));
}

TEST_F(SoCodecScenarioTest, SetInfoIfSeqContinues) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_codec = 5,
        .continue_seq = true
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.info, ::testing::Contains("Sequencing will continue."));
}

TEST_F(SoCodecScenarioTest, SetInfoIfTimestampContinues) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger = {TriggerType::AfterPackets, 100},
        .new_codec = 5,
        .continue_timestamp = true
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.info, ::testing::Contains("Timestamp will continue."));
}

TEST_F(SoCodecScenarioTest, GenerateWarningIfSsrcNotConinuedButNoneProvided) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 9,
        .continue_ssrc = false,
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No new SSRC provided, but SSRC-change configured. Random new SSRC will be generated."));
}

TEST_F(SoCodecScenarioTest, GenerateRandomSSRC) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 9,
        .continue_ssrc = false,
    });

    auto result = sov::check_configuration(opts);
    ASSERT_TRUE(opts.codec_changes[0].ssrc_to_continue.has_value());
}

TEST_F(SoCodecScenarioTest, GenerateRandomSeq) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 9
    });

    auto result = sov::check_configuration(opts);
    ASSERT_TRUE(opts.codec_changes[0].seq_to_continue.has_value());
}

TEST_F(SoCodecScenarioTest, GenerateRandomTimestamp) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 9
    });

    auto result = sov::check_configuration(opts);
    ASSERT_TRUE(opts.codec_changes[0].timestamp_to_continue.has_value());
}

/******************************************************************
 * Testing Sequence Scenarios
******************************************************************/


class SoSeqScenarioTest : public ::testing::Test {
protected:
};

TEST_F(SoSeqScenarioTest, EmptyTriggerTimeGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.sequence_changes.push_back({
        .trigger{TriggerType::AfterPackets}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No trigger-value for Sequence-change set, Event will be ignored."));
}

TEST_F(SoSeqScenarioTest, NoSettingGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.sequence_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No value for Sequence-Jump provided, Event will be ignored."));
}

TEST_F(SoSeqScenarioTest, NoWarningIfConfigSet) {
    auto opts = get_basic_opts();
    opts.sequence_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .seq_to_jump = 100
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(result.warnings.size(), 0);
}

TEST_F(SoSeqScenarioTest, RemoveOnlyInvalid) {
    auto opts = get_basic_opts();
    opts.sequence_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .seq_to_jump = -100
    });
    opts.sequence_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.sequence_changes.size(), 1);
}

/******************************************************************
 * Testing Pause Scenarios
******************************************************************/


class SoPauseScenarioTest : public ::testing::Test {
protected:
};

TEST_F(SoPauseScenarioTest, EmptyTriggerTimeGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.pause_stream.push_back({
        .trigger{TriggerType::AfterPackets}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No trigger-value for Pause-change set, Event will be ignored."));
}

TEST_F(SoPauseScenarioTest, NoSettingGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.pause_stream.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No value for Stream-Pause provided, Event will be ignored."));
}

TEST_F(SoPauseScenarioTest, NoWarningIfConfigSet) {
    auto opts = get_basic_opts();
    opts.pause_stream.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .ms_to_pause = 100
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(result.warnings.size(), 0);
}

TEST_F(SoPauseScenarioTest, RemoveOnlyInvalid) {
    auto opts = get_basic_opts();
    opts.pause_stream.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .ms_to_pause = -100
    });
    opts.pause_stream.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.pause_stream.size(), 1);
}

/******************************************************************
 * Testing Transport Scenarios
******************************************************************/


class SoTransportScenarioTest : public ::testing::Test {
protected:
};

TEST_F(SoTransportScenarioTest, EmptyTriggerTimeGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.transport_changes.push_back({
        .trigger{TriggerType::AfterPackets}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No trigger-value for Transport-change set, Event will be ignored."));
}

TEST_F(SoTransportScenarioTest, NoSettingGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.transport_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No value for Transport-change provided, Event will be ignored."));
}

TEST_F(SoTransportScenarioTest, SettingRandomSourceGetsNoWarning) {
    auto opts = get_basic_opts();
    opts.transport_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .use_random_new_source_port = true
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(result.warnings.size(), 0);
}

TEST_F(SoTransportScenarioTest, RemoveOnlyInvalid) {
    auto opts = get_basic_opts();
    opts.transport_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_dest_ip = "192.168.178.1"
    });
    opts.transport_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.transport_changes.size(), 1);
}