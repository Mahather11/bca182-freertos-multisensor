#include "system_state.h"
#include "rtos_objects.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include <stdio.h>

void vStateTask(void *pvParameters)
{
    (void)pvParameters;

    SystemState state = SystemState::ACTIVE;
    TickType_t lastMotion = xTaskGetTickCount();
    const TickType_t timeout = pdMS_TO_TICKS(kInactivityTimeoutMs);

    printf("StateTask started\r\n");

    for (;;) {
        TickType_t elapsed = xTaskGetTickCount() - lastMotion;
        TickType_t waitTime = portMAX_DELAY;
        if (state == SystemState::ACTIVE) {
            waitTime = elapsed < timeout ? timeout - elapsed : 0;
        }

        EventBits_t bits = xEventGroupWaitBits(
            xSystemEvents, EVENT_MOTION, pdFALSE, pdFALSE, waitTime);
        bool motionDetected = (bits & EVENT_MOTION) != 0;
        TickType_t now = xTaskGetTickCount();
        if (motionDetected) {
            lastMotion = now;
        }

        bool timeoutElapsed = (now - lastMotion) >= timeout;
        SystemState next = evaluateSystemState(state, motionDetected, timeoutElapsed);
        if (next != state) {
            state = next;
            if (state == SystemState::ACTIVE) {
                xEventGroupSetBits(xSystemEvents, EVENT_ACTIVE);
                printf("STATE: ACTIVE (motion)\r\n");
            } else {
                xEventGroupClearBits(xSystemEvents, EVENT_ACTIVE);
                printf("STATE: INACTIVE (no motion for 15 s)\r\n");
            }
        }

        if (motionDetected) {
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}