//
// Created by dkueh on 25.06.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

constexpr uint64_t TRIGGER_VALUE = 5;

class ScenarioEngineCodecTest : public ::testing::Test {
protected:
    StreamOptions m_opts{};
    void SetUp() override {
        m_opts = create_codec_change();
        m_opts.codec_changes[0].trigger_value = TRIGGER_VALUE;
    }
};

TEST_F(ScenarioEngineCodecTest, ChangeCodecByEvent) {
    m_opts.codec_changes[0].new_codec = 9;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_codec, m_opts.codec_changes[0].new_codec);
}

TEST_F(ScenarioEngineCodecTest, ChangeClockrate) {
    m_opts.codec_changes[0].new_clockrate = 16000;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_clockrate, m_opts.codec_changes[0].new_clockrate);
}

TEST_F(ScenarioEngineCodecTest, SetNewSSRC) {
    m_opts.codec_changes[0].continue_ssrc = false;
    m_opts.codec_changes[0].ssrc_to_continue = 0x000000;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_ssrc, m_opts.codec_changes[0].ssrc_to_continue);
}

TEST_F(ScenarioEngineCodecTest, SetNewSeq) {
    m_opts.codec_changes[0].continue_seq = false;
    m_opts.codec_changes[0].seq_to_continue = 900;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_seq, m_opts.codec_changes[0].seq_to_continue);
}

TEST_F(ScenarioEngineCodecTest, SetNewTimestamp) {
    m_opts.codec_changes[0].continue_timestamp = false;
    m_opts.codec_changes[0].timestamp_to_continue = 3260078;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_timestamp, m_opts.codec_changes[0].timestamp_to_continue);
}

TEST_F(ScenarioEngineCodecTest, SetAllValuesNewAtOnce) {
    m_opts.codec_changes[0].new_codec = 9;
    m_opts.codec_changes[0].new_clockrate = 16000;
    m_opts.codec_changes[0].continue_ssrc = false;
    m_opts.codec_changes[0].ssrc_to_continue = 0x000000;
    m_opts.codec_changes[0].continue_seq = false;
    m_opts.codec_changes[0].seq_to_continue = 900;
    m_opts.codec_changes[0].continue_timestamp = false;
    m_opts.codec_changes[0].timestamp_to_continue = 3260078;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_codec, m_opts.codec_changes[0].new_codec);
    EXPECT_EQ(state.current_clockrate, m_opts.codec_changes[0].new_clockrate);
    EXPECT_EQ(state.current_ssrc, m_opts.codec_changes[0].ssrc_to_continue);
    EXPECT_EQ(state.current_seq, m_opts.codec_changes[0].seq_to_continue);
    EXPECT_EQ(state.current_timestamp, m_opts.codec_changes[0].timestamp_to_continue);
}