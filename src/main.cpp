#include "stm32f1xx_hal.h"
#include <string.h>

UART_HandleTypeDef huart1;
static void MX_USART1_UART_Init(void);

// Required for HAL timing
extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
}

int main(void) {
    // 1. Initialize HAL (Uses Wokwi's default stable clock)
    HAL_Init();

    // 2. Initialize UART1
    MX_USART1_UART_Init();

    // 3. Print the required startup message
    const char* startup_msg = "BCA182 FreeRTOS Multisensor\r\nSystem starting...\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)startup_msg, strlen(startup_msg), HAL_MAX_DELAY);

    // Main loop
    while (1) {
    }
}

// --- MINIMAL WOKWI-SAFE BOILERPLATE ---

static void MX_USART1_UART_Init(void) {
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

extern "C" void HAL_UART_MspInit(UART_HandleTypeDef* huart) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        // PA9 = TX
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // PA10 = RX
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}