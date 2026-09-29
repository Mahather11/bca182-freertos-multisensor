# Static Analysis

Static analysis was run with PlatformIO cppcheck after the native toolchain
and STM32 platform were moved to `E:\PlatformIO`.

Commands:

```text
pio check -e bluepill_f103c8
pio check -e native
```

## Results

| Environment | High | Medium | Low | Result |
| --- | ---: | ---: | ---: | --- |
| `bluepill_f103c8` | 0 | 0 | 10 | Passed |
| `native` | 0 | 0 | 0 | Passed |

The first firmware run reported 12 low findings. Two project-owned findings
were fixed: the SSD1306 command buffer is now `const`, and the UART retarget
uses `reinterpret_cast` and `static_cast` instead of C-style casts.

The remaining 10 firmware findings are low-severity C-style casts inside STM32
CMSIS register macros such as `TIM4`, `SCB`, and `I2C1`, plus the non-const
parameter required by the STM32 HAL `HAL_UART_MspInit` callback signature.
These are vendor/API constraints, not application casts, so changing them would
require modifying vendor headers or breaking the HAL callback contract.

The native environment checks only `src/*_logic.cpp`. STM32-only sources such
as `port.c` are excluded because the custom ARM port intentionally emits a
compile-time error when analyzed as a native host program. Native cppcheck
reported no defects. PlatformIO emitted nonfatal warnings about extracting
toolchain defines during that check; they did not produce findings or affect
the test/build results.