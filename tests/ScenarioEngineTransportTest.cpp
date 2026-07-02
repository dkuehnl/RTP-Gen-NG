//
// Created by dkueh on 25.06.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ScenarioEngine.h"
#include "StreamOptionsFixture.h"

constexpr uint64_t TRIGGER_VALUE = 5;

class ScenarioEngineTransportTest : public ::testing::Test {
protected:
    StreamOptions m_opts{};

    void SetUp() override {
        m_opts = create_minimal_stream_options();
        m_opts.trigger_type = TriggerType::AfterPackets;
        TransportChange transport_change;
        transport_change.trigger_value = TRIGGER_VALUE;
        m_opts.transport_changes.emplace_back(transport_change);
    }
};

TEST_F(ScenarioEngineTransportTest, SetNewDestIp) {
    m_opts.transport_changes[0].new_dest_ip = "8.8.8.8";

    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_dest_ip, m_opts.transport_changes[0].new_dest_ip);
}

TEST_F(ScenarioEngineTransportTest, SetNewDestPort) {
    m_opts.transport_changes[0].new_dest_port = 66880;

    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_dest_port, m_opts.transport_changes[0].new_dest_port);
}

TEST_F(ScenarioEngineTransportTest, SetNewSrcPort) {
    m_opts.transport_changes[0].new_source_port = 5065;
    m_opts.transport_changes[0].use_random_new_source_port = false;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_src_port, m_opts.transport_changes[0].new_source_port);
}

TEST_F(ScenarioEngineTransportTest, SetSrcPortToZeroIfRandom) {
    m_opts.transport_changes[0].use_random_new_source_port = true;
    ScenarioEngine engine(m_opts);

    StreamState state;
    for (uint64_t i = 0; i < TRIGGER_VALUE; i++) {
        state = engine.tick(20);
    }

    EXPECT_EQ(state.current_src_port, 0);
}