#ifndef WOKWI_I2C_H
#define WOKWI_I2C_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BareI2C1_Init(void);
HAL_StatusTypeDef BareI2C1_IsDeviceReady(uint8_t address, uint32_t timeout);
HAL_StatusTypeDef BareI2C1_Write(uint8_t address, const uint8_t *data,
                                 uint16_t length, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* WOKWI_I2C_H */