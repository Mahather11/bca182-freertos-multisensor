#ifndef DISPLAY_H
#define DISPLAY_H

#include "FreeRTOS.h"
#include "task.h"

/* DisplayTask: the ONLY task that touches the OLED (Section 26).
 * Consumes xQueueSensorToDisplay (Part V) and renders the current
 * temperature reading. Mode switching (Temperature/Humidity/Light/
 * Motion) is added in Part VII. */
void vDisplayTask(void *pvParameters);

#endif // DISPLAY_H