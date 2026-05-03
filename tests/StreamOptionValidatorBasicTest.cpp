//
// Created by dkueh on 03.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "StreamOptionValidator.h"

/******************************************************************
 * Testing for valid Inputs
******************************************************************/

class SOValidatorTest : public ::testing::Test {
protected:
    StreamOptions empty_opts{};
};

TEST_F(SOValidatorTest, DetectMissingIP) {
    auto result = sov::check_configuration(empty_opts);

    ASSERT_FALSE(result.ok);
    EXPECT_THAT(result.errors, ::testing::Contains("No dest_ip set"));
}

TEST_F(SOValidatorTest, DetectMissingDestPort) {
    auto result = sov::check_configuration(empty_opts);

    ASSERT_FALSE(result.ok);
    EXPECT_THAT(result.errors, ::testing::Contains("No destination-port set"));
}

TEST_F(SOValidatorTest, DetectInvalidDestPort) {
    empty_opts.dest_port = 22;
    auto result = sov::check_configuration(empty_opts);

    ASSERT_FALSE(result.ok);
    EXPECT_THAT(result.errors, ::testing::Contains("Invalid destination-port set"));
}

TEST_F(SOValidatorTest, DetectInvalidSourcePort) {
    empty_opts.source_port = 22;
    auto result = sov::check_configuration(empty_opts);

    ASSERT_FALSE(result.ok);
    EXPECT_THAT(result.errors, ::testing::Contains("Invalid source-port set"));
}

/******************************************************************
 * Testing Default Setting
******************************************************************/


class SODefaultsTest : public ::testing::Test {
protected:
    StreamOptions empty_opts{};

    void SetUp() override {
        sov::check_configuration(empty_opts);
    }
};


TEST_F(SODefaultsTest, SetDefaultPtimes) {
    EXPECT_EQ(empty_opts.ptime_in_packet.value(), 20);
    EXPECT_EQ(empty_opts.ptime_btw_packet.value(), 20);
}

TEST_F(SODefaultsTest, SetDefaultSourcePort) {
    EXPECT_EQ(empty_opts.source_port.value(), 30000);
}

TEST_F(SODefaultsTest, SetDefaultRTPSettings) {
    EXPECT_EQ(empty_opts.start_ssrc.value(), 0x112233);
    EXPECT_EQ(empty_opts.start_timestamp.value(), 0);
    EXPECT_EQ(empty_opts.start_codec.value(), 8);
    EXPECT_EQ(empty_opts.start_seq.value(), 0);
    EXPECT_EQ(empty_opts.seq_steps.value(), 1);
    EXPECT_EQ(empty_opts.start_clockrate.value(), 8000);
}

TEST_F(SODefaultsTest, SetDefaultTimestampStepSize) {
    EXPECT_EQ(empty_opts.timestamp_step_size.value(), 160);
}

