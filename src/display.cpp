#include "display.h"
#include "ssd1306.h"
#include "rtos_objects.h"
#include "sensors.h"
#include <stdio.h>

/* Formats one decimal place using integer arithmetic only, to avoid
 * depending on newlib-nano's float-printf support (see main.cpp's
 * -Wl,-u,_printf_float note from Part IV) on a second code path. */
static void formatOneDecimal(float value, char *out, size_t outSize, const char *unit)
{
    int tenths = (int)(value * 10.0f + (value >= 0.0f ? 0.5f : -0.5f));
    int whole  = tenths / 10;
    int frac   = tenths % 10;
    if (frac < 0) frac = -frac;
    snprintf(out, outSize, "%d.%d %s", whole, frac, unit);
}

void vDisplayTask(void *pvParameters)
{
    (void)pvParameters;

    SensorData reading = {0};
    bool haveReading = false;
    char line[21];

    for (;;) {
        /* Block waiting for the next sensor reading; on timeout, keep
         * showing the last known value (or "--" if none has arrived yet). */
        if (xQueueReceive(xQueueSensorToDisplay, &reading, pdMS_TO_TICKS(2500)) == pdPASS) {
            haveReading = true;
        }

        SSD1306_Clear();

        SSD1306_SetCursor(0, 0);
        SSD1306_WriteString("ROOM MONITOR");

        SSD1306_SetCursor(2, 0);
        SSD1306_WriteString("TEMP:");

        SSD1306_SetCursor(2, 6);
        if (haveReading) {
            formatOneDecimal(reading.temperature, line, sizeof(line), "C");
        } else {
            snprintf(line, sizeof(line), "-- C");
        }
        SSD1306_WriteString(line);

        SSD1306_SetCursor(4, 0);
        SSD1306_WriteString("HUM:");
        SSD1306_SetCursor(4, 6);
        if (haveReading) {
            formatOneDecimal(reading.humidity, line, sizeof(line), "%");
        } else {
            snprintf(line, sizeof(line), "-- %%");
        }
        SSD1306_WriteString(line);

        SSD1306_SetCursor(6, 0);
        if (haveReading) {
            snprintf(line, sizeof(line), "LIGHT:%3d%%", reading.lightLevel);
        } else {
            snprintf(line, sizeof(line), "LIGHT:--%%");
        }
        SSD1306_WriteString(line);

        SSD1306_SetCursor(7, 0);
        SSD1306_WriteString(haveReading && reading.motionDetected ? "MOTION:ON" : "MOTION:OFF");

        SSD1306_UpdateScreen();
    }
}