#include "sensors.h"
#include "dht22.h"
#include "rtos_objects.h"
#include "sensors_logic.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>

extern ADC_HandleTypeDef hadc1;

/* Reads the LDR via ADC1 channel 0 (PA0) and converts the 12-bit raw
 * value to a documented 0-100% scale. This is a relative light level,
 * not calibrated lux. */
static bool readLDR(int *lightLevel) {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        uint32_t rawADC = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        *lightLevel = lightPercentFromAdc(static_cast<uint16_t>(rawADC));
        return true;
    }
    HAL_ADC_Stop(&hadc1);
    return false;
}

void vSensorTask(void *pvParameters) {
    (void)pvParameters;

    SensorData currentReadings = {0.0f, 0.0f, 0, false};
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000); // 2000 ms period

    printf("SensorTask started\r\n");

    for (;;) {
        if ((xEventGroupGetBits(xSystemEvents) & EVENT_ACTIVE) == 0) {
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
            continue;
        }

        /* 1. Acquire. Invalid DHT22 frames are not published to consumers. */
        DHT22Data dht = DHT22_Read();
        if (dht.status != Dht22Status::OK) {
            printf("DHT22: read failed (%s), sample skipped\r\n",
                   dht22StatusName(dht.status));
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
            continue;
        }
        currentReadings.temperature = dht.temperature;
        currentReadings.humidity = dht.humidity;
        if (!readLDR(&currentReadings.lightLevel)) {
            printf("LDR: ADC conversion failed, previous value retained\r\n");
        }
        currentReadings.motionDetected =
            (xEventGroupGetBits(xSystemEvents) & EVENT_MOTION) != 0;

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

        printf("Sample: Temperature: %.2f C, Humidity: %.2f %%, Light: %d %%, Motion: %s\r\n",
               currentReadings.temperature, currentReadings.humidity,
               currentReadings.lightLevel,
               currentReadings.motionDetected ? "yes" : "no");

        /* Absolute periodic delay to prevent cumulative timing drift */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}