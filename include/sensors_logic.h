#ifndef SENSORS_LOGIC_H
#define SENSORS_LOGIC_H

#include <stdint.h>

constexpr uint16_t kAdcMax = 4095;

int lightPercentFromAdc(uint16_t rawAdc);

#endif