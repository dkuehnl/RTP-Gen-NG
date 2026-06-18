//
// Created by dkueh on 31.05.2026.
//

#ifndef RTPGEN_NG_STREAMOPTIONSFIXTURE_H
#define RTPGEN_NG_STREAMOPTIONSFIXTURE_H

#include "StreamOptions.h"

/**
 * @brief Creates a fully populated StreamOptions for testing.
 * Contains all fields set and one example entry per change-event type.
 */
inline StreamOptions create_full_stream_options() {
    StreamOptions opts;

    // Connection details
    opts.dest_ip = "192.168.1.100";
    opts.dest_port = 5004;
    opts.source_port = 30001;
    opts.use_tcp = false;

    // Packetization timing
    opts.ptime_in_packet = 20;      // 20ms declared in packet
    opts.ptime_btw_packet = 20;     // 20ms actual send interval

    // Stream start values
    opts.start_ssrc = 0xDEADBEEF;
    opts.start_timestamp = 1000;
    opts.timestamp_step_size = 160;  // 20ms @ 8kHz
    opts.start_codec = 8;            // PCMA
    opts.start_clockrate = 8000;
    opts.start_seq = 42;
    opts.seq_steps = 1;

    // ========== CHANGE EVENTS ==========

    // SSRCChange: after 100 packets
    SSRCChange ssrc_evt;
    ssrc_evt.trigger = {TriggerType::AfterPackets, 100};
    ssrc_evt.new_ssrc = 0xCAFEBABE;
    ssrc_evt.continue_seq = true;        // Seq continues
    ssrc_evt.continue_timestamp = false; // Timestamp restarts
    ssrc_evt.timestamp_to_continue = 5000;
    opts.ssrc_changes.push_back(ssrc_evt);

    // SequenceChange: after 50 packets, jump -10 backwards
    SequenceChange seq_evt;
    seq_evt.trigger = {TriggerType::AfterPackets, 50};
    seq_evt.seq_to_jump = -10;
    opts.sequence_changes.push_back(seq_evt);

    // TimestampChange: after 5000ms, jump to absolute 10000
    TimestampChange ts_evt;
    ts_evt.trigger = {TriggerType::AfterSeconds, 5000};
    ts_evt.new_timestamp = 10000;
    ts_evt.continue_seq = false;  // Seq also changes
    ts_evt.seq_to_continue = 100;
    opts.timestamp_changes.push_back(ts_evt);

    // CodecChange: after 200 packets, switch to codec 0 (PCMU) with 8kHz
    CodecChange codec_evt;
    codec_evt.trigger = {TriggerType::AfterPackets, 200};
    codec_evt.new_codec = 0;       // PCMU
    codec_evt.new_clockrate = 8000;
    codec_evt.continue_ssrc = false;        // SSRC changes
    codec_evt.ssrc_to_continue = 0x11223344;
    codec_evt.continue_seq = false;         // Seq restarts
    codec_evt.seq_to_continue = 0;
    codec_evt.continue_timestamp = false;   // Timestamp restarts
    codec_evt.timestamp_to_continue = 0;
    opts.codec_changes.push_back(codec_evt);

    // PauseStream: after 3 seconds, pause for 500ms
    PauseStream pause_evt;
    pause_evt.trigger = {TriggerType::AfterSeconds, 3000};
    pause_evt.ms_to_pause = 500;
    opts.pause_stream.push_back(pause_evt);

    // TransportChange: after 150 packets, redirect to different host/port
    TransportChange transport_evt;
    transport_evt.trigger = {TriggerType::AfterPackets, 150};
    transport_evt.new_dest_ip = "192.168.1.200";
    transport_evt.new_dest_port = 6005;
    transport_evt.use_random_new_source_port = false;
    transport_evt.new_source_port = 31001;
    opts.transport_changes.push_back(transport_evt);

    return opts;
}

/**
 * @brief Creates a minimal StreamOptions (only mandatory fields).
 * Useful for testing default-value application.
 */
inline StreamOptions create_minimal_stream_options() {
    StreamOptions opts;
    opts.dest_ip = "192.168.1.100";
    opts.dest_port = 5004;
    opts.source_port = 30001;
    opts.use_tcp = false;

    // Packetization timing
    opts.ptime_in_packet = 20;      // 20ms declared in packet
    opts.ptime_btw_packet = 20;     // 20ms actual send interval

    // Stream start values
    opts.start_ssrc = 0xDEADBEEF;
    opts.start_timestamp = 1000;
    opts.timestamp_step_size = 160;  // 20ms @ 8kHz
    opts.start_codec = 8;            // PCMA
    opts.start_clockrate = 8000;
    opts.start_seq = 42;
    opts.seq_steps = 1;

    return opts;
}

/**
 * @brief Creates a StreamOptions with only SSRC changes for focused testing.
 */
inline StreamOptions create_two_ssrc_changes() {
    auto opts = create_minimal_stream_options();

    SSRCChange evt1;
    evt1.trigger = {TriggerType::AfterPackets, 10};
    evt1.new_ssrc = 0x11111111;
    opts.ssrc_changes.push_back(evt1);

    SSRCChange evt2;
    evt2.trigger = {TriggerType::AfterPackets, 50};
    evt2.new_ssrc = 0x22222222;
    opts.ssrc_changes.push_back(evt2);

    return opts;
}

/**
 * @brief Creates a StreamOptions with time-based triggers for testing AfterSeconds.
 */
inline StreamOptions create_time_based_triggers() {
    auto opts = create_minimal_stream_options();

    // Pause after 1 second
    PauseStream pause_evt;
    pause_evt.trigger = {TriggerType::AfterSeconds, 1000};
    pause_evt.ms_to_pause = 200;
    opts.pause_stream.push_back(pause_evt);

    // Codec change after 2 seconds
    CodecChange codec_evt;
    codec_evt.trigger = {TriggerType::AfterSeconds, 2000};
    codec_evt.new_codec = 9;
    codec_evt.new_clockrate = 8000;
    opts.codec_changes.push_back(codec_evt);

    return opts;
}

#endif //RTPGEN_NG_STREAMOPTIONSFIXTURE_H