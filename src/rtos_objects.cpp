#include "rtos_objects.h"
#include "sensors.h"   // SensorData

QueueHandle_t xQueueSensorToDisplay = NULL;
QueueHandle_t xQueueSensorToAlarm   = NULL;

void initRTOSObjects(void) {
    xQueueSensorToDisplay = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(SensorData));
    xQueueSensorToAlarm   = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(SensorData));

    configASSERT(xQueueSensorToDisplay != NULL);
    configASSERT(xQueueSensorToAlarm   != NULL);
}