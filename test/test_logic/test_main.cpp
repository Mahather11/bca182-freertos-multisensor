#include <unity.h>

#include "alarm_logic.h"
#include "input_logic.h"
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
    RUN_TEST(test_state_active_without_timeout);
    RUN_TEST(test_state_active_timeout_becomes_inactive);
    RUN_TEST(test_state_inactive_without_motion_stays_inactive);
    RUN_TEST(test_state_inactive_motion_reactivates);
    return UNITY_END();
}