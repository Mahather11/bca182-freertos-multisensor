#ifndef ALARM_H
#define ALARM_H

#include "FreeRTOS.h"
#include "task.h"

void alarm_init(void);
void vAlarmTask(void *pvParameters);

#endif // ALARM_H