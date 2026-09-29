#include "input.h"

int8_t encoderStep(bool previousClockHigh, bool clockHigh, bool dataHigh)
{
    if (previousClockHigh && !clockHigh) {
        return dataHigh != clockHigh ? 1 : -1;
    }
    return 0;
}