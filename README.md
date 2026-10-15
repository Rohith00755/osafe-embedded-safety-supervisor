# O-SAFE — Embedded Safety Supervisor (Prototype)

[![Build and test](https://github.com/Rohith00755/osafe-embedded-safety-supervisor/actions/workflows/build.yml/badge.svg)](https://github.com/Rohith00755/osafe-embedded-safety-supervisor/actions/workflows/build.yml)

O-SAFE is a portfolio prototype exploring fault-aware communication between a QNX-oriented supervisory process and an ESP32 node. It focuses on C++/POSIX sockets, a small message protocol, heartbeat monitoring, diagnostic state transitions, and bench testing.

> **Status:** Host-buildable prototype. It is not a deployed or certified automotive safety system. Vehicle/actuator integration, QNX target validation, timing guarantees, and safety certification are not claimed.

## Architecture

```mermaid
flowchart LR
    E[ESP32 demo node] <-->|TCP / newline records| Q[POSIX C++ supervisor]
    Q --> S[Diagnostic state machine]
    S --> L[Console logs]
```

## Protocol

Send newline-terminated records to TCP port `5000`:

| Record | Purpose |
|---|---|
| `HEARTBEAT` | Node-alive signal |
| `STATUS|link|up` | Status field |
| `ALERT|SENSOR_TIMEOUT|imu` | Node-reported fault |

The prototype supports one client at a time. A heartbeat timeout changes state to `DEGRADED`; an `ALERT` changes state to `FAULT`. These are software diagnostics only and do not imply a safe physical state.

## Build and test on Linux

Requirements: CMake 3.16+, a C++17 compiler, and POSIX sockets.

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/osafe_supervisor
```

The current CTest suite checks protocol parsing for valid heartbeat/status/alert records and rejects empty, malformed, invalid-token, and over-length messages. GitHub Actions runs the build and parser tests on pushes and pull requests. This does **not** yet test the full TCP server's timing, disconnect, or recovery behavior.

Optional port override: `OSAFE_PORT=5001 ./build/osafe_supervisor`.

In another terminal, if `nc` is installed:

```bash
nc 127.0.0.1 5000
HEARTBEAT
STATUS|link|up
ALERT|SENSOR_TIMEOUT|imu
```

## ESP32 demo

`firmware/esp32_tcp_node/esp32_tcp_node.ino` is a minimal Arduino Wi-Fi TCP client. Replace the lab Wi-Fi credentials and supervisor IP before use. Keep it on an isolated trusted lab network. It is not authenticated or encrypted and does not implement CAN, radar, camera, or actuator control.

## Project layout

```text
├── .github/workflows/build.yml
├── README.md
├── CMakeLists.txt
├── include/protocol.hpp
├── src/supervisor.cpp
├── tests/protocol_tests.cpp
├── docs/ARCHITECTURE.md
├── docs/TEST_PLAN.md
├── docs/QNX_PORTING.md
└── firmware/esp32_tcp_node/esp32_tcp_node.ino
```

## Limitations and next milestones

- No vehicle or physical actuator integration.
- No authentication, TLS, multi-client support, or production recovery policy.
- Timeout values are demonstration defaults; no latency or real-time guarantees are claimed.
- QNX compatibility must be built and tested with the actual QNX SDP/BSP toolchain.
- [x] Add protocol parser unit tests and CI build/test workflow.
- [ ] Run the CI workflow and review its result.
- [ ] Exercise TCP timeout, malformed input, fragmentation, and reconnect cases; save actual logs.
- [ ] Bench-test ESP32-to-host communication.
- [ ] Port to the selected QNX target and record compiler/toolchain details.
- [ ] Define requirements and review hazards before considering any safety-critical use.

## Author

Rohith Sathish — ECE student interested in embedded systems, QNX, communication protocols, and edge systems.
