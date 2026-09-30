# Deliberate FreeRTOS Fault Experiments

These experiments are temporary. Apply one change at a time, observe the
Wokwi Terminal and OLED, record the result, and restore the implementation
before continuing. The submitted firmware must retain the normal blocking and
priority behavior.

## Experiment 1: Remove Blocking

Temporarily replace the `vTaskDelayUntil()` call in `InputTask` with a loop
that performs no delay.

Expected observation:

- `InputTask` remains Ready/Running continuously.
- CPU usage increases and lower-priority `DisplayTask` work becomes less
  responsive.
- Encoder processing can dominate scheduling and may delay OLED updates.

Restore the 5 ms `vTaskDelayUntil()` call afterward. A continuously executing
task must perform finite work and block before its next iteration.

## Experiment 2: Raise a Task Priority

Temporarily change `DisplayTask` from priority 1 to priority 4 while it is
performing OLED updates.

Expected observation:

- The display task can delay priority-2 sensing/alarm work during its I2C
  transfer.
- Sensor sample timing and alarm response become less predictable.
- The task priority is higher than required for human-visible display work.

Restore `DisplayTask` to priority 1. Input and motion remain at priority 3
because their events can occur on a millisecond timescale; sensing, alarm, and
state work remain at priority 2.

## Experiment 3: Remove the Serial Mutex

Temporarily remove the `serialMutex` take/give around the UART transmit in
`_write()`.

Expected observation:

- Diagnostics from multiple tasks can interleave or appear as damaged lines.
- Concurrent `HAL_UART_Transmit()` calls can contend for the shared UART
  handle.
- The OLED and sensor behavior should remain logically independent, but the
  terminal log becomes unreliable.

Restore the mutex protection afterward. The shared resource is USART1 and its
HAL handle; the competing users are the task diagnostics and startup logging.

## Observation Record

Do not mark an experiment complete based only on the expected behavior above.
Run each temporary variant in Wokwi, capture the actual terminal/OLED behavior,
restore the normal firmware, and record the commit used for the run.

| Experiment | Actual observation | Result | Date / firmware commit |
| --- | --- | --- | --- |
| Remove InputTask blocking | Pending Wokwi observation | Pending | Pending |
| Raise DisplayTask priority | Pending Wokwi observation | Pending | Pending |
| Remove serial mutex | Pending Wokwi observation | Pending | Pending |

## Restoration Check

After every experiment, restore the source and run:

```text
pio run
pio test -e native
git diff --check
```

Only the restored implementation should be committed.