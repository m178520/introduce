/*
*********************************************************************************************************
*	                                  
*	模块名称 : 动态APP调用主程序的函数
*	文件名称 : App_ModuleCallFun.c
*	版    本 : V1.0
*	说    明 : 动态APP调用主程序的函数
*
*	修改记录 :
*		版本号   日期         作者            说明
*       V1.0    2022-04-30   Eric2013         首发
*                                       
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "txm_module.h"
#include "main.h"
#include <stdio.h>
#include "tim.h"

#define App_Printf_ID1       TXM_APPLICATION_REQUEST_ID_BASE
#define App_Printf_ID2       TXM_APPLICATION_REQUEST_ID_BASE + 1
#define bsp_SetTIMOutPWM_ID  TXM_APPLICATION_REQUEST_ID_BASE + 2


/*
*********************************************************************************************************
*	函 数 名: ModuleManagerAppRequest
*	功能说明: 动态APP调用主程序里面的函数
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
extern uint32_t rbarW[16];
extern uint32_t rasrW[16];
extern void ARM_MPU_OrderedMemcpy1(uint32_t* __RESTRICT dst, volatile uint32_t*  src, uint32_t len);
UINT  ModuleManagerAppRequest(ULONG request_id, ALIGN_TYPE param_1, ALIGN_TYPE param_2, ALIGN_TYPE param_3)
{
   
    switch(request_id)
    {
        /* 执行函数printf */
        case App_Printf_ID1:
//					printf("执行APP2的打印函数\n");
            HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
            return TX_SUCCESS;
        
        /* 执行函数printf */
        case App_Printf_ID2:
            printf("执行APP2的打印函数\n");
            return TX_SUCCESS;
        
        /* 执行函数bsp_SetTIMOutPWM*/
        case bsp_SetTIMOutPWM_ID:
            LCD_SetBackLight(param_2);
            return TX_SUCCESS;
        
        default: 
            return TX_NOT_AVAILABLE;
    }
}