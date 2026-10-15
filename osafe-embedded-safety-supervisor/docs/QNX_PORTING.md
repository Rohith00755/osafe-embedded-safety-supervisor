# QNX porting checklist

The code uses common POSIX socket calls, but compatibility with a specific QNX release/target is not yet verified.

1. Record the QNX SDP version, target architecture, and Raspberry Pi 5 BSP/toolchain.
2. Build with the actual QNX cross-compiler and supported build system.
3. Verify socket/select/signal behavior and network configuration on the target image.
4. Run the complete protocol and fault-injection test plan on target.
5. Measure timeout jitter and scheduling under load; host behavior is not a real-time guarantee.
6. Keep physical outputs disabled until requirements, hazard analysis, and independent review exist.

QNX deployment is a future milestone, not a completed feature.
