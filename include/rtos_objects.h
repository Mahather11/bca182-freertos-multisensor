#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

// Initialize shared FreeRTOS queues/semaphores
void initRTOSObjects(void);

#endif // RTOS_OBJECTS_H