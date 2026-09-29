#ifndef SENSORS_H
#define SENSORS_H

#include "FreeRTOS.h"
#include "task.h"

// Data structure representing all environmental sensor readings
struct SensorData {
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};

// Task function for sensor acquisition
void vSensorTask(void *pvParameters);

#endif // SENSORS_H