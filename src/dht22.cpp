#include "dht22.h"

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f1xx_hal.h"

namespace {

constexpr uint16_t kDataPin = GPIO_PIN_1;
constexpr uint32_t kDataTimeoutUs = 120;
constexpr uint32_t kStartLowUs = 1100;

void setOutputMode(void)
{
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = kDataPin;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);
}

void setInputMode(void)
{
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = kDataPin;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);
}

void delayMicroseconds(uint32_t microseconds)
{
    uint32_t cycles = microseconds * (SystemCoreClock / 1000000U);
    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cycles) {
    }
}

bool waitForLevel(GPIO_PinState level, uint32_t timeoutUs)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t timeoutCycles = timeoutUs * (SystemCoreClock / 1000000U);
    while (HAL_GPIO_ReadPin(GPIOA, kDataPin) != level) {
        if ((DWT->CYCCNT - start) >= timeoutCycles) {
            return false;
        }
    }
    return true;
}

bool readBit(bool *value)
{
    if (!waitForLevel(GPIO_PIN_RESET, kDataTimeoutUs) ||
        !waitForLevel(GPIO_PIN_SET, kDataTimeoutUs)) {
        return false;
    }

    uint32_t highStart = DWT->CYCCNT;
    if (!waitForLevel(GPIO_PIN_RESET, kDataTimeoutUs)) {
        return false;
    }

    uint32_t highCycles = DWT->CYCCNT - highStart;
    uint32_t oneThreshold = 40U * (SystemCoreClock / 1000000U);
    *value = highCycles > oneThreshold;
    return true;
}

void enableCycleCounter(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

} // namespace

void DHT22_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    enableCycleCounter();
    setInputMode();
}

DHT22Data DHT22_Read(void)
{
    DHT22Data result = {0.0f, 0.0f, false};
    uint8_t bytes[5] = {0};

    setOutputMode();
    HAL_GPIO_WritePin(GPIOA, kDataPin, GPIO_PIN_RESET);
    delayMicroseconds(kStartLowUs);
    setInputMode();

    taskENTER_CRITICAL();
    bool frameValid = waitForLevel(GPIO_PIN_RESET, kDataTimeoutUs) &&
                      waitForLevel(GPIO_PIN_SET, kDataTimeoutUs) &&
                      waitForLevel(GPIO_PIN_RESET, kDataTimeoutUs);
    if (frameValid) {
        for (uint8_t bitIndex = 0; bitIndex < 40; ++bitIndex) {
            bool bitValue = false;
            if (!readBit(&bitValue)) {
                frameValid = false;
                break;
            }
            bytes[bitIndex / 8] <<= 1;
            if (bitValue) {
                bytes[bitIndex / 8] |= 1U;
            }
        }
    }
    taskEXIT_CRITICAL();

    if (!frameValid) {
        return result;
    }

    uint8_t checksum = static_cast<uint8_t>(bytes[0] + bytes[1] + bytes[2] + bytes[3]);
    if (checksum != bytes[4]) {
        return result;
    }

    uint16_t rawHumidity = static_cast<uint16_t>((bytes[0] << 8) | bytes[1]);
    uint16_t rawTemperature = static_cast<uint16_t>(((bytes[2] & 0x7FU) << 8) | bytes[3]);
    result.humidity = rawHumidity / 10.0f;
    result.temperature = rawTemperature / 10.0f;
    if ((bytes[2] & 0x80U) != 0U) {
        result.temperature = -result.temperature;
    }
    result.valid = true;
    return result;
}