# BCA182 FreeRTOS Multisensor Room Monitor

## Project Overview

This project is a simulated room-monitoring device built for the BCA182
Embedded Systems Programming laboratory. It runs on an STM32F103C8 Blue Pill
using STM32Cube, native FreeRTOS APIs, and Wokwi. The system reads room data,
shows one measurement at a time on an OLED, watches for motion, and sounds a
buzzer when the temperature leaves the normal range.

The firmware is deliberately split into small task and logic modules. Each
module has one job, which makes the system easier to test and easier to explain
during the technical checkoff.

## Features

- Periodic environmental sampling every 2 seconds
- Relative light level reported from 0% to 100%
- OLED pages for temperature, humidity, light, and motion
- KY-040 rotary encoder navigation with wraparound
- Temperature alarm below 18 C or above 30 C
- PIR-based ACTIVE and INACTIVE behavior
- 15-second inactivity timeout
- FreeRTOS queues, event groups, mutex protection, and task priorities
- Native Unity tests for alarm, navigation, and state decisions
- Wokwi-ready STM32 circuit with no Arduino framework

## Learning Objectives

The project demonstrates how a small embedded application can be divided into
cooperating FreeRTOS tasks. It also shows why periodic tasks use
`vTaskDelayUntil()`, why queues are safer than unsynchronized shared values,
and how a mutex protects a shared serial peripheral.

## System Architecture

```mermaid
flowchart LR
		Sensors[DHT22 and LDR] --> SensorTask[SensorTask]
		PIR[PIR sensor] --> MotionTask[MotionTask]
		Encoder[KY-040 encoder] --> InputTask[InputTask]
		SensorTask --> DisplayQueue[Display queue]
		SensorTask --> AlarmQueue[Alarm queue]
		InputTask --> ModeQueue[Mode queue]
		MotionTask --> Events[System event group]
		DisplayQueue --> DisplayTask[DisplayTask]
		ModeQueue --> DisplayTask
		AlarmQueue --> AlarmTask[AlarmTask]
		Events --> StateTask[StateTask]
		DisplayTask --> OLED[SSD1306 OLED]
		AlarmTask --> Buzzer[Buzzer]
```

The OLED is owned by `DisplayTask`; no other task writes to it. SensorTask
publishes the same reading to separate display and alarm queues because a
FreeRTOS queue item is removed by the task that receives it.

## FreeRTOS Architecture

| Task | Priority | Main responsibility | Blocking behavior |
| --- | ---: | --- | --- |
| `InputTask` | 3 | Decode the rotary encoder | 5 ms periodic delay |
| `MotionTask` | 3 | Sample the PIR and update `EVENT_MOTION` | 100 ms periodic delay |
| `SensorTask` | 2 | Read environmental inputs and publish `SensorData` | 2 s periodic delay |
| `AlarmTask` | 2 | Evaluate temperature and control the buzzer | Queue wait with state recheck |
| `StateTask` | 2 | Manage ACTIVE and INACTIVE transitions | Event wait with 15 s timeout |
| `DisplayTask` | 1 | Render the selected OLED page | Sensor queue wait |

The higher priority of the encoder and PIR tasks keeps short-lived input events
responsive. DisplayTask stays at the lowest priority because a human can accept
a small display delay, while sensing and motion handling should not wait behind
a long OLED transfer.

## Hardware / Simulated Components

| Component | Purpose |
| --- | --- |
| STM32 Blue Pill | Main controller |
| DHT22 | Temperature and humidity input |
| Photoresistor module | Relative ambient light input |
| PIR motion sensor | Motion detection |
| KY-040 rotary encoder | OLED page selection |
| SSD1306 OLED | User display |
| Buzzer | Temperature alarm output |

## Pin Configuration

| Pin | Connection |
| --- | --- |
| PA0 | LDR analog output |
| PA1 | DHT22 data line |
| PA2 | PIR output |
| PA3 | Encoder CLK |
| PA4 | Encoder DT |
| PA5 | Encoder switch |
| PA8 | Buzzer, TIM1 channel 1 PWM |
| PA9 | USART1 transmit |
| PA10 | USART1 receive |
| PB6 | OLED SCL |
| PB7 | OLED SDA |

## Task Design

`SensorTask` samples the inputs on a fixed two-second schedule. `InputTask` and
`MotionTask` use shorter fixed schedules because encoder and PIR changes need
faster response. `AlarmTask` consumes sensor readings and treats exactly 18 C
and exactly 30 C as normal. `StateTask` starts the system ACTIVE, enters
INACTIVE after 15 seconds without motion, and returns to ACTIVE when motion is
detected. `DisplayTask` is the only task that updates the OLED.

## Inter-Task Communication

- `xQueueSensorToDisplay` carries `SensorData` to the OLED task.
- `xQueueSensorToAlarm` carries `SensorData` to the alarm task.
- `xQueueInputToDisplay` carries the selected `DisplayMode`.
- `xSystemEvents` contains `EVENT_ACTIVE`, `EVENT_MOTION`, and `EVENT_ALARM`.
- `serialMutex` protects USART1 while task diagnostics are transmitted.

