#include "wokwi_i2c.h"

#define I2C_SCL_PIN GPIO_PIN_6
#define I2C_SDA_PIN GPIO_PIN_7

static void i2c_delay(void)
{
    for (volatile uint32_t count = 0; count < 12U; count++) {
        __NOP();
    }
}

static void sda_high(void)
{
    GPIOB->BSRR = I2C_SDA_PIN;
}

static void sda_low(void)
{
    GPIOB->BRR = I2C_SDA_PIN;
}

static void scl_high(void)
{
    GPIOB->BSRR = I2C_SCL_PIN;
}

static void scl_low(void)
{
    GPIOB->BRR = I2C_SCL_PIN;
}

static uint8_t sda_read(void)
{
    return (GPIOB->IDR & I2C_SDA_PIN) != 0U ? 1U : 0U;
}

static void i2c_start(void)
{
    sda_high();
    scl_high();
    i2c_delay();
    sda_low();
    i2c_delay();
    scl_low();
}

static void i2c_stop(void)
{
    sda_low();
    scl_high();
    i2c_delay();
    sda_high();
    i2c_delay();
}

static HAL_StatusTypeDef i2c_write_byte(uint8_t value)
{
    for (uint8_t bit = 0; bit < 8U; bit++) {
        if ((value & 0x80U) != 0U) {
            sda_high();
        } else {
            sda_low();
        }
        scl_high();
        i2c_delay();
        scl_low();
        i2c_delay();
        value <<= 1;
    }

    sda_high();
    scl_high();
    i2c_delay();
    uint8_t acknowledged = sda_read() == 0U ? 1U : 0U;
    scl_low();
    i2c_delay();
    return acknowledged != 0U ? HAL_OK : HAL_ERROR;
}

void BareI2C1_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);

    sda_high();
    scl_high();
}

HAL_StatusTypeDef BareI2C1_IsDeviceReady(uint8_t address, uint32_t timeout)
{
    (void)timeout;
    i2c_start();
    HAL_StatusTypeDef status = i2c_write_byte((uint8_t)(address << 1));
    i2c_stop();
    return status;
}

HAL_StatusTypeDef BareI2C1_Write(uint8_t address, const uint8_t *data,
                                 uint16_t length, uint32_t timeout)
{
    (void)timeout;
    i2c_start();
    HAL_StatusTypeDef status = i2c_write_byte((uint8_t)(address << 1));

    for (uint16_t index = 0; status == HAL_OK && index < length; index++) {
        status = i2c_write_byte(data[index]);
    }

    i2c_stop();
    return status;
}