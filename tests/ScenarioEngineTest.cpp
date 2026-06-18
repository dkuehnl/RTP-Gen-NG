//
// Created by dkueh on 31.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

class ScenarioEngineTest : public ::testing::Test {
protected:
    StreamOptions m_opts{};
    std::optional<ScenarioEngine> m_engine;
    void SetUp() override {
        m_opts = create_minimal_stream_options();
        m_engine.emplace(m_opts);
    }
};

TEST_F(ScenarioEngineTest, FillStartValuesIntoStreamState) {

    const auto& state = m_engine->get_state();

    EXPECT_EQ(state.current_ssrc, m_opts.start_ssrc.value());
    EXPECT_EQ(state.current_timestamp, m_opts.start_timestamp.value());
    EXPECT_EQ(state.current_seq, m_opts.start_seq.value());
    EXPECT_EQ(state.current_codec, m_opts.start_codec.value());
    EXPECT_EQ(state.current_clockrate, m_opts.start_clockrate.value());
    EXPECT_EQ(state.trigger_type, m_opts.trigger_type.value());
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
    opts.ssrc_changes[0].trigger_value = 10;
    opts.ssrc_changes[1].trigger_value = 5;

    ScenarioEngine engine(opts);

    const auto& ssrc_deque = engine.get_ssrc_changes_for();
    EXPECT_EQ(ssrc_deque.front().trigger_value, 5);
}

//RTP specifica:
//Seq and Timestamp should increase with every packet.
//This is tested here.
TEST_F(ScenarioEngineTest, NormalTickWithNoChangesIncreaseRTPfields) {
    m_opts.start_seq = 1;
    m_opts.start_timestamp = 0;
    ScenarioEngine engine(m_opts);

    auto state = engine.tick(20);
    EXPECT_EQ(state.current_seq, 2);
    EXPECT_EQ(state.current_timestamp, 160);
}

TEST_F(ScenarioEngineTest, DoubleValuesAfterTwoTicks) {
    m_opts.start_seq = 1;
    m_opts.start_timestamp = 0;

    ScenarioEngine engine(m_opts);
    engine.tick(20);
    auto state = engine.tick(20);

    EXPECT_EQ(state.current_seq, 3);
    EXPECT_EQ(state.current_timestamp, 320);
}

TEST_F(ScenarioEngineTest, IncreaseInternalPacketCountPerTick) {
    auto state = m_engine->tick(20);

    EXPECT_EQ(state.packet_count, 1);
}

TEST_F(ScenarioEngineTest, IncreaseInternalTimeCountPerTick) {
    ScenarioEngine engine(m_opts);
    auto state = m_engine->tick(20);

    EXPECT_EQ(state.elapsed_ms, 20);
}

