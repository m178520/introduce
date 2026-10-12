/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_msc.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ux_device_msc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "main.h"
#include "tx_api.h"
//#include "app_usbx_device.h"
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
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static int32_t check_sd_status(uint32_t instance);
/* USER CODE END PFP */

#define BSP_ERROR_NONE                    0
#define BSP_ERROR_BUSY                   -3

#include "bsp.h"
#include "tx_api.h"
#include "fx_api.h"

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief  Function implementing app_usb_device_thread_media_status.
  * @param arg: Not used
  * @retval status
  */
UINT  app_usb_device_thread_media_status(VOID *storage, ULONG lun, ULONG media_id, ULONG *media_status)
{

  /* The ATA drive never fails. This is just for app_usb_device only !!!! */
  return (UX_SUCCESS);
}

/**
  * @brief  Function implementing app_usb_device_thread_media_read.
  * @param  storage : Not used
  * @param  lun: Logical unit number
  * @param  lba: Logical block address
  * @param  number_blocks: Blocks number
  * @param  data_pointer: Data
  * @param  media_status: Not used
  * @retval Status (0 : OK / -1 : Error)
  */
UINT  app_usb_device_thread_media_read(VOID *storage, ULONG lun,
                                       UCHAR *data_pointer,
                                       ULONG number_blocks,
                                       ULONG lba, ULONG *media_status)
{
    UINT status = 0U;

    BSP_SD_ReadBlocks((uint32_t *) data_pointer, lba, number_blocks, 500);									
    //while(BSP_SD_GetCardState() != SD_TRANSFER_OK);
    check_sd_status(0);    
    return (status);
}

/**
  * @brief  Function implementing app_usb_device_thread_media_write.
  * @param  storage : Not used
  * @param  lun: Logical unit number
  * @param  lba: Logical block address
  * @param  number_blocks: Blocks number
  * @param  data_pointer: Data
  * @param  media_status: Not used
  * @retval Status (0 : OK / -1 : Error)
  */
UINT  app_usb_device_thread_media_write(VOID *storage, ULONG lun,
                                        UCHAR *data_pointer,
                                        ULONG number_blocks,
                                        ULONG lba, ULONG *media_status)
{
    UINT status = 0U;

    BSP_SD_WriteBlocks((uint32_t *) data_pointer, lba, number_blocks, 500);
    //while(BSP_SD_GetCardState() != SD_TRANSFER_OK);
    check_sd_status(0);
    
    return (status);
}

/**
  * @brief check SD card Transfer Status
  * @param Instance
  * @retval BSP_ERROR_NONE, BSP_ERROR_BUSY
  */
static int32_t check_sd_status(uint32_t Instance)
{
  uint32_t start = tx_time_get();

  while (tx_time_get() - start < 500)
  {
    if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
    {
      return BSP_ERROR_NONE;
    }
  }

  return BSP_ERROR_BUSY;
}


#if 0

UINT  demo_thread_media_status(VOID *storage, ULONG lun, ULONG media_id, ULONG *media_status)
{

    /* The ATA drive never fails. This is just for demo only !!!! */
    return(UX_SUCCESS);
}


UINT  demo_thread_media_read(VOID *storage, ULONG lun, UCHAR * data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{

UINT    status =  0;

    while(number_blocks--)
    {
        status =  fx_media_read(&sdio_disk,lba,data_pointer);
        data_pointer+=512;
        lba++;
    }
        
    return(status);
}


UINT  demo_thread_media_write(VOID *storage, ULONG lun, UCHAR * data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{

UINT    status =  0;


    while(number_blocks--)
    {
        status =  fx_media_write(&sdio_disk,lba,data_pointer);
        data_pointer+=512;
        lba++;
    }        
    return(status);
}

#endif





/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
