/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32h7xx_hal.h"
//#include "tx_execution_profile.h"
#include "stdarg.h"
#include <stdio.h>

#include "gpio.h"
#include "fmc.h"
#include "ltdc.h"
#include "Touch.h"

#include "dma2d.h"
#include "MainTask.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//extern EXECUTION_TIME     _tx_execution_thread_time_total;
//extern EXECUTION_TIME     _tx_execution_isr_time_total;
//extern EXECUTION_TIME     _tx_execution_idle_time_total;

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

/*
*********************************************************************************************************
*                                 ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¼ï¿½ï¿½ï¿½ï¿½ï¿½ÖµÔ½Ð¡ï¿½ï¿½ï¿½È¼ï¿½Ô½ï¿½ï¿½
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                          2u
#define  APP_CFG_TASK_LED_PRIO                            4u
#define  APP_CFG_TASK_GUI_TOUCH_PRIO                      1u
#define  APP_CFG_TASK_GUI_PRIO                            17u

/*
*********************************************************************************************************
*                                   ï¿½ï¿½ï¿½ï¿½Õ»ï¿½ï¿½Ð¡ï¿½ï¿½ï¿½ï¿½Î»ï¿½Ö½ï¿½
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    1024
#define  APP_CFG_TASK_LED_STK_SIZE                   		1024
#define  APP_CFG_TASK_GUI_TOUCH_STK_SIZE                1024
#define  APP_CFG_TASK_GUI_STK_SIZE                      1024

/*
*********************************************************************************************************
*                                       ï¿½ï¿½Ì¬È«ï¿½Ö±ï¿½ï¿½ï¿½
*********************************************************************************************************
*/                                                        
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE/8];

static  TX_THREAD   AppTaskGUITCB;
static  uint64_t    AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE/8];

static  TX_THREAD   AppTaskLedTCB;
static  uint64_t    AppTaskLedStk[APP_CFG_TASK_LED_STK_SIZE/8];

static  TX_THREAD   AppTaskGUITOUCHTCB;
static  uint64_t    AppTaskGUITOUCHStk[APP_CFG_TASK_LED_STK_SIZE/8];

/*
*********************************************************************************************************
*                                      ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
*********************************************************************************************************
*/
static  void  AppTaskStart          (ULONG thread_input);
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);
static  void  AppTaskLED            (ULONG thread_input);
static  void  AppTaskGUI						(ULONG thread_input);
static  void  AppTaskGUITouch       (ULONG thread_input);

void  DispTaskInfo          (void);
void  App_Printf (const char *fmt, ...);


/*
*******************************************************************************************************
*                               ï¿½ï¿½ï¿½ï¿½
*******************************************************************************************************
*/
static  TX_MUTEX   AppPrintfSemp;	/* ï¿½ï¿½ï¿½ï¿½printfï¿½ï¿½ï¿½ï¿½ */

__IO double OSCPUUsage;       	   /* CPUï¿½Ù·Ö±ï¿½ */

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
	
	/**************ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½*********************/
    tx_thread_create(&AppTaskStartTCB,              /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ¿ï¿½ï¿½Ö· */   
                       "App Task Start",              /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
                       AppTaskStart,                  /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö· */
                       0,                             /* ï¿½ï¿½ï¿½Ý¸ï¿½ï¿½ï¿½ï¿½ï¿½Ä²ï¿½ï¿½ï¿? */
                       &AppTaskStartStk[0],            /* ï¿½ï¿½Õ»ï¿½ï¿½ï¿½ï¿½Ö· */
                       APP_CFG_TASK_START_STK_SIZE,    /* ï¿½ï¿½Õ»ï¿½Õ¼ï¿½ï¿½Ð? */  
                       APP_CFG_TASK_START_PRIO,        /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¼ï¿½*/
                       APP_CFG_TASK_START_PRIO,        /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Õ¼ï¿½ï¿½Öµ */
                       TX_NO_TIME_SLICE,               /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½Æ¬ */
                       TX_AUTO_START);                 /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
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

/* USER CODE BEGIN 1 */

