#ifndef DHT22_H
#define DHT22_H

#include <stdbool.h>
#include "dht22_logic.h"

struct DHT22Data {
    float temperature;
    float humidity;
    Dht22Status status;
};

void DHT22_Init(void);
DHT22Data DHT22_Read(void);

#endif