#ifndef INPUT_H
#define INPUT_H

#include "FreeRTOS.h"
#include "task.h"
#include "input_logic.h"

void vInputTask(void *pvParameters);

#endif // INPUT_H