#ifndef DHT22_H
#define DHT22_H

#include <stdbool.h>

struct DHT22Data {
    float temperature;
    float humidity;
    bool valid;
};

void DHT22_Init(void);
DHT22Data DHT22_Read(void);

#endif