/*
*********************************************************************************************************
*	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: AppTaskStart
*	ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½: ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
*	ï¿½ï¿½    ï¿½ï¿½: thread_input ï¿½ï¿½ï¿½Ú´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½Ýµï¿½ï¿½Î²ï¿½
*	ï¿½ï¿½ ï¿½ï¿½ Öµ: ï¿½ï¿½
	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: 2
*********************************************************************************************************
*/
extern void module_application_define(void);
static  void  AppTaskStart (ULONG thread_input)
{
//	EXECUTION_TIME TolTime, IdleTime, deltaTolTime, deltaIdleTime;
	(void)thread_input;
	

	/* ï¿½ÚºË¿ï¿½ï¿½ï¿½ï¿½ó£¬»Ö¸ï¿½HALï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½×¼ */
    HAL_ResumeTick();
	
 /* ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼ï¿½ï¿? */
    MX_GPIO_Init();
		MX_FMC_Init();
		MX_DMA2D_Init();
		TOUCH_InitHard();
//		MX_LTDC_Init();
//		MX_TIM3_Init();
	
	//´´½¨¶¯Ì¬¼ÓÔØapp¹ÜÀíÈÎÎñ
		module_application_define();
	/* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
		AppTaskCreate(); 

	/* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¨ï¿½Å»ï¿½ï¿½ï¿? */
		AppObjCreate();	
	
	/* ï¿½ï¿½ï¿½ï¿½CPUï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
//	IdleTime = _tx_execution_idle_time_total;
//	TolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
    while (1)
	{
//		if((_tx_execution_idle_time_total != TX_EXECUTION_MAX_TIME_SOURCE) || 
//			 (_tx_execution_isr_time_total != TX_EXECUTION_MAX_TIME_SOURCE)  || 
//			 (_tx_execution_thread_time_total != TX_EXECUTION_MAX_TIME_SOURCE))
//		{
//			/* CPUï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í³ï¿½ï¿½ */
//			deltaIdleTime = _tx_execution_idle_time_total - IdleTime;
//			deltaTolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total - TolTime;
//			OSCPUUsage = (double)deltaIdleTime/deltaTolTime;
//			OSCPUUsage = 100- OSCPUUsage*100;
//			IdleTime = _tx_execution_idle_time_total;
//			TolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
//			
//		}
//		else
//		{
//			_tx_execution_idle_time_reset();
//			_tx_execution_isr_time_reset();
//			_tx_execution_thread_total_time_reset();
//		}
		DispTaskInfo();
    tx_thread_sleep(8000);
  }
}

