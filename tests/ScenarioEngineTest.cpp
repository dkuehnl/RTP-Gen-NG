//
// Created by dkueh on 31.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

class ScenarioEngineTest : public ::testing::Test {
protected:

};

TEST_F(ScenarioEngineTest, FillStartValuesIntoStreamState) {
    auto opts = create_minimal_stream_options();
    ScenarioEngine engine(opts);

    const auto& state = engine.get_state();

    EXPECT_EQ(state.current_ssrc, opts.start_ssrc.value());
    EXPECT_EQ(state.current_timestamp, opts.start_timestamp.value());
    EXPECT_EQ(state.current_seq, opts.start_seq.value());
    EXPECT_EQ(state.current_codec, opts.start_codec.value());
    EXPECT_EQ(state.current_clockrate, opts.start_clockrate.value());
}
/*
TEST_F(ScenarioEngineTest, FillChangeEventIntoDeque) {
    ScenarioEngine engine(m_opts);

    EXPECT_EQ(engine.m_ssrc_changes.size(), 1);
}*/