//
// Created by dkueh on 30.04.2026.
//

#ifndef RTPGEN_NG_YAML_TEMPLATE_H
#define RTPGEN_NG_YAML_TEMPLATE_H

inline constexpr const char* YAML_TEMPLATE = R"(
#RTPGen NG - Next-Generation RTP-Stream generation
#With this file it is possible to configure all basic setting and
#the advanced scenario-feature is available here.
#Under the changes-part of the file you can configure as much changes as
#you need to setup a proper edge-case-scenario

connectionDetails:
    destinationIP: ""
    destinationPort: 34000
    sourcePort: ""
    useTCP: no
streamStartValues:
    ssrc: 0x112233
    timestamp: 0
    timestampStep: 150
    seq: 0
    seqStep: 1
changes:
    - ssrcChanges:
        - trigger:
            type: AfterPackets
            value: 100
          newSsrc: 0x445566
    - codecChanges:
        - trigger:
            type: AfterTime
            value: 20
          stepsToJump: 100
)";

#endif //RTPGEN_NG_YAML_TEMPLATE_H