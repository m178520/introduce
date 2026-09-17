/*
*********************************************************************************************************
*	                                  
*	模块名称 : 动态APP1应用程序
*	文件名称 : App_Module.c
*	版    本 : V1.0
*	说    明 : 动态APP1应用程序
*
*	修改记录 :
*		版本号   日期         作者            说明
*       V1.0    2022-04-30   Eric2013         首发
*                                       
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#define TXM_MODULE


#include "bsp.h"
#include "txm_module.h"


/*
*********************************************************************************************************
*                        MPU配置主程序和动态APP的共享内存，本程序暂未用到
*********************************************************************************************************
*/
#define READONLY_REGION            0x24070000
#define READWRITE_REGION           0x24078000

typedef enum {
    PROCESSING_NOT_STARTED    = 99,
    WRITING_TO_READWRITE      = 88,
    WRITING_TO_READONLY       = 77,
    READING_FROM_READWRITE    = 66,
    READING_FROM_READONLY     = 55,
    PROCESSING_FINISHED       = 44
} ProgressState;


/*
*********************************************************************************************************
*                                    宏定义
*********************************************************************************************************
*/
#define DEFAULT_STACK_SIZE        1024

#define App_Printf_ID1       TXM_APPLICATION_REQUEST_ID_BASE
#define App_Printf_ID2       TXM_APPLICATION_REQUEST_ID_BASE + 1
#define bsp_SetTIMOutPWM_ID  TXM_APPLICATION_REQUEST_ID_BASE + 2


#define MODULE_THREAD_PRIO                         3
#define MODULE_THREAD_PREEMPTION_THRESHOLD         MODULE_THREAD_PRIO

uint64_t      AppModuleStk[DEFAULT_STACK_SIZE/8];

/* 相关控制块 */
TX_THREAD               *thread_0;
TX_QUEUE                *resident_queue;

/* 函数 */
void thread_0_entry(ULONG thread_input);
void Error_Handler1(void);


/*
*********************************************************************************************************
*	函 数 名: default_module_start
*	功能说明: 动态APP入口
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
void  default_module_start(ULONG id)
{
    CHAR    *pointer;

    /* 从主程序申请相关控制块，不在APP里面申请，防止APP出问题了影响主程序 */
    txm_module_object_allocate((void*)&thread_0, sizeof(TX_THREAD));
    //txm_module_object_allocate((void*)&byte_pool_0, sizeof(TX_BYTE_POOL));
    //txm_module_object_allocate((void*)&block_pool_0, sizeof(TX_BLOCK_POOL));

    /* 创建任务 */
    tx_thread_create(thread_0, 
                     "module thread 1", 
                     thread_0_entry, 
                     0,
                     &AppModuleStk[0], 
                     DEFAULT_STACK_SIZE,
                     MODULE_THREAD_PRIO, 
                     MODULE_THREAD_PREEMPTION_THRESHOLD, 
                     TX_NO_TIME_SLICE, 
                     TX_AUTO_START);

}

/*
*********************************************************************************************************
*	函 数 名: thread_0_entry
*	功能说明: 动态APP里面的任务
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
void thread_0_entry(ULONG thread_input)
{
    UINT status;
    ULONG s_msg;
    ULONG readbuffer;

    /* 请求访问主程序里面的消息队列 */
    status = txm_module_object_pointer_get(TXM_QUEUE_OBJECT, "Resident Queue", (VOID **)&resident_queue);

    if(status)
    {
        Error_Handler1();
    }

    /* 下面几个暂未用到，后面MPU方式使用展示 */
    s_msg = WRITING_TO_READWRITE;
    tx_queue_send(resident_queue, &s_msg, TX_NO_WAIT);
    *(ULONG *)READWRITE_REGION = 0xABABABAB;
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 10);

    s_msg = READING_FROM_READWRITE;
    tx_queue_send(resident_queue, &s_msg, TX_NO_WAIT);
    readbuffer = *(ULONG*)READWRITE_REGION;
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 10);

    s_msg = READING_FROM_READONLY;
    tx_queue_send(resident_queue, &s_msg, TX_NO_WAIT);
    readbuffer = *(ULONG*)READONLY_REGION;
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 10);

    s_msg = WRITING_TO_READONLY;
    tx_queue_send(resident_queue, &s_msg, TX_NO_WAIT);
    *(ULONG *)READONLY_REGION = 0xABABABAB;
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 10);

    /* 发送出来完毕消息  */
    s_msg = PROCESSING_FINISHED;
    tx_queue_send(resident_queue, &s_msg, TX_NO_WAIT);

    /* 防止警告 */
    UNUSED(readbuffer);

    while(1)
    {
        /* 调用主程序里面的串口打印 */
        txm_module_application_request(App_Printf_ID1, 0, 0, 0);
        tx_thread_sleep(1000);
    }
}

/*
*********************************************************************************************************
*	函 数 名: thread_0_entry
*	功能说明: 执行出错
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
void Error_Handler1(void)
{
    tx_thread_sleep(TX_WAIT_FOREVER);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
