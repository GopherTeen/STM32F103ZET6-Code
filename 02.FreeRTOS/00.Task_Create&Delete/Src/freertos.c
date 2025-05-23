/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"

#include "cmsis_os.h"
#include "main.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "key.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

TaskHandle_t startTaskHandle;
TaskHandle_t task1Handle;
TaskHandle_t task2Handle;
TaskHandle_t task3Handle;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 10);
    return ch;
}

void startTask(void);
void Task1(void);
void Task2(void);
void Task3(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const* argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const* argument) {
    /* USER CODE BEGIN StartDefaultTask */
    xTaskCreate((TaskFunction_t) startTask, "startTask", 128, NULL, 0, &startTaskHandle);

    vTaskDelete(NULL); // Delete the default task
    /* Infinite loop */
    // for (;;) {
    //     osDelay(1);
    // }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void startTask(void) {
    xTaskCreate((TaskFunction_t) Task1, "task1", 128, NULL, 1, &task1Handle);
    xTaskCreate((TaskFunction_t) Task2, "task2", 128, NULL, 2, &task2Handle);
    xTaskCreate((TaskFunction_t) Task3, "task3", 128, NULL, 3, &task3Handle);

    vTaskDelete(NULL); // Delete the start task
}

void Task1(void) {
    for (;;) {
        printf("task1 running\r\n");
        HAL_GPIO_TogglePin(RedLED_GPIO_Port, RedLED_Pin);
        vTaskDelay(500);
    }
}

void Task2(void) {
    for (;;) {
        printf("task2 running\r\n");
        HAL_GPIO_TogglePin(GreenLED_GPIO_Port, GreenLED_Pin);
        vTaskDelay(500);
    }
}

void Task3(void) {
    uint8_t keyState = 0;
    for (;;) {
        // printf("task3 running\r\n");
        keyState = key_scan(0);
        if (keyState == KEY0_PRES) {
            printf("task1 delete\r\n");

            if (task1Handle != NULL) {
                vTaskDelete(task1Handle);
                task1Handle = NULL;
            }
        } else if (keyState == KEY1_PRES) {
            printf("Hello, World!\r\n");
        }
        vTaskDelay(10);
    }
}
/* USER CODE END Application */
