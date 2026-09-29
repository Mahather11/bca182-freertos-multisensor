#include "input.h"
#include "rtos_objects.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>

#define ENCODER_CLK_PIN GPIO_PIN_3
#define ENCODER_DT_PIN GPIO_PIN_4

void vInputTask(void *pvParameters)
{
    (void)pvParameters;

    DisplayMode mode = DisplayMode::TEMPERATURE;
    bool previousClockHigh = HAL_GPIO_ReadPin(GPIOA, ENCODER_CLK_PIN) == GPIO_PIN_SET;
    TickType_t lastWakeTime = xTaskGetTickCount();

    printf("InputTask started\r\n");

    for (;;) {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(5));

        bool currentClockHigh = HAL_GPIO_ReadPin(GPIOA, ENCODER_CLK_PIN) == GPIO_PIN_SET;
        bool currentDataHigh = HAL_GPIO_ReadPin(GPIOA, ENCODER_DT_PIN) == GPIO_PIN_SET;
        int8_t step = encoderStep(previousClockHigh, currentClockHigh, currentDataHigh);
        previousClockHigh = currentClockHigh;

        if (step != 0 && (xEventGroupGetBits(xSystemEvents) & EVENT_ACTIVE) != 0) {
            mode = step > 0 ? nextDisplayMode(mode) : previousDisplayMode(mode);
            xQueueSend(xQueueInputToDisplay, &mode, 0);
            const char *modeName = "TEMPERATURE";
            if (mode == DisplayMode::HUMIDITY) modeName = "HUMIDITY";
            if (mode == DisplayMode::LIGHT) modeName = "LIGHT";
            if (mode == DisplayMode::MOTION) modeName = "MOTION";
            printf("INPUT: mode %s\r\n", modeName);
        }
    }
}