#include <unity.h>

#include "alarm_logic.h"
#include "dht22_logic.h"
#include "input_logic.h"
#include "sensors_logic.h"
#include "system_state.h"

void setUp(void) {}
void tearDown(void) {}

void test_alarm_below_lower_limit(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(AlarmState::LOW_TEMPERATURE),
                          static_cast<int>(evaluateTemperature(17.9f)));
}

void test_alarm_at_lower_limit_is_normal(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(AlarmState::NORMAL),
                          static_cast<int>(evaluateTemperature(18.0f)));
}

void test_alarm_normal_temperature(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(AlarmState::NORMAL),
                          static_cast<int>(evaluateTemperature(25.0f)));
}

void test_alarm_at_upper_limit_is_normal(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(AlarmState::NORMAL),
                          static_cast<int>(evaluateTemperature(30.0f)));
}

void test_alarm_above_upper_limit(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(AlarmState::HIGH_TEMPERATURE),
                          static_cast<int>(evaluateTemperature(30.1f)));
}

void test_navigation_forward_transition(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayMode::HUMIDITY),
                          static_cast<int>(nextDisplayMode(DisplayMode::TEMPERATURE)));
}

void test_navigation_forward_wraparound(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayMode::TEMPERATURE),
                          static_cast<int>(nextDisplayMode(DisplayMode::MOTION)));
}

void test_navigation_reverse_transition(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayMode::LIGHT),
                          static_cast<int>(previousDisplayMode(DisplayMode::MOTION)));
}

void test_navigation_reverse_wraparound(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayMode::MOTION),
                          static_cast<int>(previousDisplayMode(DisplayMode::TEMPERATURE)));
}

void test_encoder_clockwise_falling_edge(void)
{
    TEST_ASSERT_EQUAL_INT(1, encoderStep(true, false, true));
}

void test_encoder_counterclockwise_falling_edge(void)
{
    TEST_ASSERT_EQUAL_INT(-1, encoderStep(true, false, false));
}

void test_encoder_ignores_rising_edge(void)
{
    TEST_ASSERT_EQUAL_INT(0, encoderStep(false, true, false));
}

static void encodeDht22Bytes(const uint8_t bytes[5], uint16_t pulseWidths[kDht22Bits])
{
    for (uint8_t bitIndex = 0; bitIndex < kDht22Bits; ++bitIndex) {
        bool bitValue = (bytes[bitIndex / 8] & (0x80U >> (bitIndex % 8))) != 0U;
        pulseWidths[bitIndex] = bitValue ? 70U : 28U;
    }
}

void test_dht22_bit_threshold(void)
{
    TEST_ASSERT_FALSE(dht22BitFromHighUs(kDht22OneThresholdUs));
    TEST_ASSERT_TRUE(dht22BitFromHighUs(kDht22OneThresholdUs + 1U));
}

void test_dht22_decodes_valid_frame(void)
{
    const uint8_t bytes[5] = {0x02, 0x64, 0x00, 0xFE, 0x64};
    uint16_t pulseWidths[kDht22Bits];
    encodeDht22Bytes(bytes, pulseWidths);
    Dht22Reading reading = {0, 0};

    TEST_ASSERT_EQUAL_INT(static_cast<int>(Dht22Status::OK),
                          static_cast<int>(dht22Decode(pulseWidths, &reading)));
    TEST_ASSERT_EQUAL_INT16(254, reading.temperatureTenths);
    TEST_ASSERT_EQUAL_UINT16(612, reading.humidityTenths);
}

void test_dht22_rejects_bad_checksum(void)
{
    const uint8_t bytes[5] = {0x02, 0x64, 0x00, 0xFE, 0x00};
    uint16_t pulseWidths[kDht22Bits];
    encodeDht22Bytes(bytes, pulseWidths);
    Dht22Reading reading = {0, 0};

    TEST_ASSERT_EQUAL_INT(static_cast<int>(Dht22Status::CHECKSUM),
                          static_cast<int>(dht22Decode(pulseWidths, &reading)));
}

