/*
*********************************************************************************************************
*
*	模块名称 : USBX模拟U盘测试。
*	文件名称 : demo_sd_usbx.c
*	版    本 : V1.0
*	说    明 : SD卡模拟U盘的打开和关闭的实现
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2021-01-30   Eric2013    正式发布
*
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"



/* USBX动态内存分配 */
#define USBX_MEMORY_SIZE            (32 * 1024)
#define USBX_APP_BYTE_POOL_SIZE     ((1024 * 10) + (USBX_MEMORY_SIZE))

static UX_SLAVE_CLASS_STORAGE_PARAMETER    storage_parameter;
static uint8_t usbx_pool[USBX_APP_BYTE_POOL_SIZE];

extern FX_MEDIA        sdio_disk;
extern SD_HandleTypeDef uSdHandle;

PCD_HandleTypeDef  hpcd_USB_OTG_FS;

/*
*********************************************************************************************************
*	函 数 名: OpenUsbStorage
*	功能说明: 打开SD卡的U盘模拟
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void OpenUsbStorage(void)
{
    static uint32_t uiFirstRun = 0;
    UINT  status =  UX_SUCCESS;
    ULONG device_framework_hs_length;
    ULONG device_framework_fs_length;
    ULONG string_framework_length;
    ULONG languge_id_framework_length;
    UCHAR *device_framework_high_speed;
    UCHAR *device_framework_full_speed;
    UCHAR *string_framework;
    UCHAR *language_id_framework;

  
    if(uiFirstRun == 0)
    {
        uiFirstRun = 1;
        
        /* 初始化USBX */
        ux_system_initialize(usbx_pool, USBX_MEMORY_SIZE, UX_NULL, 0);
    }


    /* USB设备协议栈初始化 */    
    device_framework_high_speed = USBD_Get_Device_Framework_Speed(HIGH_SPEED, &device_framework_hs_length);
    device_framework_full_speed = USBD_Get_Device_Framework_Speed(FULL_SPEED, &device_framework_fs_length);
    string_framework = USBD_Get_String_Framework(&string_framework_length);
    language_id_framework = USBD_Get_Language_Id_Framework(&languge_id_framework_length);

    status =  _ux_device_stack_initialize(device_framework_high_speed,
                                            device_framework_hs_length,
                                            device_framework_full_speed,
                                            device_framework_fs_length,
                                            string_framework,
                                            string_framework_length,
                                            language_id_framework,
                                            languge_id_framework_length, UX_NULL);

    if (status != UX_SUCCESS)
    {
        return;
    }

    /* 初始化SD卡 */
    BSP_SD_Init();

    /* 1个USB模拟U盘设备 */
    storage_parameter.ux_slave_class_storage_parameter_number_lun = 1;

    /* 设置相关参数并注册读写API */
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_last_lba        = (ULONG)(uSdHandle.SdCard.BlockNbr - 1);
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_block_length    = uSdHandle.SdCard.BlockSize;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_type            =  0;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_removable_flag  =  0x80;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_read            =  app_usb_device_thread_media_read;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_write           =  app_usb_device_thread_media_write; 
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_status          =  app_usb_device_thread_media_status;

    /* 注册USB模U盘到接口0，配置1*/
    status =  _ux_device_stack_class_register(_ux_system_slave_class_storage_name, _ux_device_class_storage_entry, 1, 0, (VOID *)&storage_parameter);
                                                                                  
    if (status != UX_SUCCESS)
    {
        return;
    }
  
    /* 配置USB GPIO, NVIC */
    {
        GPIO_InitTypeDef  GPIO_InitStruct = {0};

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* 使能USB FS时钟 */
        __HAL_RCC_USB2_OTG_FS_CLK_ENABLE();

        /* 在睡眠模式下禁用USB时钟 */
        __HAL_RCC_USB2_OTG_FS_ULPI_CLK_SLEEP_DISABLE();
        
        /* 配置USB FS中断 */
        HAL_NVIC_SetPriority(OTG_FS_IRQn, 0x2, 0); 
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
    }

    /* 初始化USB */
    {
        HAL_PWREx_EnableUSBVoltageDetector();

        memset(&hpcd_USB_OTG_FS, 0x0, sizeof(PCD_HandleTypeDef));
        hpcd_USB_OTG_FS.Instance = USB2_OTG_FS;
        hpcd_USB_OTG_FS.Init.dev_endpoints = 8;
        hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = 0;
        hpcd_USB_OTG_FS.Init.ep0_mps = 0x40;
        hpcd_USB_OTG_FS.Init.low_power_enable = 0;
        hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
        hpcd_USB_OTG_FS.Init.Sof_enable = 0;
        hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
        hpcd_USB_OTG_FS.Init.vbus_sensing_enable = 0;
        hpcd_USB_OTG_FS.Init.lpm_enable = 0;

        /* 初始化USB  */
        HAL_PCD_Init(&hpcd_USB_OTG_FS);

        /* 设置TX FIFO和RX FIFO */
        HAL_PCDEx_SetRxFiFo(&hpcd_USB_OTG_FS, 1024);
        HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 0, 64);
        HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 1, 1024);

        /* 注册STM32到USBX协议栈并初始化 */
        status =  ux_dcd_stm32_initialize((ULONG)USB2_OTG_FS, (ULONG)&hpcd_USB_OTG_FS);

        if (status != FX_SUCCESS)
        {
            return;
        }
        
    }
    
    /* 启动USB */
    HAL_PCD_Start(&hpcd_USB_OTG_FS);
    
}

/*
*********************************************************************************************************
*	函 数 名: CloseUsbStorage
*	功能说明: 关闭SD卡的U盘模拟
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void  CloseUsbStorage(void)
{
    /* 停止USB */
    HAL_PCD_Stop(&hpcd_USB_OTG_FS);

    /* 复位USB相关配置 */    
    _ux_dcd_stm32_uninitialize((ULONG)USB2_OTG_FS, (ULONG)&hpcd_USB_OTG_FS);
    _ux_device_stack_uninitialize();
    _ux_device_stack_class_unregister(_ux_system_slave_class_storage_name, _ux_device_class_storage_entry);

    /* 关闭USB时钟，中断，并复位SD卡 */
    __HAL_RCC_USB2_OTG_FS_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
    HAL_PCD_DeInit(&hpcd_USB_OTG_FS);  
    BSP_SD_DeInit();   
}

/*
*********************************************************************************************************
*	函 数 名: OTG_FS_IRQHandler
*	功能说明: USB中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void OTG_FS_IRQHandler(void)
{
    _ux_dcd_stm32_interrupt_handler();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
