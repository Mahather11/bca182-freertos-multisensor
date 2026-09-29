#include "rtos_objects.h"
#include "sensors.h"   // SensorData
#include "input.h"

QueueHandle_t xQueueSensorToDisplay = NULL;
QueueHandle_t xQueueSensorToAlarm   = NULL;
QueueHandle_t xQueueInputToDisplay  = NULL;
EventGroupHandle_t xSystemEvents    = NULL;

void initRTOSObjects(void) {
    xQueueSensorToDisplay = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(SensorData));
    xQueueSensorToAlarm   = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(SensorData));
    xQueueInputToDisplay  = xQueueCreate(DISPLAY_MODE_QUEUE_LENGTH, sizeof(DisplayMode));
    xSystemEvents         = xEventGroupCreate();

    configASSERT(xQueueSensorToDisplay != NULL);
    configASSERT(xQueueSensorToAlarm   != NULL);
    configASSERT(xQueueInputToDisplay  != NULL);
    configASSERT(xSystemEvents          != NULL);
    xEventGroupSetBits(xSystemEvents, EVENT_ACTIVE);
}