void test_dht22_rejects_out_of_range_humidity(void)
{
    const uint8_t bytes[5] = {0x03, 0xE9, 0x00, 0xFE, 0xEA};
    uint16_t pulseWidths[kDht22Bits];
    encodeDht22Bytes(bytes, pulseWidths);
    Dht22Reading reading = {0, 0};

    TEST_ASSERT_EQUAL_INT(static_cast<int>(Dht22Status::OUT_OF_RANGE),
                          static_cast<int>(dht22Decode(pulseWidths, &reading)));
}

void test_dht22_decodes_negative_temperature(void)
{
    const uint8_t bytes[5] = {0x02, 0x64, 0x80, 0x19, 0xFF};
    uint16_t pulseWidths[kDht22Bits];
    encodeDht22Bytes(bytes, pulseWidths);
    Dht22Reading reading = {0, 0};

    TEST_ASSERT_EQUAL_INT(static_cast<int>(Dht22Status::OK),
                          static_cast<int>(dht22Decode(pulseWidths, &reading)));
    TEST_ASSERT_EQUAL_INT16(-25, reading.temperatureTenths);
}

void test_light_adc_full_scale_is_darkest(void)
{
    TEST_ASSERT_EQUAL_INT(0, lightPercentFromAdc(kAdcMax));
}

void test_light_adc_zero_is_brightest(void)
{
    TEST_ASSERT_EQUAL_INT(100, lightPercentFromAdc(0));
}

void test_light_adc_midscale_is_about_half(void)
{
    TEST_ASSERT_EQUAL_INT(50, lightPercentFromAdc(kAdcMax / 2U));
}

void test_light_adc_clamps_out_of_range(void)
{
    TEST_ASSERT_EQUAL_INT(0, lightPercentFromAdc(kAdcMax + 1U));
}

void test_state_active_without_timeout(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SystemState::ACTIVE),
                          static_cast<int>(evaluateSystemState(SystemState::ACTIVE, false, false)));
}

void test_state_active_timeout_becomes_inactive(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SystemState::INACTIVE),
                          static_cast<int>(evaluateSystemState(SystemState::ACTIVE, false, true)));
}

void test_state_inactive_without_motion_stays_inactive(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SystemState::INACTIVE),
                          static_cast<int>(evaluateSystemState(SystemState::INACTIVE, false, false)));
}

void test_state_inactive_motion_reactivates(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SystemState::ACTIVE),
                          static_cast<int>(evaluateSystemState(SystemState::INACTIVE, true, false)));
}

void test_state_motion_wins_over_timeout(void)
{
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SystemState::ACTIVE),
                          static_cast<int>(evaluateSystemState(SystemState::ACTIVE, true, true)));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_alarm_below_lower_limit);
    RUN_TEST(test_alarm_at_lower_limit_is_normal);
    RUN_TEST(test_alarm_normal_temperature);
    RUN_TEST(test_alarm_at_upper_limit_is_normal);
    RUN_TEST(test_alarm_above_upper_limit);
    RUN_TEST(test_navigation_forward_transition);
    RUN_TEST(test_navigation_forward_wraparound);
    RUN_TEST(test_navigation_reverse_transition);
    RUN_TEST(test_navigation_reverse_wraparound);
    RUN_TEST(test_encoder_clockwise_falling_edge);
    RUN_TEST(test_encoder_counterclockwise_falling_edge);
    RUN_TEST(test_encoder_ignores_rising_edge);
    RUN_TEST(test_dht22_bit_threshold);
    RUN_TEST(test_dht22_decodes_valid_frame);
    RUN_TEST(test_dht22_rejects_bad_checksum);
    RUN_TEST(test_dht22_rejects_out_of_range_humidity);
    RUN_TEST(test_dht22_decodes_negative_temperature);
    RUN_TEST(test_light_adc_full_scale_is_darkest);
    RUN_TEST(test_light_adc_zero_is_brightest);
    RUN_TEST(test_light_adc_midscale_is_about_half);
    RUN_TEST(test_light_adc_clamps_out_of_range);
    RUN_TEST(test_state_active_without_timeout);
    RUN_TEST(test_state_active_timeout_becomes_inactive);
    RUN_TEST(test_state_inactive_without_motion_stays_inactive);
    RUN_TEST(test_state_inactive_motion_reactivates);
    RUN_TEST(test_state_motion_wins_over_timeout);
    return UNITY_END();
}