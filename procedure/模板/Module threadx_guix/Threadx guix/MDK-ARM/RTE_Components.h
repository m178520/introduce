/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file
  * @author  MCD Application Team
  * @version V2.0.0
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
#ifndef  __RTE_COMPONENTS_H__
#define  __RTE_COMPONENTS_H__

/* Defines ------------------------------------------------------------------*/
/* STMicroelectronics.X-CUBE-AZRTOS-H7.3.3.0 */
#define THREADX_ENABLED

#define CMSIS_device_header "stm32h7xx.h"

/* Keil::Compiler&ARM Compiler:Event Recorder&DAP@1.5.1 */
#define RTE_Compiler_EventRecorder
#define RTE_Compiler_EventRecorder_DAP
/* Keil::Compiler&ARM Compiler:I/O:STDOUT&EVR@1.2.0 */
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
#define RTE_Compiler_IO_STDOUT_EVR      /* Compiler I/O: STDOUT EVR */

#endif /* __RTE_COMPONENTS_H__ */
