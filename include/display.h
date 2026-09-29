#ifndef DISPLAY_H
#define DISPLAY_H

#include "FreeRTOS.h"
#include "task.h"

/* DisplayTask: the ONLY task that touches the OLED (Section 26).
 * Consumes sensor readings and display-mode selections while remaining
 * the only task that writes to the OLED. */
void vDisplayTask(void *pvParameters);

#endif // DISPLAY_H