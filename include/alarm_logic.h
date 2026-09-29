#ifndef ALARM_LOGIC_H
#define ALARM_LOGIC_H

constexpr float kTempLowLimitC = 18.0f;
constexpr float kTempHighLimitC = 30.0f;

enum class AlarmState {
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

AlarmState evaluateTemperature(float temperatureC);
const char *alarmStateName(AlarmState state);

#endif // ALARM_LOGIC_H