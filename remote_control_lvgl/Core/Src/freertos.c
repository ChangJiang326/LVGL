/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
volatile uint32_t FreeRTOS_StackOverflowHit = 0;
volatile uint32_t FreeRTOS_StackOverflowTask = 0;
volatile char FreeRTOS_StackOverflowTaskName[16];

/* USER CODE END Variables */
osThreadId ADC_TASKHandle;
osThreadId LVGL_TASKHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void ADC_Task(void const * argument);
void lvgl_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
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
  /* definition and creation of ADC_TASK */
  osThreadDef(ADC_TASK, ADC_Task, osPriorityNormal, 0, 128);
  ADC_TASKHandle = osThreadCreate(osThread(ADC_TASK), NULL);

  /* definition and creation of LVGL_TASK */
  osThreadDef(LVGL_TASK, lvgl_task, osPriorityNormal, 0, 4096);
  LVGL_TASKHandle = osThreadCreate(osThread(LVGL_TASK), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_ADC_Task */
/**
  * @brief  Function implementing the ADC thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ADC_Task */
__weak void ADC_Task(void const * argument)
{
  /* USER CODE BEGIN ADC_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ADC_Task */
}

/* USER CODE BEGIN Header_lvgl_task */
/**
* @brief Function implementing the LVGL_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lvgl_task */
__weak void lvgl_task(void const * argument)
{
  /* USER CODE BEGIN lvgl_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END lvgl_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  uint32_t i;

  FreeRTOS_StackOverflowHit = 1;
  FreeRTOS_StackOverflowTask = (uint32_t)xTask;

  for(i = 0; i < (sizeof(FreeRTOS_StackOverflowTaskName) - 1U); i++) {
    if(pcTaskName == NULL || pcTaskName[i] == '\0') {
      break;
    }
    FreeRTOS_StackOverflowTaskName[i] = pcTaskName[i];
  }
  FreeRTOS_StackOverflowTaskName[i] = '\0';

  taskDISABLE_INTERRUPTS();
  while(1) {
  }
}

/* USER CODE END Application */
