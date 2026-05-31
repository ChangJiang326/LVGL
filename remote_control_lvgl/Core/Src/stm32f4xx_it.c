/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint32_t HardFault_CFSR;
volatile uint32_t HardFault_HFSR;
volatile uint32_t HardFault_DFSR;
volatile uint32_t HardFault_AFSR;
volatile uint32_t HardFault_BFAR;
volatile uint32_t HardFault_MMFAR;
volatile uint32_t HardFault_LR;
volatile uint32_t HardFault_PC;
volatile uint32_t HardFault_BFSR;
volatile uint32_t HardFault_MFSR;
volatile uint32_t HardFault_UFSR;
volatile uint32_t HardFault_CCR;
volatile uint32_t HardFault_BFARVALID;
volatile uint32_t HardFault_PRECISERR;
volatile uint32_t HardFault_IMPRECISERR;
volatile uint32_t Fault_Handler_Type;
volatile uint32_t Fault_Stack_R0;
volatile uint32_t Fault_Stack_R1;
volatile uint32_t Fault_Stack_R2;
volatile uint32_t Fault_Stack_R3;
volatile uint32_t Fault_Stack_R12;
volatile uint32_t Fault_Stack_XPSR;
volatile uint32_t Fault_Stack_SP;
volatile uint32_t Fault_MSP;
volatile uint32_t Fault_PSP;
volatile uint32_t ADC_DMA_IRQ_Block_Debug = 1;
volatile uint32_t ADC_DMA_IRQ_Handler_Count = 0;
volatile uint32_t ADC_DMA_IRQ_Skipped_Count = 0;

static void Fault_Capture(uint32_t type, uint32_t *stack)
{
  Fault_Handler_Type = type;
  HardFault_CFSR = SCB->CFSR;
  HardFault_HFSR = SCB->HFSR;
  HardFault_DFSR = SCB->DFSR;
  HardFault_CCR = SCB->CCR;
  HardFault_AFSR = SCB->AFSR;
  HardFault_BFAR = SCB->BFAR;
  HardFault_MMFAR = SCB->MMFAR;
  Fault_Stack_R0 = stack[0];
  Fault_Stack_R1 = stack[1];
  Fault_Stack_R2 = stack[2];
  Fault_Stack_R3 = stack[3];
  Fault_Stack_R12 = stack[4];
  HardFault_LR = stack[5];
  HardFault_PC = stack[6];
  Fault_Stack_XPSR = stack[7];
  Fault_Stack_SP = (uint32_t)stack;
  Fault_MSP = __get_MSP();
  Fault_PSP = __get_PSP();
  HardFault_MFSR = HardFault_CFSR & 0xFFU;
  HardFault_BFSR = (HardFault_CFSR >> 8) & 0xFFU;
  HardFault_UFSR = (HardFault_CFSR >> 16) & 0xFFFFU;
  HardFault_BFARVALID = HardFault_BFSR & (1U << 7);
  HardFault_PRECISERR = HardFault_BFSR & (1U << 1);
  HardFault_IMPRECISERR = HardFault_BFSR & (1U << 2);
}

__attribute__((noreturn, used)) void Fault_Capture_And_Halt(uint32_t type, uint32_t *stack)
{
  Fault_Capture(type, stack);
  __disable_irq();
  while(1) {
  }
}

__attribute__((naked, used)) void Fault_Handler_Entry(void)
{
  __asm volatile (
    "TST lr, #4\n"
    "ITE EQ\n"
    "MRSEQ r1, MSP\n"
    "MRSNE r1, PSP\n"
    "B Fault_Capture_And_Halt\n"
  );
}

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim6;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
__attribute__((naked)) void HardFault_Handler(void)
{
  __asm volatile (
    "MOVS r0, #1\n"
    "B Fault_Handler_Entry\n"
  );
}

/**
  * @brief This function handles Memory management fault.
  */
__attribute__((naked)) void MemManage_Handler(void)
{
  __asm volatile (
    "MOVS r0, #2\n"
    "B Fault_Handler_Entry\n"
  );
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
__attribute__((naked)) void BusFault_Handler(void)
{
  __asm volatile (
    "MOVS r0, #3\n"
    "B Fault_Handler_Entry\n"
  );
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
__attribute__((naked)) void UsageFault_Handler(void)
{
  __asm volatile (
    "MOVS r0, #4\n"
    "B Fault_Handler_Entry\n"
  );
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
  ADC_DMA_IRQ_Handler_Count++;
  if(ADC_DMA_IRQ_Block_Debug != 0U)
  {
    ADC_DMA_IRQ_Skipped_Count++;
    HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
    return;
  }

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
