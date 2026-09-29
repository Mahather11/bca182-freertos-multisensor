#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "display.h"
#include "input.h"
#include "alarm.h"
#include "motion.h"
#include "system_state.h"
#include "ssd1306.h"
#include "wokwi_i2c.h"
#include <stdio.h>

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;
extern "C" uint32_t g_pfnVectors[];

/* Diagnostic counter defined in ssd1306.c */
extern "C" uint32_t g_ssd1306_i2c_errors;

static void MX_USART1_UART_Init(void);

/* ---------------------------------------------------------
 * Retarget printf() to USART1.
 * --------------------------------------------------------- */
extern "C" int _write(int fd, char *ptr, int len)
{
    (void)fd;

    bool schedulerRunning = xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
    if (schedulerRunning && serialMutex != NULL) {
        xSemaphoreTake(serialMutex, portMAX_DELAY);
    }

    HAL_UART_Transmit(&huart1, reinterpret_cast<uint8_t *>(ptr),
                      static_cast<uint16_t>(len), HAL_MAX_DELAY);

    if (schedulerRunning && serialMutex != NULL) {
        xSemaphoreGive(serialMutex);
    }
    return len;
}

/* ---------------------------------------------------------
 * HAL tick on TIM4, not SysTick: the custom FreeRTOS port
 * (src/port.c) owns SysTick for context switching.
 * --------------------------------------------------------- */
extern "C" HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    __HAL_RCC_TIM4_CLK_ENABLE();
    TIM4->PSC = 7;              /* 8 MHz / 8 = 1 MHz */
    TIM4->ARR = 999;            /* 1 kHz -> 1 ms     */
    TIM4->CNT = 0;
    TIM4->DIER |= TIM_DIER_UIE;
    TIM4->CR1 |= TIM_CR1_CEN;

    HAL_NVIC_SetPriority(TIM4_IRQn, TickPriority, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    return HAL_OK;
}

extern "C" void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR &= ~TIM_SR_UIF;
        HAL_IncTick();
    }
}

/* ---------------------------------------------------------
 * System Clock Configuration: HSI 8 MHz, no PLL.
 * --------------------------------------------------------- */
extern "C" void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

/* ---------------------------------------------------------
 * Peripheral Initializers
 * --------------------------------------------------------- */
extern "C" void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef encoderPins = {0};
    encoderPins.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    encoderPins.Mode = GPIO_MODE_INPUT;
    encoderPins.Pull = GPIO_PULLUP;
    encoderPins.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &encoderPins);
}

extern "C" void MX_ADC1_Init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    ADC_ChannelConfTypeDef sConfig = {0};
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&hadc1);

    sConfig.Channel = ADC_CHANNEL_0;   /* PA0 = LDR */
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

extern "C" void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (huart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9;             /* TX */
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;            /* RX */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/* ---------------------------------------------------------
 * FreeRTOS Required Callback Hooks
 * --------------------------------------------------------- */
extern "C" {
    void vAssertCalled(const char *pcFile, int ulLine)
    {
        (void)pcFile;
        (void)ulLine;
        taskDISABLE_INTERRUPTS();
        for (;;);
    }

    void vApplicationIdleHook(void) {}

    void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
    {
        (void)xTask;
        (void)pcTaskName;
        taskDISABLE_INTERRUPTS();
        for (;;);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    SCB->VTOR = (uint32_t)g_pfnVectors;
    __DSB();
    __ISB();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    alarm_init();
    motion_init();

    printf("BCA182 FreeRTOS Multisensor\r\n");
    printf("System starting...\r\n");

    /* Bare-metal I2C1 (see wokwi_i2c.h for why HAL_I2C_Init is bypassed). */
    BareI2C1_Init();

    HAL_StatusTypeDef probeStatus = BareI2C1_IsDeviceReady(SSD1306_I2C_ADDR, 100);
    printf("SSD1306 address probe (0x%02X): %s\r\n",
           (unsigned)SSD1306_I2C_ADDR,
           (probeStatus == HAL_OK) ? "ACK (device found)" : "NO RESPONSE");

    /* FreeRTOS object creation before any task that uses them
     * (Section 41: hardware init -> RTOS objects -> task creation). */
    initRTOSObjects();

    SSD1306_Init();
    printf("SSD1306 I2C errors during init: %lu\r\n",
           (unsigned long)g_ssd1306_i2c_errors);
    printf("DISPLAY: OLED initialised\r\n");

    /* Priorities per Section 38's suggested table. */
    xTaskCreate(vSensorTask,          "SensorTask",  256, NULL, 2, NULL);
    xTaskCreate(vDisplayTask,         "DisplayTask", 256, NULL, 1, NULL);
    xTaskCreate(vInputTask,           "InputTask",   256, NULL, 3, NULL);
    xTaskCreate(vAlarmTask,           "AlarmTask",   256, NULL, 2, NULL);
    xTaskCreate(vMotionTask,          "MotionTask",  256, NULL, 3, NULL);
    xTaskCreate(vStateTask,           "StateTask",   256, NULL, 2, NULL);

    vTaskStartScheduler();

    while (1) {}
}