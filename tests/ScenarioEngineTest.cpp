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

//Because the function of the following test is made via template, it
//works exactly the same for every member of the same type. Therefore, only
//one member is tested.
TEST_F(ScenarioEngineTest, CopyVectorInDeque) {
    auto opts = create_full_stream_options();
    ScenarioEngine engine(opts);

    const auto& ssrc_deque = engine.get_ssrc_changes_for();
    EXPECT_EQ(ssrc_deque.size(), 1);
}

//Because the function of the following test is made via template, it
//works exactly the same for every member of the same type. Therefore, only
//one member is tested.
TEST_F(ScenarioEngineTest, TwoEventsInOneDequeAreSorted) {
    auto opts = create_two_ssrc_changes();
    opts.ssrc_changes[0].trigger.value = 10;
    opts.ssrc_changes[1].trigger.value = 5;

    ScenarioEngine engine(opts);

    const auto& ssrc_deque = engine.get_ssrc_changes_for();
    EXPECT_EQ(ssrc_deque.front().trigger.value, 5);
}