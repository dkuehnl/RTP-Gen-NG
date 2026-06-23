//
// Created by dkueh on 23.06.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

constexpr uint64_t TRIGGER_VALUE = 5;

class ScenarioEngineTimestampTest : public ::testing::Test {
protected:
    StreamOptions m_opts{};
    void SetUp() override {
        m_opts = create_timestamp_change();
        m_opts.timestamp_changes[0].trigger_value = TRIGGER_VALUE;
    }
};

TEST_F(ScenarioEngineTimestampTest, ChangeToNewTimestampByEvent) {
    m_opts.timestamp_changes[0].new_timestamp = 260789;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_timestamp, 260789);
}


TEST_F(ScenarioEngineTimestampTest, JumpStepsizeForward) {
    m_opts.timestamp_changes[0].steps_to_jump = 5;
    m_opts.start_timestamp = 0;

    uint32_t expected_end_timestamp =
        m_opts.start_timestamp.value()
        + (TRIGGER_VALUE * m_opts.timestamp_step_size.value())
        + (m_opts.timestamp_changes[0].steps_to_jump.value() * m_opts.timestamp_step_size.value());

    ScenarioEngine engine(m_opts);
    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_timestamp, expected_end_timestamp);
}

TEST_F(ScenarioEngineTimestampTest, ContinueWithNewSeq) {
    m_opts.timestamp_changes[0].continue_seq = false;
    m_opts.timestamp_changes[0].seq_to_continue = 6058;
    m_opts.start_seq = 0;

    ScenarioEngine engine(m_opts);
    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_seq, 6058);
}