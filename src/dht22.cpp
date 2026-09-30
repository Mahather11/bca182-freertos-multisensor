#include "dht22.h"

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f1xx_hal.h"

namespace {

constexpr uint16_t kDataPin = GPIO_PIN_1;
constexpr uint32_t kDataPinShift = 4;
constexpr uint32_t kDataModeMask = 0xFU << kDataPinShift;
constexpr uint32_t kDataTimeoutUs = 100;
constexpr uint8_t kDataPortConfiguration = 0x6U;

bool cycleCounterReady = false;
uint32_t cyclesPerMicrosecond = 8U;

void setOutputMode(void)
{
    GPIOA->BRR = kDataPin;
    GPIOA->CRL = (GPIOA->CRL & ~kDataModeMask) |
                 (kDataPortConfiguration << kDataPinShift);
}

void setInputMode(void)
{
    GPIOA->BSRR = kDataPin;
    GPIOA->CRL = (GPIOA->CRL & ~kDataModeMask) | (0x8U << kDataPinShift);
}

bool waitForLevel(bool high, uint32_t timeoutCycles)
{
    uint32_t start = DWT->CYCCNT;
    for (uint32_t guard = 0; guard < timeoutCycles; ++guard) {
        bool pinHigh = (GPIOA->IDR & kDataPin) != 0U;
        if (pinHigh == high) {
            return true;
        }
        if ((DWT->CYCCNT - start) >= timeoutCycles) {
            return false;
        }
    }
    return false;
}

bool captureFrame(uint16_t pulseWidths[kDht22Bits], Dht22Status *status,
                  uint32_t timeoutCycles)
{
    setInputMode();
    if (!waitForLevel(true, timeoutCycles) ||
        !waitForLevel(false, timeoutCycles) ||
        !waitForLevel(true, timeoutCycles) ||
        !waitForLevel(false, timeoutCycles)) {
        *status = Dht22Status::NO_RESPONSE;
        return false;
    }

    for (uint8_t bitIndex = 0; bitIndex < kDht22Bits; ++bitIndex) {
        if (!waitForLevel(true, timeoutCycles)) {
            *status = Dht22Status::TIMEOUT;
            return false;
        }
        uint32_t highStart = DWT->CYCCNT;
        if (!waitForLevel(false, timeoutCycles)) {
            *status = Dht22Status::TIMEOUT;
            return false;
        }
        pulseWidths[bitIndex] = static_cast<uint16_t>(
            (DWT->CYCCNT - highStart) / cyclesPerMicrosecond);
    }
    return true;
}

} // namespace

void DHT22_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    setInputMode();
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    cyclesPerMicrosecond = SystemCoreClock / 1000000U;
    uint32_t initialCycles = DWT->CYCCNT;
    for (volatile uint8_t probe = 0; probe < 16; ++probe) {
    }
    cycleCounterReady = DWT->CYCCNT != initialCycles && cyclesPerMicrosecond != 0U;
}

DHT22Data DHT22_Read(void)
{
    DHT22Data result = {0.0f, 0.0f, Dht22Status::NO_TIMER};
    if (!cycleCounterReady) {
        return result;
    }

    setOutputMode();
    vTaskDelay(pdMS_TO_TICKS(2));

    uint16_t pulseWidths[kDht22Bits] = {0};
    Dht22Status status = Dht22Status::OK;
    taskENTER_CRITICAL();
    bool frameCaptured = captureFrame(pulseWidths, &status,
                                      kDataTimeoutUs * cyclesPerMicrosecond);
    taskEXIT_CRITICAL();
    if (!frameCaptured) {
        result.status = status;
        return result;
    }

    Dht22Reading reading = {0, 0};
    result.status = dht22Decode(pulseWidths, &reading);
    if (result.status == Dht22Status::OK) {
        result.temperature = reading.temperatureTenths / 10.0f;
        result.humidity = reading.humidityTenths / 10.0f;
    }
    return result;
}