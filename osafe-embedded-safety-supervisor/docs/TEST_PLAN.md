# Bench test plan

Record date, host/target, compiler version, commit, observed output, pass/fail, and log evidence. Do not claim a test passed until it has been run.

1. Build with CMake and review compiler warnings.
2. Send `HEARTBEAT`; expect a `HEALTHY` state log.
3. Send `STATUS|link|up`; expect a status log.
4. Send `ALERT|SENSOR_TIMEOUT|imu`; expect `FAULT`.
5. Connect and send no heartbeat for more than five seconds; expect `DEGRADED`.
6. Send `HELLO`; expect a protocol rejection.
7. Send multiple newline-separated records and split one record across multiple writes.
8. Test oversized input, disconnect mid-record, repeated reconnects, and malformed delimiters.

Fault-injection results and screenshots should be committed only after the tests are actually performed.
