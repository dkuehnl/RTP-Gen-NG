# RTPGen NG — Next-Gen RTP Generator

A Linux CLI tool for generating customizable RTP streams with configurable anomalous behavior — built to test RTP stacks, not to play audio. Payload content is irrelevant by design (null-byte payloads are intentional); what matters is precise control over sequence numbers, timestamps, SSRC, source port, codec, and mid-stream transport changes.

Primary use case: paired with **Asterisk** for SIP/RTP testing. Asterisk owns the full SIP
signaling lifecycle; RTPGen NG operates as an autonomous bypass engine, driving the RTP
stream itself — completely bypassing Asterisk's own media stack. Two control-channel modes
are supported: a Unix domain socket (destination is configured up front) or Asterisk's own
Manager Interface (AMI), where RTPGen NG connects directly to Asterisk, authenticates, and
resolves the RTP destination automatically once a call reaches `Up`.
---

## Why RTPGen NG?

Most RTP test tools generate well-behaved streams. RTPGen NG generates the streams your system needs to *survive*:

- Sequence number jumps (forward and backward)
- SSRC changes mid-stream with configurable seq/timestamp continuity
- Codec switches without SSRC changes
- Timestamp manipulation (absolute and relative)
- ptime/delta mismatches
- Stream pauses with configurable duration
- Transport-layer redirects mid-stream (new source port / rebind)

---

## Features

- **Event-based scenario engine** — trigger changes after N packets or after N seconds
- **YAML-driven configuration** — define complex multi-event scenarios declaratively
- **Inline editor flow** — invoke without arguments to open a YAML template in `$EDITOR` (Kubernetes-style)
- **Sensible defaults** — only a control channel is strictly mandatory; everything else (including destination IP/port for the AMI mode) has a fallback or is resolved automatically
- **Validation with feedback** — warnings for no-op events, errors for invalid config, info for applied defaults
- **External control channel** — session start/end/dest-update driven by an external peer (e.g. an Asterisk dialplan), decoupling stream lifecycle from process lifecycle

---

## Supported Change Events

| Event             | Trigger        | Description                                                |
|-------------------|----------------|--------------------------------------------------------------|
| `ssrcChange`      | packets / time | Replace SSRC, optionally reset seq and timestamp             |
| `timestampChange` | packets / time | Absolute jump or relative step manipulation                  |
| `codecChange`     | packets / time | Switch payload type and clockrate, with SSRC/seq control      |
| `sequenceChange`  | packets / time | Jump sequence number forward or backward                     |
| `pauseStream`     | packets / time | Stop sending packets for N milliseconds                      |
| `transportChange` | packets / time | Redirect to new destination IP/port or source port           |

---

## Architecture

```
main.cpp → YAML/CLI parsing → StreamOptions → StreamOptionsValidator
         → RtpEngine (ScenarioEngine + Scheduler + Sender) → IControlChannel
```

| Component | Responsibility |
|---|---|
| `CliParser` | Parses a reduced set of connection-level CLI flags (dest IP/port, source port, input file, verbosity). |
| `YamlParser` | Parses the full scenario configuration (stream start values + change events) from YAML. |
| `StreamOptionsValidator` | Validates and applies defaults to a parsed `StreamOptions`; single point of truth for configuration correctness. |
| `ScenarioEngine` | Advances stream state (sequence, timestamp, SSRC, transport) and consumes scheduled change events. |
| `PacketBuilder` | Builds RTP packets from current `StreamState`. |
| `Scheduler` | Drives the send-loop timing (pacing, pause handling). |
| `Sender` | Owns the UDP socket; sends packets, rebinding on demand without dropping the stream. |
| `IControlChannel` | Strategy interface for external stream control (`start_stream`, `end_stream`, `update_dest`); implementations: Unix domain socket, Asterisk AMI. |
| `RtpEngine` | Wires the above together into a single stream session; owns the session lifecycle. |
| `TemplateEditor` | Interactive YAML config editor, launched when no CLI args are given. |

---

## Requirements

| Dependency | Version           |
|------------|-------------------|
| C++        | 20                |
| CMake      | 3.22+             |
| yaml-cpp   | via FetchContent (MIT) |
| CLI11      | header-only, in `external/` (BSD-3-Clause) |
| GoogleTest | via FetchContent (tests only) |

Target runtime: **Linux / UNIX** (not portable to Windows/macOS by design)

---

## Build

```bash
git clone https://github.com/dkuehnl/RTP-Gen-NG.git
cd RTP-Gen-NG
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target cli_app
```

