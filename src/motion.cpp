#include "motion.h"
#include "rtos_objects.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>

void motion_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);
}

void vMotionTask(void *pvParameters)
{
    (void)pvParameters;

    bool previousDetected = false;
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100));

        bool detected = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_SET;
        if (detected) {
            xEventGroupSetBits(xSystemEvents, EVENT_MOTION);
        } else {
            xEventGroupClearBits(xSystemEvents, EVENT_MOTION);
        }

        if (detected != previousDetected) {
            printf("MOTION: %s\r\n", detected ? "detected" : "clear");
            previousDetected = detected;
        }
    }
}