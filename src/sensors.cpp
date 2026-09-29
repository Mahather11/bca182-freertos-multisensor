#include "sensors.h"
#include "rtos_objects.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>

extern ADC_HandleTypeDef hadc1;

/* Reads the LDR via ADC1 channel 0 (PA0) and converts the 12-bit raw
 * value to a documented 0-100% scale. This is a relative light level,
 * not calibrated lux. */
static int readLDR(void) {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        uint32_t rawADC = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        return (int)((rawADC / 4095.0f) * 100.0f);
    }
    HAL_ADC_Stop(&hadc1);
    return 0;
}

/* TODO: real DHT22 single-wire protocol on PA1 (dht1:SDA -> bluepill:A1).
 * Placeholder values for now, matching the lab's sample output
 * (Section 20) so the queue pipeline can be verified first. */
static void readDHT22(float *temp, float *humidity) {
    *temp = 25.4f;
    *humidity = 61.2f;
}

void vSensorTask(void *pvParameters) {
    (void)pvParameters;

    SensorData currentReadings;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000); // 2000 ms period

    for (;;) {
        /* 1. Acquire */
        readDHT22(&currentReadings.temperature, &currentReadings.humidity);
        currentReadings.lightLevel = readLDR();
        currentReadings.motionDetected = false;   /* set by MotionTask in Part IX */

        /* 2. Publish to both consumer queues (see rtos_objects.h for why
         * two queues implement the diagram's single fan-out arrow).
         * A short timeout, not portMAX_DELAY: SensorTask must not block
         * indefinitely on a stalled consumer, or its own 2 s period
         * (Section 22) would be violated. */
        if (xQueueSend(xQueueSensorToDisplay, &currentReadings, pdMS_TO_TICKS(50)) != pdPASS) {
            printf("WARNING: display queue full, reading dropped\r\n");
        }
        if (xQueueSend(xQueueSensorToAlarm, &currentReadings, pdMS_TO_TICKS(50)) != pdPASS) {
            printf("WARNING: alarm queue full, reading dropped\r\n");
        }

        /* Absolute periodic delay to prevent cumulative timing drift */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}