/*
*********************************************************************************************************
*	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: AppTaskCreate
*	ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½: ï¿½ï¿½ï¿½ï¿½Ó¦ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
*	ï¿½ï¿½    ï¿½ï¿½: ï¿½ï¿½
*	ï¿½ï¿½ ï¿½ï¿½ Öµ: ï¿½ï¿½
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	/**************´´½¨GUIXÈÎÎñ*********************/
    tx_thread_create(&AppTaskGUITCB,               /* ÈÎÎñ¿ØÖÆ¿éµØÖ· */    
                       "App Task GUI",              /* ÈÎÎñÃû */
                       AppTaskGUI,                  /* Æô¶¯ÈÎÎñº¯ÊýµØÖ· */
                       0,                           /* ´«µÝ¸øÈÎÎñµÄ²ÎÊý */
                       &AppTaskGUIStk[0],            /* ¶ÑÕ»»ùµØÖ· */
                       APP_CFG_TASK_GUI_STK_SIZE,    /* ¶ÑÕ»¿Õ¼ä´óÐ¡ */  
                       APP_CFG_TASK_GUI_PRIO,        /* ÈÎÎñÓÅÏÈ¼¶*/
                       APP_CFG_TASK_GUI_PRIO,        /* ÈÎÎñÇÀÕ¼·§Öµ */
                       TX_NO_TIME_SLICE,             /* ²»¿ªÆôÊ±¼äÆ¬ */
                       TX_AUTO_START);               /* ´´½¨ºóÁ¢¼´Æô¶¯ */
	
		/**************ï¿½ï¿½ï¿½ï¿½ï¿½Åºï¿½Õ¹Ê¾ï¿½ï¿½ï¿½ï¿½*********************/
    tx_thread_create(&AppTaskLedTCB,                /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ¿ï¿½ï¿½Ö· */    
                       "App LED",                      /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
                       AppTaskLED,                     /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö· */
                       0,                              /* ï¿½ï¿½ï¿½Ý¸ï¿½ï¿½ï¿½ï¿½ï¿½Ä²ï¿½ï¿½ï¿??*/
                       &AppTaskLedStk[0],             /* ï¿½ï¿½Õ»ï¿½ï¿½ï¿½ï¿½Ö· */
                       APP_CFG_TASK_LED_STK_SIZE,     /* ï¿½ï¿½Õ»ï¿½Õ¼ï¿½ï¿½ï¿½?*/  
                       APP_CFG_TASK_LED_PRIO,        /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¼ï¿½*/
                       APP_CFG_TASK_LED_PRIO,        /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Õ¼ï¿½ï¿½Öµ */
                       TX_NO_TIME_SLICE,               /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½Æ¬ */
                       TX_AUTO_START);                /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
	
		/**************´´½¨ÆÁÄ»´¥ÃþÈÎÎñ*********************/
    tx_thread_create(&AppTaskGUITOUCHTCB,              		/* ÈÎÎñ¿ØÖÆ¿éµØÖ· */   
                       "App Task GUI Touch",              /* ÈÎÎñÃû */
                       AppTaskGUITouch,                  	/* Æô¶¯ÈÎÎñº¯ÊýµØÖ· */
                       0,                             		/* ´«µÝ¸øÈÎÎñµÄ²ÎÊý */
                       &AppTaskGUITOUCHStk[0],            /* ¶ÑÕ»»ùµØÖ· */
                       APP_CFG_TASK_GUI_TOUCH_STK_SIZE,   /* ¶ÑÕ»¿Õ¼ä´óÐ¡ */  
                       APP_CFG_TASK_GUI_TOUCH_PRIO,       /* ÈÎÎñÓÅÏÈ¼¶*/
                       APP_CFG_TASK_GUI_TOUCH_PRIO,       /* ÈÎÎñÇÀÕ¼·§Öµ */
                       TX_NO_TIME_SLICE,               		/* ²»¿ªÆôÊ±¼äÆ¬ */
                       TX_AUTO_START);                 		/* ´´½¨ºóÁ¢¼´Æô¶¯ */
                       
}
/*
*********************************************************************************************************
*	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: AppObjCreate
*	ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½: ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¨Ñ¶
*	ï¿½ï¿½    ï¿½ï¿½: ï¿½ï¿½
*	ï¿½ï¿½ ï¿½ï¿½ Öµ: ï¿½ï¿½
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	 /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Åºï¿½ï¿½ï¿½ */
    tx_mutex_create(&AppPrintfSemp,"AppPrintfSemp",TX_NO_INHERIT);
}

/*
*********************************************************************************************************
*	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: AppTaskLED
*	ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½: LED
*	ï¿½ï¿½    ï¿½ï¿½: thread_input ï¿½ï¿½ï¿½Ú´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½Ýµï¿½ï¿½Î²ï¿½
*	ï¿½ï¿½ ï¿½ï¿½ Öµ: ï¿½ï¿½
	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: 4
*********************************************************************************************************
*/

static void AppTaskLED(ULONG thread_input)
{
	(void)thread_input;
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
    tx_thread_sleep(3000);
	}
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: AppTaskGUI
*	¹¦ÄÜËµÃ÷: GUIÓ¦ÓÃÈÎÎñ
*	ÐÎ    ²Î: thread_input ´´½¨¸ÃÈÎÎñÊ±´«µÝµÄÐÎ²Î
*	·µ »Ø Öµ: ÎÞ
	ÓÅ ÏÈ ¼¶: 5
*********************************************************************************************************
*/

