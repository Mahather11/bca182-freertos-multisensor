#include "input_logic.h"

static DisplayMode wrapMode(int mode)
{
    if (mode < 0) {
        mode = 3;
    } else if (mode > 3) {
        mode = 0;
    }
    return static_cast<DisplayMode>(mode);
}

DisplayMode nextDisplayMode(DisplayMode mode)
{
    return wrapMode(static_cast<int>(mode) + 1);
}

DisplayMode previousDisplayMode(DisplayMode mode)
{
    return wrapMode(static_cast<int>(mode) - 1);
}

int8_t encoderStep(bool previousClockHigh, bool clockHigh, bool dataHigh)
{
    if (previousClockHigh && !clockHigh) {
        return dataHigh != clockHigh ? 1 : -1;
    }
    return 0;
}