#ifndef DHT22_LOGIC_H
#define DHT22_LOGIC_H

#include <stdint.h>

constexpr uint8_t kDht22Bits = 40;
constexpr uint16_t kDht22OneThresholdUs = 40;

enum class Dht22Status : uint8_t {
    OK,
    NO_RESPONSE,
    TIMEOUT,
    CHECKSUM,
    OUT_OF_RANGE,
    NO_TIMER
};

struct Dht22Reading {
    int16_t temperatureTenths;
    uint16_t humidityTenths;
};

bool dht22BitFromHighUs(uint16_t highUs);
Dht22Status dht22Decode(const uint16_t highUs[kDht22Bits], Dht22Reading *reading);
const char *dht22StatusName(Dht22Status status);

#endif