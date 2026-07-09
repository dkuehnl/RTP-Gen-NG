//
// Created by dkueh on 25.06.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

constexpr uint64_t TRIGGER_VALUE = 5;

class ScenarioEnginePauseTest : public ::testing::Test {
protected:
    StreamOptions m_opts{};

    void SetUp() override {
        m_opts = create_minimal_unix_stream_options();
        m_opts.trigger_type = TriggerType::AfterPackets;
        PauseStream pause_stream;
        pause_stream.trigger_value = TRIGGER_VALUE;
        m_opts.pause_stream.emplace_back(pause_stream);
    }
};

TEST_F(ScenarioEnginePauseTest, SetStreamStateToPause) {
    m_opts.pause_stream[0].ms_to_pause = 40;

    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_TRUE(state.is_paused);
    EXPECT_EQ(state.ms_to_pause, m_opts.pause_stream[0].ms_to_pause);
}

TEST_F(ScenarioEnginePauseTest, ResetPauseStateAfterEvent) {
    m_opts.pause_stream[0].ms_to_pause = 40;

    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < (TRIGGER_VALUE + 1); i++) {
        state = engine.tick(20);
    }

    EXPECT_FALSE(state.is_paused);
    EXPECT_EQ(state.ms_to_pause, 0);
}

TEST_F(ScenarioEnginePauseTest, AddPausedTimeToElapsedTimeAfterEvent) {
    m_opts.pause_stream[0].ms_to_pause = 40;

    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < (TRIGGER_VALUE + 1); i++) {
        state = engine.tick(20);
    }
    uint64_t expected_elapsed_time = 20 * (TRIGGER_VALUE + 1) + m_opts.pause_stream[0].ms_to_pause.value();

    EXPECT_EQ(state.elapsed_ms, expected_elapsed_time);
}