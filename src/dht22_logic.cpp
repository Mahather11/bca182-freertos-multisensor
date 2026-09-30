#include "dht22_logic.h"

bool dht22BitFromHighUs(uint16_t highUs)
{
    return highUs > kDht22OneThresholdUs;
}

Dht22Status dht22Decode(const uint16_t highUs[kDht22Bits], Dht22Reading *reading)
{
    if (highUs == nullptr || reading == nullptr) {
        return Dht22Status::TIMEOUT;
    }

    uint8_t bytes[5] = {0};
    for (uint8_t bitIndex = 0; bitIndex < kDht22Bits; ++bitIndex) {
        bytes[bitIndex / 8] = static_cast<uint8_t>(
            (bytes[bitIndex / 8] << 1) | (dht22BitFromHighUs(highUs[bitIndex]) ? 1U : 0U));
    }

    uint8_t checksum = static_cast<uint8_t>(bytes[0] + bytes[1] + bytes[2] + bytes[3]);
    if (checksum != bytes[4]) {
        return Dht22Status::CHECKSUM;
    }

    uint16_t humidity = static_cast<uint16_t>((bytes[0] << 8) | bytes[1]);
    int16_t temperature = static_cast<int16_t>(((bytes[2] & 0x7FU) << 8) | bytes[3]);
    if ((bytes[2] & 0x80U) != 0U) {
        temperature = static_cast<int16_t>(-temperature);
    }

    if (humidity > 1000U || temperature < -400 || temperature > 800) {
        return Dht22Status::OUT_OF_RANGE;
    }

    reading->temperatureTenths = temperature;
    reading->humidityTenths = humidity;
    return Dht22Status::OK;
}

const char *dht22StatusName(Dht22Status status)
{
    switch (status) {
        case Dht22Status::OK: return "ok";
        case Dht22Status::NO_RESPONSE: return "no response";
        case Dht22Status::TIMEOUT: return "timeout";
        case Dht22Status::CHECKSUM: return "checksum mismatch";
        case Dht22Status::OUT_OF_RANGE: return "value out of range";
        case Dht22Status::NO_TIMER: return "cycle counter not running";
    }
    return "unknown";
}