The binary is built at `build/bin/rtpgen-ng`.

To build and run the test suite instead:

```bash
cmake --build build --target core_tests
ctest --test-dir build
```

> Developed on Windows via CLion with remote SSH deployment to a Linux build host; any Linux toolchain works equally well for a local build.

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

### Connection-level flags only (CLI-only, no scenario/change-events)

```bash
rtpgen-ng --dest-ip 192.168.178.1 --dest-port 34000 --source-port 35000
```

Increase verbosity with repeated `-v` (`-v`, `-vv`, `-vvv`).

### Control channel

Regardless of invocation, the process starts an `IControlChannel` listener and waits for an
external signal before the RTP stream begins. Which implementation runs is selected via
`connectionDetails.controlChannel` (`UNIX` or `AMI`) and cannot be mixed at runtime.

**Unix mode:** listens on a Unix domain socket (`/tmp/rtpgen.sock` by default) for
`start_stream`/`stop_stream` messages. Destination IP/port must already be configured
(`destinationIP`/`destinationPort`); the control channel only drives lifecycle, not the
RTP destination.

**AMI mode:** connects directly to Asterisk's Manager Interface (TCP, typically port 5038),
authenticates with the configured credentials, and subscribes to call events. On `Newstate`
with `ChannelStateDesc: Up`, RTPGen NG resolves the RTP destination itself via
`CHANNEL(rtp,dest)` — no dialplan changes or manual `start_stream` trigger required.
`Hangup`/`DialEnd` end the session. Requires `directmedia=yes` on the relevant PJSIP
endpoint so Asterisk stays out of the media path.

In both modes, `end_stream`/`Hangup` tears the session down; `update_dest` (Unix) /
re-INVITE handling (AMI) is reserved for future dest/codec-update scenarios.

---

## Configuration

### Minimal example (Unix mode)

```yaml
connectionDetails:
  controlChannel: "UNIX"
  destinationIP: "192.168.1.100"
  destinationPort: 5004
```

### Minimal example (AMI mode)

```yaml
connectionDetails:
  controlChannel: "AMI"
  amiHost: "192.168.1.10"
  amiPort: 5038
  amiUser: "rtpgen"
  amiSecret: "changeme"
```

### Full example with change events

```yaml
connectionDetails:
  controlChannel: "UNIX"
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

| Field             | Default                          |
|-------------------|-----------------------------------|
| `sourcePort`      | 30000                             |
| `useTCP`          | false                              |
| `ptimeInPacket`   | 20 ms                              |
| `ptimeBtwPacket`  | 20 ms                              |
| `start_ssrc`      | 0x112233                           |
| `start_seq`       | 0                                   |
| `seqStep`         | 1                                    |
| `start_timestamp` | 0                                    |
| `timestampStep`   | (clockrate × ptime) / 1000          |
| `codec`           | 8 (PCMA)                             |
| `startClockrate`  | 8000 Hz                               |

> `controlChannel` has no default and is always mandatory; a missing or unrecognized value
> is a hard validation error. `destinationIP`/`destinationPort` (Unix) or
> `amiHost`/`amiPort`/`amiUser`/`amiSecret` (AMI) are mandatory depending on the selected mode.
---

## Project Structure

```
src/
  core/        # StreamOptions, validation, YAML/CLI parsing, ScenarioEngine,
               # PacketBuilder, Scheduler, Sender, IControlChannel
  cli/         # CLI entry point (main.cpp, TemplateEditor)
tests/         # GoogleTest/GMock unit tests
external/      # Header-only dependencies (CLI11)
docs/          # Design documents
```

---

## Running Tests

```bash
cmake -B build
cmake --build build --target core_tests
cd build && ctest
```

---

## Status

- [x] StreamOptions data structures
- [x] YAML config parser
- [x] Validation & default system
- [x] ScenarioEngine (event scheduling)
- [x] PacketBuilder (RTP packet assembly)
- [x] Scheduler
- [x] UDP Sender
- [x] RtpEngine (session wiring, lifecycle)
- [x] IControlChannel — Unix socket implementation
- [x] IControlChannel — AMI implementation (Newstate/Hangup triggering, automatic RTP-destination resolution)
- [ ] Re-INVITE destination/codec update handling
- [ ] Multi-stream support (deferred, out of current scope)
- [ ] SRTP-Implementation incl. switch from plain RTP to SRTP

---

## Contributing

Not open for external contributions at this stage. Issues and feedback welcome via GitHub Issues.

---

## License

MIT — see [LICENSE](LICENSE).
