#include "alarm.h"
#include "alarm_logic.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>

static TIM_HandleTypeDef buzzerTimer;

void alarm_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM1_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_8;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    buzzerTimer.Instance = TIM1;
    buzzerTimer.Init.Prescaler = 7;
    buzzerTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    buzzerTimer.Init.Period = 999;
    buzzerTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    buzzerTimer.Init.RepetitionCounter = 0;
    buzzerTimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&buzzerTimer);

    TIM_OC_InitTypeDef output = {0};
    output.OCMode = TIM_OCMODE_PWM1;
    output.Pulse = 500;
    output.OCPolarity = TIM_OCPOLARITY_HIGH;
    output.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    output.OCFastMode = TIM_OCFAST_DISABLE;
    output.OCIdleState = TIM_OCIDLESTATE_RESET;
    output.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&buzzerTimer, &output, TIM_CHANNEL_1);
}

static void buzzer_set(bool enabled)
{
    if (enabled) {
        HAL_TIM_PWM_Start(&buzzerTimer, TIM_CHANNEL_1);
    } else {
        HAL_TIM_PWM_Stop(&buzzerTimer, TIM_CHANNEL_1);
    }
}

void vAlarmTask(void *pvParameters)
{
    (void)pvParameters;

    AlarmState state = AlarmState::NORMAL;
    bool buzzerEnabled = false;
    SensorData reading;

    for (;;) {
        if (xQueueReceive(xQueueSensorToAlarm, &reading, pdMS_TO_TICKS(500)) == pdPASS) {
            AlarmState newState = evaluateTemperature(reading.temperature);
            if (newState != state) {
                state = newState;
                printf("ALARM: temperature %s\r\n", alarmStateName(state));
            }

            bool active = (xEventGroupGetBits(xSystemEvents) & EVENT_ACTIVE) != 0;
            bool shouldEnable = active && state != AlarmState::NORMAL;
            if (shouldEnable != buzzerEnabled) {
                buzzerEnabled = shouldEnable;
                buzzer_set(buzzerEnabled);
                if (buzzerEnabled) {
                    xEventGroupSetBits(xSystemEvents, EVENT_ALARM);
                } else {
                    xEventGroupClearBits(xSystemEvents, EVENT_ALARM);
                }
                printf("ALARM: buzzer %s\r\n", buzzerEnabled ? "on" : "off");
            }
        }

        bool active = (xEventGroupGetBits(xSystemEvents) & EVENT_ACTIVE) != 0;
        if (!active && buzzerEnabled) {
            buzzerEnabled = false;
            buzzer_set(false);
            xEventGroupClearBits(xSystemEvents, EVENT_ALARM);
            printf("ALARM: buzzer off\r\n");
        }
    }
}