static void AppTaskGUI(ULONG thread_input)
{	
  (void)thread_input;
	while(1)
	{
		MainTask();
//		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
//    tx_thread_sleep(1000);
	}   
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: AppTaskGUITouch
*	¹¦ÄÜËµÃ÷: LED
*	ÐÎ    ²Î: thread_input ÊÇÔÚ´´½¨¸ÃÈÎÎñÊ±´«µÝµÄÐÎ²Î
*	·µ »Ø Öµ: ÎÞ
	ÓÅ ÏÈ ¼¶: 2
*********************************************************************************************************
*/
static void AppTaskGUITouch(ULONG thread_input)
{
	(void)thread_input;
	
	while(1)
	{
		TOUCH_CapScan();
//		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
    tx_thread_sleep(5);
	}   
}
/*
*********************************************************************************************************
*	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: App_Printf
*	ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½: ï¿½ß³Ì°ï¿½È«ï¿½ï¿½printfï¿½ï¿½Ê½		  			  
*	ï¿½ï¿½    ï¿½ï¿½: Í¬printfï¿½Ä²ï¿½ï¿½ï¿½ï¿½ï¿½
*             ï¿½ï¿½Cï¿½Ð£ï¿½ï¿½ï¿½ï¿½Þ·ï¿½ï¿½Ð³ï¿½ï¿½ï¿½ï¿½Ýºï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Êµï¿½Îµï¿½ï¿½ï¿½ï¿½Íºï¿½ï¿½ï¿½Ä¿Ê±,ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¡ï¿½Ôºï¿½Ö¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
*	ï¿½ï¿½ ï¿½ï¿½ Öµ: ï¿½ï¿½
*********************************************************************************************************
*/
void  App_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1]; /* ï¿½Ø±ï¿½×¢ï¿½â£¬ï¿½ï¿½ï¿½printfï¿½Ä±ï¿½ï¿½ï¿½ï¿½Ï¶à£¬×¢ï¿½ï¿½Ë¾Ö²ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä´ï¿½Ð¡ï¿½Ç·ï¿½ï¿½ï¿? */
    va_list   v_args;


    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

	/* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);

    printf("%s", buf_str);

    tx_mutex_put(&AppPrintfSemp);
}
 
/*
*********************************************************************************************************
*	ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½: DispTaskInfo
*	ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½: ï¿½ï¿½ThreadXï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ï¢Í¨ï¿½ï¿½ï¿½ï¿½ï¿½Ú´ï¿½Ó¡ï¿½ï¿½ï¿½ï¿½
*	ï¿½ï¿½    ï¿½Î£ï¿½ï¿½ï¿½
*	ï¿½ï¿½ ï¿½ï¿½ Öµ: ï¿½ï¿½
*********************************************************************************************************
*/
void DispTaskInfo(void)
{
	TX_THREAD      *p_tcb;	        /* ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ¿ï¿½Ö¸ï¿½ï¿? */

    p_tcb = &AppTaskStartTCB;
	
	/* ´òÓ¡±êÌâ */
//	App_Printf("===============================================================\r\n");
//	App_Printf("CPUÀûÓÃÂÊ = %5.2f%%\r\n", OSCPUUsage);
//	App_Printf("ÈÎÎñÖ´ÐÐÊ±¼ä = %.9fs\r\n", (double)_tx_execution_thread_time_total/SystemCoreClock);
//	App_Printf("¿ÕÏÐÖ´ÐÐÊ±¼ä = %.9fs\r\n", (double)_tx_execution_idle_time_total/SystemCoreClock);
//	App_Printf("ÖÐ¶ÏÖ´ÐÐÊ±¼ä = %.9fs\r\n", (double)_tx_execution_isr_time_total/SystemCoreClock);
//	App_Printf("ÏµÍ³×ÜÖ´ÐÐÊ±¼ä = %.9fs\r\n", (double)(_tx_execution_thread_time_total + \
//		                                               _tx_execution_idle_time_total +  \
//	                                                   _tx_execution_isr_time_total)/SystemCoreClock);	
	App_Printf("===============================================================\r\n");
	App_Printf(" ÈÎÎñÓÅÏÈ¼¶ ÈÎÎñÕ»´óÐ¡ µ±Ç°Ê¹ÓÃÕ»  ×î´óÕ»Ê¹ÓÃ   ÈÎÎñÃû\r\n");
	App_Printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

	/* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ð±ï¿½TCB list)ï¿½ï¿½ï¿½ï¿½Ó¡ï¿½ï¿½ï¿½Ðµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? */
	while (p_tcb != (TX_THREAD *)0) 
	{
		
		App_Printf("   %2d        %5d      %5d       %5d      %s\r\n", 
                    p_tcb->tx_thread_priority,
                    p_tcb->tx_thread_stack_size,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_ptr,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_highest_ptr,
                    p_tcb->tx_thread_name);


        p_tcb = p_tcb->tx_thread_created_next;

        if(p_tcb == &AppTaskStartTCB) break;
	}
}

/* USER CODE END 1 */
