/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS_Pin GPIO_PIN_0
#define RS_GPIO_Port GPIOF
#define X1_Pin GPIO_PIN_0
#define X1_GPIO_Port GPIOA
#define Y1_Pin GPIO_PIN_1
#define Y1_GPIO_Port GPIOA
#define X2_Pin GPIO_PIN_2
#define X2_GPIO_Port GPIOA
#define Y2_Pin GPIO_PIN_3
#define Y2_GPIO_Port GPIOA
#define B1_Pin GPIO_PIN_4
#define B1_GPIO_Port GPIOA
#define B2_Pin GPIO_PIN_5
#define B2_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_15
#define KEY2_GPIO_Port GPIOF
#define KEY1_Pin GPIO_PIN_0
#define KEY1_GPIO_Port GPIOG
#define M1_Pin GPIO_PIN_6
#define M1_GPIO_Port GPIOC
#define M0_Pin GPIO_PIN_7
#define M0_GPIO_Port GPIOC
#define AUX_Pin GPIO_PIN_8
#define AUX_GPIO_Port GPIOC
#define DIN_Pin GPIO_PIN_9
#define DIN_GPIO_Port GPIOC
#define CLK_Pin GPIO_PIN_8
#define CLK_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_2
#define KEY3_GPIO_Port GPIOD
#define Q2_Pin GPIO_PIN_3
#define Q2_GPIO_Port GPIOD
#define RD_Pin GPIO_PIN_4
#define RD_GPIO_Port GPIOD
#define WR_Pin GPIO_PIN_5
#define WR_GPIO_Port GPIOD
#define CS_Pin GPIO_PIN_7
#define CS_GPIO_Port GPIOD
#define KEY4_Pin GPIO_PIN_9
#define KEY4_GPIO_Port GPIOG
#define Q1_Pin GPIO_PIN_10
#define Q1_GPIO_Port GPIOG
#define RST_Pin GPIO_PIN_1
#define RST_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
