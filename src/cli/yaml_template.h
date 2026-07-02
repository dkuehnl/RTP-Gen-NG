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
  destinationIP: 192.168.178.1
  destinationPort: 34000
  sourcePort: 35000
  useTCP: no
streamStartValues:
  ssrc: 0x112233
  timestamp: 160
  timestampStep: 150
  seq: 10
  seqStep: 1
  codec: 9
  startClockrate: 8000
  ptimeInPacket: 20
  ptimeBtwPacket: 20
triggerUnit: Packets
changes:
  - type: ssrcChange
    trigger:
      value: 100
    newSsrc: 0x445566
    continueSeq: no
    seqToContinue: 300
    continueTimestamp: no
    timestampToContinue: 320
)";

#endif //RTPGEN_NG_YAML_TEMPLATE_H