#include "sensors_logic.h"

int lightPercentFromAdc(uint16_t rawAdc)
{
    uint32_t clamped = rawAdc > kAdcMax ? kAdcMax : rawAdc;
    uint32_t brightness = kAdcMax - clamped;
    return static_cast<int>((brightness * 100U + kAdcMax / 2U) / kAdcMax);
}