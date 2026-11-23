/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user_global.h"

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
#define  APP_CFG_TASK_START_PRIO                          2u
#define  APP_CFG_TASK_START_STK_SIZE                    4096u
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE/8];
static  void  AppTaskStart          (ULONG thread_input);

#define  APP_CFG_TASK_LED_PRIO                          2u
#define  APP_CFG_TASK_LED_STK_SIZE                    4096u
static  TX_THREAD   AppTaskLEDTCB;
static  uint64_t    AppTaskLEDStk[APP_CFG_TASK_START_STK_SIZE/8];
static  void  AppTaskLED          (ULONG thread_input);

#define  APP_CFG_TASK_GUI_PRIO                          17u
#define  APP_CFG_TASK_GUI_STK_SIZE                    4096u
static  TX_THREAD   AppTaskGUITCB;
static  uint64_t    AppTaskGUIStk[APP_CFG_TASK_START_STK_SIZE/8];
static  void  AppTaskGUI          (ULONG thread_input);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
extern TX_MUTEX   PrintfSemp;	/* 用于printf互斥 */
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);

static  void  DispTaskInfo          (void);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */
	
		/* 创建任务 */
    AppTaskCreate(); 

	/* 创建任务间通信机制 */
		AppObjCreate();	

  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 2 */
/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务。
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (ULONG thread_input)
{
//	EXECUTION_TIME TolTime, IdleTime, deltaTolTime, deltaIdleTime;
//	uint32_t uiCount = 0;
	(void)thread_input;

	/* 计算CPU利用率 */
//	IdleTime = _tx_execution_idle_time_total;
//	TolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
    while (1)
	{  
		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
//        bsp_ProPer1ms();
		
		/* CPU利用率统计 */
//		uiCount++;
//		if(uiCount == 200)
//		{
//			uiCount = 0;
//			deltaIdleTime = _tx_execution_idle_time_total - IdleTime;
//			deltaTolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total - TolTime;
//			OSCPUUsage = (double)deltaIdleTime/deltaTolTime;
//			OSCPUUsage = 100- OSCPUUsage*100;
//			IdleTime = _tx_execution_idle_time_total;
//			TolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
//		}
//			DispTaskInfo();
//			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
//      tx_thread_sleep(3000);
		TOUCH_CapScan();
		tx_thread_sleep(5);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLED
*	功能说明: LED
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 4
*********************************************************************************************************
*/
static void AppTaskLED(ULONG thread_input)
{
	(void)thread_input;

	while(1)
	{
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
    tx_thread_sleep(2000);
	}   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUI
*	功能说明: GUI
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 4
*********************************************************************************************************
*/
static void AppTaskGUI(ULONG thread_input)
{
	(void)thread_input;
	//测试屏幕
//	LCD_SetBackLight(0);
//	memcpy(SDRAM_LCD_BUF,&DISPLAY_1_THEME_1_BK1_1024X600_pixelmap_data,sizeof(DISPLAY_1_THEME_1_BK1_1024X600_pixelmap_data));
//	LCD_SetBackLight(255);
	while(1)
	{
		MainTask();
	}   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	
		/**************创建启动任务*********************/
    tx_thread_create(&AppTaskStartTCB,              /* 任务控制块地址 */   
                       "App Task Start",              /* 任务名 */
                       AppTaskStart,                  /* 启动任务函数地址 */
                       0,                             /* 传递给任务的参数 */
                       &AppTaskStartStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_START_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_START_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_START_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                 /* 创建后立即启动 */
	
	/**************创建LED任务*********************/
    tx_thread_create(&AppTaskLEDTCB,                /* 任务控制块地址 */    
                       "App LED",                      /* 任务名 */
                       AppTaskLED,                     /* 启动任务函数地址 */
                       0,                              /* 传递给任务的参数 */
                       &AppTaskLEDStk[0],             /* 堆栈基地址 */
                       APP_CFG_TASK_LED_STK_SIZE,     /* 堆栈空间大小 */  
                       APP_CFG_TASK_LED_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_LED_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                /* 创建后立即启动 */

   /**************创建GUI任务*********************/
    tx_thread_create(&AppTaskGUITCB,                /* 任务控制块地址 */    
                       "App GUI",                      /* 任务名 */
                       AppTaskGUI,                     /* 启动任务函数地址 */
                       0,                              /* 传递给任务的参数 */
                       &AppTaskGUIStk[0],             /* 堆栈基地址 */
                       APP_CFG_TASK_GUI_STK_SIZE,     /* 堆栈空间大小 */  
                       APP_CFG_TASK_GUI_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_GUI_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                /* 创建后立即启动 */               
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	 /* 创建互斥信号量 */
    tx_mutex_create(&PrintfSemp,"AppPrintfSemp",TX_NO_INHERIT);
}


 
/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将ThreadX任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	TX_THREAD      *p_tcb;	        /* 定义一个任务控制块指针 */

    p_tcb = &AppTaskStartTCB;
	
	/* 打印标题 */
//	App_Printf("===============================================================\r\n");
//	App_Printf("CPU利用率 = %5.2f%%\r\n", OSCPUUsage);
//	App_Printf("任务执行时间 = %.9fs\r\n", (double)_tx_execution_thread_time_total/SystemCoreClock);
//	App_Printf("空闲执行时间 = %.9fs\r\n", (double)_tx_execution_idle_time_total/SystemCoreClock);
//	App_Printf("中断执行时间 = %.9fs\r\n", (double)_tx_execution_isr_time_total/SystemCoreClock);
//	App_Printf("系统总执行时间 = %.9fs\r\n", (double)(_tx_execution_thread_time_total + \
//		                                               _tx_execution_idle_time_total +  \
//	                                                   _tx_execution_isr_time_total)/SystemCoreClock);	
	RTC_LOG_Printf("===============================================================\r\n");
	RTC_LOG_Printf(" 任务优先级 任务栈大小 当前使用栈  最大栈使用   任务名\r\n");
	RTC_LOG_Printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

	/* 遍历任务控制列表TCB list)，打印所有的任务的优先级和名称 */
	while (p_tcb != (TX_THREAD *)0) 
	{
		
		RTC_LOG_Printf("   %2d        %5d      %5d       %5d      %s\r\n", 
                    p_tcb->tx_thread_priority,
                    p_tcb->tx_thread_stack_size,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_ptr,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_highest_ptr,
                    p_tcb->tx_thread_name);


        p_tcb = p_tcb->tx_thread_created_next;

        if(p_tcb == &AppTaskStartTCB) break;
	}
}


/* USER CODE END 2 */
