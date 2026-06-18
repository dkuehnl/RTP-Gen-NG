//
// Created by dkueh on 31.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

class ScenarioEngineTest : public ::testing::Test {
protected:
    StreamOptions m_opts;

    void SetUp() override {
        m_opts = create_full_stream_options();
    }
};

TEST_F(ScenarioEngineTest, FillStartValuesIntoStreamState) {
    ScenarioEngine engine(m_opts);

    const auto& state = engine.get_state();

    EXPECT_EQ(state.current_ssrc, m_opts.start_ssrc.value());
    EXPECT_EQ(state.current_timestamp, m_opts.start_timestamp.value());
    EXPECT_EQ(state.current_seq, m_opts.start_seq.value());
    EXPECT_EQ(state.current_codec, m_opts.start_codec.value());
    EXPECT_EQ(state.current_clockrate, m_opts.start_clockrate.value());
}
/*
TEST_F(ScenarioEngineTest, FillChangeEventIntoDeque) {
    ScenarioEngine engine(m_opts);

    EXPECT_EQ(engine.m_ssrc_changes.size(), 1);
}*/