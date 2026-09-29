#include "sensors.h"
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
 * (Section 20) so SensorTask and the pipeline can be verified first. */
static void readDHT22(float *temp, float *humidity) {
    *temp = 25.4f;
    *humidity = 61.2f;
}

void vSensorTask(void *pvParameters) {
    (void)pvParameters;

    SensorData currentReadings;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000);

    for (;;) {
        readDHT22(&currentReadings.temperature, &currentReadings.humidity);
        currentReadings.lightLevel = readLDR();

        printf("Temperature: %.2f C\r\n", currentReadings.temperature);
        printf("Humidity: %.2f %%\r\n", currentReadings.humidity);
        printf("Light Level: %d %%\r\n", currentReadings.lightLevel);
        printf("-----------------------------\r\n");

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}