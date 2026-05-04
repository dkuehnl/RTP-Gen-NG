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

//Move this test to all the Default-Change-Info-Messages
TEST_F(SoCodecScenarioTest, GenerateWarningIfUseNewSSRCIsSetButNoneProvided) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .use_new_ssrc = true,
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No new SSRC provided, but SSRC-change configured. Random new SSRC will be generated."));
}

TEST_F(SoCodecScenarioTest, NoWarningIfConfigSet) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 8,
        .new_ssrc = 0x223344
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(result.warnings.size(), 0);
}

TEST_F(SoCodecScenarioTest, RemoveOnlyInvalid) {
    auto opts = get_basic_opts();
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100},
        .new_codec = 8,
        .new_ssrc = 0x223344
    });
    opts.codec_changes.push_back({
        .trigger{TriggerType::AfterPackets, 100}
    });

    auto result = sov::check_configuration(opts);
    ASSERT_EQ(opts.codec_changes.size(), 1);
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