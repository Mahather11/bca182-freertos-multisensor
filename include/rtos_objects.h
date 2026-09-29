#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* Sensor data queues (Part V).
 *
 * A native FreeRTOS queue is single-reader: an item read by one task is
 * consumed and will not be seen by a second task. The lab's Section 25
 * diagram shows one conceptual "Sensor Queue" fanning out to DisplayTask
 * and AlarmTask; to implement that fan-out with two independent readers,
 * SensorTask writes the same reading into two separate queue instances.
 *
 * Depth 5: SensorTask produces one item every 2 s (Section 22). A queue
 * depth of 5 tolerates a consumer lagging up to ~10 s behind before
 * xQueueSend() would start blocking/dropping, which is generous headroom
 * for a task that is otherwise only doing OLED/buzzer work.
 */
#define SENSOR_QUEUE_LENGTH   5
#define DISPLAY_MODE_QUEUE_LENGTH 4

#define EVENT_ACTIVE  (1U << 0)
#define EVENT_MOTION  (1U << 1)
#define EVENT_ALARM   (1U << 2)

extern QueueHandle_t xQueueSensorToDisplay;
extern QueueHandle_t xQueueSensorToAlarm;
extern QueueHandle_t xQueueInputToDisplay;
extern EventGroupHandle_t xSystemEvents;

// Initialize shared FreeRTOS queues/semaphores
void initRTOSObjects(void);

#endif // RTOS_OBJECTS_H