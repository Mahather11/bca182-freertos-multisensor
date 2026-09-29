# Modular Software Architecture

The firmware is split by responsibility. Hardware-facing tasks own their
peripherals, while decision functions remain independent of HAL and FreeRTOS
where practical.

| Module | Responsibility | Owned resource or interface |
| --- | --- | --- |
| `sensors` | Periodic DHT22/LDR acquisition and publication | ADC1 and sensor queues |
| `display` | Render the selected measurement | SSD1306 and display queue inputs |
| `input` | Decode KY-040 rotation and select a display mode | Encoder GPIO and mode queue |
| `alarm` | Evaluate temperature and drive the buzzer | TIM1 PWM and alarm queue |
| `motion` | Sample the PIR and publish the current motion level | PIR GPIO and `EVENT_MOTION` |
| `system_state` | Apply the 15-second inactivity state machine | `EVENT_ACTIVE` |
| `rtos_objects` | Create and expose shared FreeRTOS objects | Queues, event group, and serial mutex |
| `ssd1306` / `wokwi_i2c` | OLED framebuffer and I2C transport | SSD1306 bus operations |

`main.cpp` is the startup composition root. It initializes the MCU and
peripherals, creates the RTOS objects, creates tasks with explicit priorities,
and starts the scheduler. Application behavior stays in the task modules.

The OLED has a single owner: `DisplayTask`. Sensor data uses separate queues
for the display and alarm consumers because receiving an item removes it from a
FreeRTOS queue. The UART is shared by task diagnostics and is protected by
`serialMutex` at the transmit boundary.

Hardware-independent decisions are isolated in `alarm_logic.cpp`,
`input_logic.cpp`, and `system_state_logic.cpp`. These functions can be tested
without starting the STM32 peripherals or the FreeRTOS scheduler.