//
// Created by dkueh on 23.06.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

constexpr uint64_t TRIGGER_VALUE = 5;

class ScenarioEngineSSRCTest : public ::testing::Test {
protected:
    StreamOptions m_opts{};
    void SetUp() override {
        m_opts = create_ssrc_change();
        m_opts.ssrc_changes[0].trigger_value = TRIGGER_VALUE;
    }
};

TEST_F(ScenarioEngineSSRCTest, ChangeSSRCByEvent) {
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_ssrc, m_opts.ssrc_changes[0].new_ssrc);
}

TEST_F(ScenarioEngineSSRCTest, ContinueWithNewSeq) {
    m_opts.ssrc_changes[0].continue_seq = false;
    m_opts.ssrc_changes[0].seq_to_continue = 500;
    m_opts.start_seq = 1;

    ScenarioEngine engine(m_opts);
    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_seq, 500);
}

TEST_F(ScenarioEngineSSRCTest, ContinueWithNewTimestamp) {
    m_opts.ssrc_changes[0].continue_timestamp = false;
    m_opts.ssrc_changes[0].timestamp_to_continue = 151680;
    m_opts.start_timestamp = 0;

    ScenarioEngine engine(m_opts);
    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_timestamp, 151680);
}