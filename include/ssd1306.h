#ifndef SSD1306_H
#define SSD1306_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#define SSD1306_WIDTH     128
#define SSD1306_HEIGHT    64
#define SSD1306_PAGES     (SSD1306_HEIGHT / 8)   /* 8 pages of 8 rows each */

/* 7-bit I2C address (Wokwi's board-ssd1306 diagram.json uses "0x3c").
 * BareI2C1_* functions take and shift this themselves. */
#define SSD1306_I2C_ADDR  0x3C

#ifdef __cplusplus
extern "C" {
#endif

void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_UpdateScreen(void);

/* page: 0-7 (each page is one row of 8 pixels tall).
 * col:  character column, 0-20 (128 px / 6 px per glyph). */
void SSD1306_SetCursor(uint8_t page, uint8_t col);
void SSD1306_WriteChar(char c);
void SSD1306_WriteString(const char *str);

void SSD1306_DisplayOn(void);
void SSD1306_DisplayOff(void);

#ifdef __cplusplus
}
#endif

#endif /* SSD1306_H */