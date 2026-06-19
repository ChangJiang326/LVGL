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
  * If no LICENSE file comes with this sof
  tware, it is provided AS-IS.
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
volatile uint32_t freertos_stack_overflow = 0;
volatile char freertos_stack_overflow_task[configMAX_TASK_NAME_LEN] = {0};
volatile uint8_t freertos_task_create_error = 0;

/* USER CODE END Variables */
osThreadId LVGL_TASKHandle;
osThreadId ADC_TASKHandle;
osThreadId LORA_TASKHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void lvgl_task(void const * argument);
void ADC_Task(void const * argument);
void LORA_Task(void const * argument);

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
  /* definition and creation of LVGL_TASK */
  osThreadDef(LVGL_TASK, lvgl_task, osPriorityNormal, 0, 3072);
  LVGL_TASKHandle = osThreadCreate(osThread(LVGL_TASK), NULL);
  if (LVGL_TASKHandle == NULL) freertos_task_create_error |= 0x01U;

  /* definition and creation of ADC_TASK */
  osThreadDef(ADC_TASK, ADC_Task, osPriorityNormal, 0, 128);
  ADC_TASKHandle = osThreadCreate(osThread(ADC_TASK), NULL);
  if (ADC_TASKHandle == NULL) freertos_task_create_error |= 0x02U;

  /* definition and creation of LORA_TASK */
  osThreadDef(LORA_TASK, LORA_Task, osPriorityNormal, 0, 1024);
  LORA_TASKHandle = osThreadCreate(osThread(LORA_TASK), NULL);
  if (LORA_TASKHandle == NULL) freertos_task_create_error |= 0x04U;

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_lvgl_task */
/**
  * @brief  Function implementing the LVGL_TASK thread.
  * @param  argument: Not used
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

/* USER CODE BEGIN Header_ADC_Task */
/**
* @brief Function implementing the ADC_TASK thread.
* @param argument: Not used
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

/* USER CODE BEGIN Header_LORA_Task */
/**
* @brief Function implementing the LORA_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LORA_Task */
__weak void LORA_Task(void const * argument)
{
  /* USER CODE BEGIN LORA_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LORA_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  uint32_t i;

  (void)xTask;
  freertos_stack_overflow = 1U;

  for(i = 0U; i < (configMAX_TASK_NAME_LEN - 1U); i++)
  {
    if(pcTaskName == NULL || pcTaskName[i] == '\0')
    {
      break;
    }
    freertos_stack_overflow_task[i] = pcTaskName[i];
  }
  freertos_stack_overflow_task[i] = '\0';

  taskDISABLE_INTERRUPTS();
  for(;;)
  {
  }
}

/* USER CODE END Application */
