//
// Created by dkueh on 03.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "StreamOptionValidator.h"

/******************************************************************
 * Testing SSRC Scenarios
******************************************************************/


class SoSsrcScenarioTest : public ::testing::Test {
protected:
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

};

TEST_F(SoSsrcScenarioTest, EmptyTriggerTimeGeneratesWarning) {
    auto opts = get_basic_opts();
    opts.ssrc_changes.push_back({
        .trigger = { .type = TriggerType::AfterPackets }
    });

    auto result = sov::check_configuration(opts);
    EXPECT_THAT(result.warnings, ::testing::Contains("No trigger-time for SSRC-change set, Event will be ignored."));
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
 * Testing SSRC Scenarios
******************************************************************/


class SoTimestampScenarioTest : public ::testing::Test {
protected:
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

};

TEST_F(SoTimestampScenarioTest, )