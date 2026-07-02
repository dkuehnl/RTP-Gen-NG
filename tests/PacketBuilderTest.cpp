//
// Created by dkueh on 25.06.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "PacketBuilder.h"
#include "ScenarioEngine.h"

StreamState make_test_state() {
    StreamState s{};
    s.current_ssrc = 0x112233;
    s.current_seq = 1;
    s.current_timestamp = 160;
    s.current_codec = 8;
    s.current_clockrate = 8000;
    s.current_ptime_in_packet = 20;
    return s;
}

class PacketBuilderTest : public ::testing::Test {
protected:
    StreamState m_state = make_test_state();
};

TEST_F(PacketBuilderTest, PacketHasCorrectTotalSize) {
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet.size(), 172);
}

TEST_F(PacketBuilderTest, VersionBitsAreSet) {
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet[0], 0x80);
}

TEST_F(PacketBuilderTest, PayloadTypeIsSet) {
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet[1], 8);
}

TEST_F(PacketBuilderTest, SequenceNumberBigEndian) {
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet[2], 0x00);
    EXPECT_EQ(packet[3], 0x01);
}

TEST_F(PacketBuilderTest, TimestampIsBigEndian) {
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet[4], 0x00);
    EXPECT_EQ(packet[5], 0x00);
    EXPECT_EQ(packet[6], 0x00);
    EXPECT_EQ(packet[7], 0xA0);
}

TEST_F(PacketBuilderTest, SSRCIsBigEndian) {
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet[8],  0x00);
    EXPECT_EQ(packet[9],  0x11);
    EXPECT_EQ(packet[10], 0x22);
    EXPECT_EQ(packet[11], 0x33);
}

TEST_F(PacketBuilderTest, PayloadIsNullbytes) {
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    for (size_t i = 12; i < packet.size(); i++) {
        EXPECT_EQ(packet[i], 0x00);
    }
}

TEST_F(PacketBuilderTest, PayloadSizeScalesWithPtime) {
    m_state.current_ptime_in_packet = 40; // 40ms @ 8kHz = 320 Bytes
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet.size(), 12 + 320);
}

TEST_F(PacketBuilderTest, PayloadSizeScalesWithClockrate) {
    m_state.current_clockrate = 16000; // 20ms @ 16kHz = 320 Bytes
    auto packet = rtp::builder::generate_rtp_packet(m_state);
    EXPECT_EQ(packet.size(), 12 + 320);
}