# Task Priority Rationale

The priorities express scheduling urgency, not feature importance. Every task
performs finite work and then blocks, so the higher-priority tasks do not
starve lower-priority work.

| Task | Priority | Reason |
| --- | ---: | --- |
| InputTask | 3 | Encoder edges can arrive milliseconds apart, so prompt polling prevents missed navigation steps. |
| MotionTask | 3 | PIR activity must be observed promptly to refresh the inactivity timer and wake the system. |
| SensorTask | 2 | Sensor sampling is periodic at 2 seconds and can tolerate short scheduling latency. |
| AlarmTask | 2 | Temperature alarms should react promptly to each sensor update, but do not require edge-level latency. |
| StateTask | 2 | The state machine tracks a 15-second timeout and motion events, so millisecond-level priority is unnecessary. |
| DisplayTask | 1 | OLED transfers are comparatively slow and user-visible latency is acceptable; keeping this task lowest prevents it delaying sensing or input. |

`InputTask`, `MotionTask`, and `SensorTask` use `vTaskDelayUntil()` for
deterministic periodic execution. A high-priority task that never blocks would
consume the CPU and delay lower-priority tasks, so each task returns to the
Blocked state after its finite work.