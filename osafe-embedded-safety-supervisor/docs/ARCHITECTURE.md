# Architecture

The ESP32 demo sends newline-terminated `HEARTBEAT`, `STATUS|key|value`, or `ALERT|code|detail` records over TCP. The C++ supervisor validates message syntax and maintains diagnostic states.

- `WAITING`: a client session starts; no heartbeat received yet.
- `HEALTHY`: a heartbeat arrived within the timeout.
- `DEGRADED`: heartbeat timeout, disconnection, or local input-buffer issue.
- `FAULT`: the node explicitly reported an alert; latched for the current session.

These states are diagnostic only. A software state does not establish a safe physical condition. The prototype supports one client and has no authentication, encryption, sequence numbers, or rate limiting. TCP is a byte stream, so the server buffers data until newline delimiters arrive.
