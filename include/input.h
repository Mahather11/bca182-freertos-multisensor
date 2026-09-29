#ifndef INPUT_H
#define INPUT_H

#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

enum class DisplayMode : uint8_t {
    TEMPERATURE = 0,
    HUMIDITY,
    LIGHT,
    MOTION
};

DisplayMode nextDisplayMode(DisplayMode mode);
DisplayMode previousDisplayMode(DisplayMode mode);
int8_t encoderStep(bool previousClockHigh, bool clockHigh, bool dataHigh);
void vInputTask(void *pvParameters);

#endif // INPUT_H