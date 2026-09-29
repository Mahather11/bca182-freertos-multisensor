#ifndef MOTION_H
#define MOTION_H

#include "FreeRTOS.h"
#include "task.h"

void motion_init(void);
void vMotionTask(void *pvParameters);

#endif // MOTION_H