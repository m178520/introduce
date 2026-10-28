/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : FMC.h
  * Description        : This file provides code for the configuration
  *                      of the FMC peripheral.
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
#ifndef __FMC_H
#define __FMC_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN Private defines */

#define Bank5_SDRAM_ADDR  	((uint32_t)0xC0000000)
#define Bank5_SDRAM_SIZE			(32 * 1024 * 1024)     //SDRAM��С

#define EXT_SDRAM_ADDR 			Bank5_SDRAM_ADDR
#define EXT_SDRAM_SIZE 			Bank5_SDRAM_SIZE

/* LCD�Դ�,��1ҳ, ����2M�ֽ� */
#define SDRAM_LCD_BUF1 		EXT_SDRAM_ADDR

/* LCD�Դ�,��2ҳ, ����2M�ֽ� */
#define SDRAM_LCD_BUF2		(EXT_SDRAM_ADDR + SDRAM_LCD_SIZE)

#define SDRAM_LCD_SIZE		(2 * 1024 * 1024)		/* ÿ��2M */
#define SDRAM_LCD_LAYER		2						/* 2�� */

//SDRAM���ò���
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/* USER CODE END Private defines */

void MX_FMC_Init(void);
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram);
void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* hsdram);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__FMC_H */

/**
  * @}
  */

/**
  * @}
  */
