#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>

enum class SystemState {
    ACTIVE,
    INACTIVE
};

constexpr uint32_t kInactivityTimeoutMs = 15000;

SystemState evaluateSystemState(SystemState current, bool motionDetected,
                                bool timeoutElapsed);
void vStateTask(void *pvParameters);

#endif // SYSTEM_STATE_H