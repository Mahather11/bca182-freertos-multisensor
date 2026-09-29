#include "input.h"
#include "rtos_objects.h"
#include "stm32f1xx_hal.h"

#define ENCODER_CLK_PIN GPIO_PIN_3
#define ENCODER_DT_PIN GPIO_PIN_4

static DisplayMode wrapMode(int mode)
{
    if (mode < 0) {
        mode = 3;
    } else if (mode > 3) {
        mode = 0;
    }
    return static_cast<DisplayMode>(mode);
}

DisplayMode nextDisplayMode(DisplayMode mode)
{
    return wrapMode(static_cast<int>(mode) + 1);
}

DisplayMode previousDisplayMode(DisplayMode mode)
{
    return wrapMode(static_cast<int>(mode) - 1);
}

void vInputTask(void *pvParameters)
{
    (void)pvParameters;

    DisplayMode mode = DisplayMode::TEMPERATURE;
    bool previousClockHigh = HAL_GPIO_ReadPin(GPIOA, ENCODER_CLK_PIN) == GPIO_PIN_SET;
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(5));

        bool currentClockHigh = HAL_GPIO_ReadPin(GPIOA, ENCODER_CLK_PIN) == GPIO_PIN_SET;
        bool currentDataHigh = HAL_GPIO_ReadPin(GPIOA, ENCODER_DT_PIN) == GPIO_PIN_SET;
        int8_t step = encoderStep(previousClockHigh, currentClockHigh, currentDataHigh);
        previousClockHigh = currentClockHigh;

        if (step != 0 && (xEventGroupGetBits(xSystemEvents) & EVENT_ACTIVE) != 0) {
            mode = step > 0 ? nextDisplayMode(mode) : previousDisplayMode(mode);
            xQueueSend(xQueueInputToDisplay, &mode, 0);
        }
    }
}