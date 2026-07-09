//
// Created by dkueh on 25.06.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

constexpr uint64_t TRIGGER_VALUE = 5;

class ScenarioEngineSequenceTest : public ::testing::Test {
protected:
    StreamOptions m_opts{};

    void SetUp() override {
        m_opts = create_minimal_unix_stream_options();
        m_opts.trigger_type = TriggerType::AfterPackets;

        SequenceChange seq_change;
        seq_change.trigger_value = TRIGGER_VALUE;
        m_opts.sequence_changes.emplace_back(seq_change);
    }
};

TEST_F(ScenarioEngineSequenceTest, JumpSeqInPosWay) {
    m_opts.sequence_changes[0].seq_to_jump = 10;

    uint16_t expected_end_seq =
        m_opts.start_seq.value() + TRIGGER_VALUE + m_opts.sequence_changes[0].seq_to_jump.value();

    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_seq, expected_end_seq);
}

TEST_F(ScenarioEngineSequenceTest, JumpSeqInNegWay) {
    m_opts.sequence_changes[0].seq_to_jump = -10;

    uint16_t expected_end_seq =
        m_opts.start_seq.value() + TRIGGER_VALUE + m_opts.sequence_changes[0].seq_to_jump.value();

    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_seq, expected_end_seq);
}