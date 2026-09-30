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
| `bluepill_f103c8` | 0 | 0 | 20 | Passed |
| `native` | 0 | 0 | 0 | Passed |

The first firmware run reported 12 low findings. Two project-owned findings
were fixed: the SSD1306 command buffer is now `const`, and the UART retarget
uses `reinterpret_cast` and `static_cast` instead of C-style casts.

The 20 current firmware findings are low severity. Ten are the previously
documented casts inside STM32 CMSIS/HAL register macros and the required
`HAL_UART_MspInit` callback signature. The DHT22 driver's direct GPIO/DWT
register accesses add low-severity macro-cast reports; cppcheck also reports an
unsigned timeout-bound diagnostic on its bounded polling loop. The register
casts are vendor/API constraints, while the timeout loop independently has an
iteration cap and a DWT elapsed-time limit. No high- or medium-severity issue
was reported.

The native environment checks only `src/*_logic.cpp`. STM32-only sources such
as `port.c` are excluded because the custom ARM port intentionally emits a
compile-time error when analyzed as a native host program. Native cppcheck
reported no defects. PlatformIO emitted nonfatal warnings about extracting
toolchain defines during that check; they did not produce findings or affect
the test/build results.