## State Machine

```mermaid
stateDiagram-v2
		[*] --> ACTIVE
		ACTIVE --> ACTIVE: motion detected
		ACTIVE --> INACTIVE: 15 s without motion
		INACTIVE --> ACTIVE: motion detected
		INACTIVE --> INACTIVE: no motion
```

While ACTIVE, the OLED, encoder, sensor sampling, and alarm operate normally.
While INACTIVE, the OLED is blanked, sensor sampling and encoder navigation are
paused, and the PIR remains active so the system can wake up.

## Repository Structure

```text
include/    Module headers and FreeRTOS configuration
src/        STM32 application, drivers, tasks, and pure logic
test/       Native Unity tests
docs/       Architecture, verification, analysis, and development records
diagram.json Wokwi circuit definition
wokwi.toml  Wokwi firmware and ELF paths
platformio.ini PlatformIO environments
```

## Getting Started

Install PlatformIO and the Wokwi extension for VS Code, then open this project
folder. PlatformIO downloads the embedded dependencies during the first build.
The native test toolchain is stored under `E:\PlatformIO` in this workspace so
the C: drive does not run out of space.

## Building the Project

```text
pio run
```

The default environment is `bluepill_f103c8`. The latest verified build passes
with approximately 50.2% of RAM and 51.9% of flash used.

## Running the Wokwi Simulation
<img width="1156" height="828" alt="image" src="https://github.com/user-attachments/assets/5a568e31-f1df-491d-8128-3ea99586ac96" />


Build first, then run **Wokwi: Start Simulator** from VS Code. Keep the Wokwi
Terminal open beside the circuit. The firmware reports a boot sequence similar
to this, although task startup order is scheduler-dependent:

```text
BCA182 FreeRTOS Multisensor
System starting...
ALARM: TIM1 clock 8000000 Hz, PSC=7, ARR=999 -> 1000 Hz PWM
SSD1306 address probe (0x3C): ACK (device found)
SSD1306 I2C errors during init: 0
DISPLAY: OLED initialised
SensorTask started
DisplayTask started
InputTask started
AlarmTask started
MotionTask started
StateTask started
Sample: Temperature: 25.40 C, Humidity: 61.20 %, Light: NN %, Motion: no
```

The DHT22 values shown above depend on the Wokwi sensor control. The light
percentage depends on the Wokwi photoresistor control.

![Wokwi simulation showing the connected Blue Pill, sensors, OLED, encoder, and buzzer]

## Unit Testing

Run the native tests with:

```text
pio test -e native
```

The 26 native tests cover alarm boundaries, display navigation and encoder
edges, DHT22 pulse decoding/checksum/range handling, ADC light scaling, and
ACTIVE/INACTIVE transitions. The latest run passed all 26 tests.

## Static Code Analysis

Run:

```text
pio check -e bluepill_f103c8
pio check -e native
```

The firmware analysis reports no high- or medium-severity findings. The
remaining low-level findings are register-macro casts, the required HAL
callback signature, and one bounded-timeout diagnostic. Native analysis checks
the hardware-independent logic modules and reports no defects. Details are in
[`docs/static-analysis.md`](docs/static-analysis.md).

## Functional Verification

The functional test plan follows FT-01 through FT-10 in the laboratory PDF:
sensor updates, OLED modes, encoder wraparound, PIR behavior, alarm limits,
the inactivity timeout, and reactivation. The supplied Wokwi screenshot shows
the Humidity page and terminal samples such as 25.40 C, 61.25% humidity, and
15% light with no motion.

<img width="1172" height="888" alt="image" src="https://github.com/user-attachments/assets/50eabcc7-6119-48a9-91a3-1b897f1e9093" />
)

Runtime observations should be recorded only after performing each interaction
in Wokwi. The reproducible checklist is in
[`docs/functional-verification.md`](docs/functional-verification.md).

## Engineering Decisions

- Separate queues are used for display and alarm consumers.
- The OLED has one owner, so it does not need a second task-level lock.
- The UART uses a mutex because several tasks write diagnostics.
- Pure alarm, navigation, and state decisions are separated so they can run in
	native tests.
- The custom FreeRTOS port is retained because it is needed for this Wokwi
	simulation setup.

## Limitations

- DHT22 acquisition now uses the PA1 single-wire protocol with checksum validation;
	runtime Wokwi verification is still pending.
- Interactive Wokwi FT results still need to be recorded manually in the test
	plan.
- The Wokwi circuit is not a substitute for testing on physical STM32 hardware.
- The project does not claim calibrated light values; light is a relative
	percentage only.

## Future Improvements

- Run and record the real DHT22 Wokwi verification cases.
- Record the complete FT-01 through FT-10 Wokwi observations.
- Add a finished-system screenshot to the repository’s documentation assets.
- Expand the native tests as new hardware-independent decisions are added.
