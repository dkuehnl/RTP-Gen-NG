# RTPGen NG — NextGen RTP Generator

> ⚠️ **Alpha / Pre-release** — Active development. APIs and config schema may change without notice.

A configurable CLI tool for generating RTP and SRTP test traffic over UDP Unicast. Designed specifically for testing edge cases, anomalous stream behavior, and interoperability scenarios that standard traffic generators cannot produce.

---

## Why RTPGen NG?

Most RTP test tools generate well-behaved streams. RTPGen NG generates the streams your system needs to *survive*:

- Sequence number jumps (forward and backward)
- SSRC changes mid-stream with configurable seq/timestamp continuity
- Codec switches without SSRC changes
- Timestamp manipulation (absolute and relative)
- ptime/delta mismatches
- SRTP/RTP context mismatches
- Stream pauses with configurable duration
- Transport-layer redirects mid-stream

---

## Features

- **Event-based scenario engine** — trigger changes after N packets or after N seconds
- **YAML-driven configuration** — define complex multi-event scenarios declaratively
- **Inline editor flow** — invoke without arguments to open a YAML template in `$EDITOR` (Kubernetes-style)
- **Sensible defaults** — only destination IP and port are mandatory; everything else has a fallback
- **Validation with feedback** — warnings for no-op events, errors for invalid config, info for applied defaults

---

## Supported Change Events

| Event            | Trigger       | Description                                              |
|------------------|---------------|----------------------------------------------------------|
| `ssrcChange`     | packets / time | Replace SSRC, optionally reset seq and timestamp        |
| `timestampChange`| packets / time | Absolute jump or relative step manipulation             |
| `codecChange`    | packets / time | Switch payload type and clockrate, with SSRC/seq control|
| `sequenceChange` | packets / time | Jump sequence number forward or backward                |
| `pauseStream`    | packets / time | Stop sending packets for N milliseconds                 |
| `transportChange`| packets / time | Redirect to new destination IP/port or source port      |

---

## Requirements

| Dependency  | Version      |
|-------------|--------------|
| C++         | 20           |
| CMake       | 3.22+        |
| yaml-cpp    | via FetchContent |
| GoogleTest  | via FetchContent (tests only) |

Target runtime: **Linux / UNIX**

---

## Build

```bash
git clone https://github.com/dkuehnl/RTP-Gen-NG.git
cd RTP-Gen-NG
cmake -B build
cmake --build build
```

The binary will be at `build/bin/rtpgen-ng`.

---

## Usage

### With a config file

```bash
rtpgen-ng --file my_scenario.yaml
```

### Interactive editor mode

```bash
rtpgen-ng
```

Opens a YAML template in `$EDITOR`. Edit, save, and close — the stream starts immediately. On parse error, the file is saved to `~/` with an error message.

### Keep config after run

If opened in interactive editor mode or the CLI-args (see -h for more) are used, the config can be saved with:
```bash
rtpgen-ng [...] --keep-config
```

---

## Configuration

### Minimal example

```yaml
connectionDetails:
  destinationIP: "192.168.1.100"
  destinationPort: 5004
```

### Full example with change events

```yaml
connectionDetails:
  destinationIP: "192.168.1.100"
  destinationPort: 5004
  sourcePort: 30000
  useTCP: false

streamStartValues:
  ssrc: 0x112233
  seq: 0
  seqStep: 1
  timestamp: 0
  timestampStep: 160
  codec: 8           # PCMA
  startClockrate: 8000
  ptimeInPacket: 20
  ptimeBtwPacket: 20

changes:
  # Jump sequence number forward by 500 after 100 packets
  - type: sequenceChange
    trigger:
      type: AfterPackets
      value: 100
    seqToJump: 500

  # Pause stream for 2 seconds after 5 seconds of streaming
  - type: pauseStream
    trigger:
      type: AfterSeconds
      value: 5
    msToPause: 2000

  # Change SSRC after 10 seconds, reset seq and timestamp
  - type: ssrcChange
    trigger:
      type: AfterSeconds
      value: 10
    newSsrc: 0xAABBCC
    continueSeq: false
    continueTimestamp: false

  # Switch codec without changing SSRC (intentional edge case)
  - type: codecChange
    trigger:
      type: AfterPackets
      value: 500
    newCodec: 0        # PCMU
    newClockrate: 8000
    continueSsrc: true
    continueSeq: true
    continueTimestamp: true
```

### Defaults applied automatically

| Field              | Default                                  |
|--------------------|------------------------------------------|
| `sourcePort`       | 30000                                    |
| `useTCP`           | false                                    |
| `ptimeInPacket`    | 20 ms                                    |
| `ptimeBtwPacket`   | 20 ms                                    |
| `start_ssrc`       | 0x112233                                 |
| `start_seq`        | 0                                        |
| `seqStep`          | 1                                        |
| `start_timestamp`  | 0                                        |
| `timestampStep`    | (clockrate × ptime) / 1000              |
| `codec`            | 8 (PCMA)                                 |
| `startClockrate`   | 8000 Hz                                  |

---

## Project Structure

```
src/
  core/        # StreamOptions, validation, YAML parser, random generators
  cli/         # CLI entry point (CLI11)
tests/         # GoogleTest/GMock unit tests
external/      # Header-only dependencies (CLI11)
docs/          # Design documents
```

---

## Running Tests

```bash
cmake -B build
cmake --build build
cd build && ctest
```

---

## Status

This project is in active development. The following components are planned or in progress:

- [x] StreamOptions data structures
- [x] YAML config parser
- [x] Validation & default system
- [ ] ScenarioEngine (event scheduling)
- [ ] PacketBuilder (RTP/SRTP packet assembly)
- [ ] Scheduler
- [ ] UDP Sender
- [ ] SRTP support

---

## Contributing

Not open for external contributions at this stage. Issues and feedback welcome via GitHub Issues.

---

## License

Not yet specified.
