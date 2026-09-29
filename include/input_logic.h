#ifndef INPUT_LOGIC_H
#define INPUT_LOGIC_H

#include <stdint.h>

enum class DisplayMode : uint8_t {
    TEMPERATURE = 0,
    HUMIDITY,
    LIGHT,
    MOTION
};

DisplayMode nextDisplayMode(DisplayMode mode);
DisplayMode previousDisplayMode(DisplayMode mode);
int8_t encoderStep(bool previousClockHigh, bool clockHigh, bool dataHigh);

#endif // INPUT_LOGIC_H