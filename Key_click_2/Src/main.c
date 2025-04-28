/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "gpio.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 按键事件类型
typedef enum {
    KEY_EVENT_NONE = 0, // 无事件
    KEY_EVENT_SINGLE, // 单击
    KEY_EVENT_DOUBLE, // 双击
    KEY_EVENT_LONG, // 长按（持续1秒）
} KeyEventType;

// 按键状态机状态
typedef enum {
    KEY_STATE_RELEASED, // 按键释放状态
    KEY_STATE_PRESS_DEBOUNCE, // 按下消抖状态
    KEY_STATE_RELEASE_DEBOUNCE, // 释放消抖状态
    KEY_STATE_PRESSED, // 按下稳定状态
    KEY_STATE_LONG_PRESSED, // 长按状态
    KEY_STATE_WAIT_DOUBLE, // 等待双击时间窗口
} KeyState;

// 按键结构体
typedef struct {
    KeyState state; // 当前状态
    uint32_t pressTime; // 按下时间戳（ms）
    uint32_t releaseTime; // 释放时间戳（ms）
    uint8_t clickCount; // 连击次数
    KeyEventType event; // 检测到的事件
} KeyHandle;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_TIME 20 // 消抖时间（单位：ms）
#define LONG_PRESS_TIME 1000 // 长按判定时间（单位：ms）
#define MULTI_CLICK_TIME 200 // 双击判定窗口（单位：ms）
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
KeyHandle key1_handle = {KEY_STATE_RELEASED, 0, 0, 0, KEY_EVENT_NONE};

char msg[] = "Long Press detected\r\n";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 获取按键事件 */
KeyEventType KEY_GetEvent(void) {
    KeyEventType event = key1_handle.event;
    key1_handle.event = KEY_EVENT_NONE; // 清除事件标志
    return event;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_TIM6_Init();
    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim6);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        // 获取按键事件
        KeyEventType event = KEY_GetEvent();
        if (event != KEY_EVENT_NONE) {
            switch (event) {
                case KEY_EVENT_SINGLE: // 单击操作：例如切换红灯状态
                    HAL_GPIO_TogglePin(RedLED_GPIO_Port, RedLED_Pin);
                    break;
                case KEY_EVENT_DOUBLE: // 双击操作：例如切换绿灯状态
                    HAL_GPIO_TogglePin(GreenLED_GPIO_Port, GreenLED_Pin);
                    break;
                case KEY_EVENT_LONG: // 长按操作：例如通过串口发送信息
                    HAL_UART_Transmit(&huart1, (uint8_t*) msg, sizeof(msg), 1000);
                    break;
                default: break;
            }
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        HAL_Delay(10); // 主循环适当延时，降低CPU负担
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
/* 定时器中断回调函数 */
// todo
//! 长按后的释放会记录一次单击
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM6) {
        // 读取按键状态（假设按键按下为高电平，如硬件为低电平有效请调整判断）
        uint8_t keyPressed = !HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin);

        switch (key1_handle.state) {
            case KEY_STATE_RELEASED:
                if (keyPressed) {
                    key1_handle.state = KEY_STATE_PRESS_DEBOUNCE;
                    key1_handle.pressTime = HAL_GetTick();
                }
                break;

            case KEY_STATE_PRESS_DEBOUNCE:
                if (HAL_GetTick() - key1_handle.pressTime >= DEBOUNCE_TIME) {
                    if (keyPressed) {
                        key1_handle.state = KEY_STATE_PRESSED;
                    } else {
                        key1_handle.state = KEY_STATE_RELEASED;
                    }
                }
                break;

            case KEY_STATE_PRESSED:
                if (!keyPressed) {
                    // 正常释放时进入释放消抖状态
                    key1_handle.state = KEY_STATE_RELEASE_DEBOUNCE;
                    key1_handle.releaseTime = HAL_GetTick();
                } else {
                    // 检测长按：进入长按状态，不立即输出事件
                    if (HAL_GetTick() - key1_handle.pressTime >= LONG_PRESS_TIME) {
                        key1_handle.clickCount = 0;
                        key1_handle.state = KEY_STATE_LONG_PRESSED;
                        // 注意：此时不输出事件，等待松手后再输出长按事件
                    }
                }
                break;

            case KEY_STATE_LONG_PRESSED:
                if (!keyPressed) {
                    // 按键释放后，消抖后输出长按事件，并清除点击计数（防止被识别成单击）
                    key1_handle.state = KEY_STATE_RELEASE_DEBOUNCE;
                    key1_handle.releaseTime = HAL_GetTick();
                    key1_handle.event = KEY_EVENT_LONG;
                }
                // 如果依然按下，则继续停留在长按状态
                break;

            case KEY_STATE_RELEASE_DEBOUNCE:
                if (HAL_GetTick() - key1_handle.releaseTime >= DEBOUNCE_TIME) {
                    if (!keyPressed) {
                        // 如果之前不是长按情况，则更新点击计数
                        if (key1_handle.event == KEY_EVENT_NONE) {
                            key1_handle.clickCount++; // 增加点击次数
                            key1_handle.state = KEY_STATE_WAIT_DOUBLE;
                            key1_handle.releaseTime = HAL_GetTick();
                        } else {
                            // 如果已经有长按事件，则直接进入释放状态，不累计单击计数
                            key1_handle.state = KEY_STATE_RELEASED;
                            key1_handle.clickCount = 0;
                        }
                    } else {
                        key1_handle.state = KEY_STATE_PRESSED;
                    }
                }
                break;

            case KEY_STATE_WAIT_DOUBLE:
                if (keyPressed) {
                    key1_handle.state = KEY_STATE_PRESS_DEBOUNCE;
                    key1_handle.pressTime = HAL_GetTick();
                } else if (HAL_GetTick() - key1_handle.releaseTime >= MULTI_CLICK_TIME) {
                    // 时间窗口结束，确定点击次数（只处理单击或双击）
                    if (key1_handle.clickCount == 1) {
                        key1_handle.event = KEY_EVENT_SINGLE;
                    } else if (key1_handle.clickCount == 2) {
                        key1_handle.event = KEY_EVENT_DOUBLE;
                    }
                    key1_handle.clickCount = 0;
                    key1_handle.state = KEY_STATE_RELEASED;
                }
                break;

            default: key1_handle.state = KEY_STATE_RELEASED; break;
        }
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {}
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
