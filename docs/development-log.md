# Development Log

The repository was developed through small, reviewable milestones. Each major
laboratory part was built and pushed separately so the Git history records the
engineering progression.

| Commit | Milestone |
| --- | --- |
| `e8afd53` | Initial repository |
| `2b82b4e` | Initialize STM32 PlatformIO project |
| `64b5ea2` | Add initial FreeRTOS tasks |
| `27cd1de` | Verify dual-task scheduling |
| `f3c7a78` | Add sensor subsystem |
| `1006617` | Add sensor data communication |
| `6e4c81c` | Implement OLED display task |
| `48c3491` | Add rotary encoder navigation |
| `a957c4c` | Implement alarm task |
| `de32416` | Add PIR motion monitoring |
| `df15f17` | Add system state machine |
| `a7572c0` | Add FreeRTOS event group |
| `2eeac08` | Protect serial output with mutex |
| `6c5042b` | Document task priority rationale |
| `d828515` | Document modular software design |
| `3f746e8` | Add 13 alarm, navigation, and state unit tests |
| `055ce22` | Resolve static-analysis findings |
| `f4a08d0` | Complete Wokwi circuit verification documentation |
| `520db9d` | Add expected serial diagnostics |
| `fe1603d` | Align boot diagnostic order |
| `e6154c8` | Document FreeRTOS fault experiments |

The current verification commands are:

```text
pio run
pio test -e native
pio check -e bluepill_f103c8
pio check -e native
```

The firmware and native tests are kept as separate environments: the default
build targets the Blue Pill, while the native environment compiles only the
hardware-independent logic and Unity tests.