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

| File / line | Finding / cause | Resolution |
| --- | --- | --- |
| `src/dht22.cpp:33, 39, 63, 69, 80, 81, 82, 84, 87` | Low style C-style pointer casts reported through CMSIS GPIO/DWT register macros | Accepted vendor/register diagnostics; no project cast is being suppressed |
| `src/dht22.cpp:39` | Low unsigned-bound diagnostic on the timeout check | Retained; the loop has both an iteration bound and DWT elapsed-time bound |
| `src/main.cpp:41, 53, 54, 55, 56, 57, 66, 67, 198` | Low style casts in FreeRTOS/CMSIS register macros and vector-table setup | Accepted framework/register access patterns |
| `src/main.cpp:150` | Low constParameterPointer suggestion for `HAL_UART_MspInit` | HAL callback signature is framework-defined and retained |

The native environment checks only `src/*_logic.cpp`. STM32-only sources such
as `port.c` are excluded because the custom ARM port intentionally emits a
compile-time error when analyzed as a native host program. Native cppcheck
reported no defects. PlatformIO emitted nonfatal warnings about extracting
toolchain defines during that check; they did not produce findings or affect
the test/build results.