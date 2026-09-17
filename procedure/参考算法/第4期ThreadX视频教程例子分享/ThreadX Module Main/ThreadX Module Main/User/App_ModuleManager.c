/*
*********************************************************************************************************
*	                                  
*	模块名称 : 动态加载管理器
*	文件名称 : App_ModuleManager.c
*	版    本 : V1.0
*	说    明 : ThreadX动态加载管理器
*
*	修改记录 :
*		版本号   日期         作者            说明
*       V1.0    2022-04-30   Eric2013         首发
*                                       
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "txm_module.h"



/*
*********************************************************************************************************
*                        MPU配置主程序和动态APP的共享内存，本程序暂未用到
*********************************************************************************************************
*/
typedef enum {
    PROCESSING_NOT_STARTED    = 99,
    WRITING_TO_READWRITE      = 88,
    WRITING_TO_READONLY       = 77,
    READING_FROM_READWRITE    = 66,
    READING_FROM_READONLY     = 55,
    PROCESSING_FINISHED       = 44
} ProgressState;

#define READONLY_REGION            0x24070000
#define READWRITE_REGION           0x24078000
#define SHARED_MEM_SIZE            0xFFFF


/* Define the count of memory faults.  */
ULONG   memory_faults = 0;

VOID pretty_msg(char *p_msg, ULONG r_msg);
VOID module_fault_handler(TX_THREAD *thread, TXM_MODULE_INSTANCE *module);


/*
*********************************************************************************************************
*                                   动态APP地址
*********************************************************************************************************
*/
#define MODULE1_FLASH_ADDRESS       0x08020000    /* 动态APP1地址 */
#define MODULE2_FLASH_ADDRESS       0x08040000    /* 动态APP2地址 */


/*
*********************************************************************************************************
*                                   动态APP管理任务   
*********************************************************************************************************
*/
#define DEFAULT_STACK_SIZE                                 1024
#define MODULE_MANAGER_THREAD_PRIO                         4
#define MODULE_MANAGER_THREAD_PREEMPTION_THRESHOLD         MODULE_MANAGER_THREAD_PRIO

TX_THREAD     ModuleManager;
uint64_t      AppModuleManagerStk[DEFAULT_STACK_SIZE/8];


/*
*********************************************************************************************************
*                                    宏定义
*********************************************************************************************************
*/
TXM_MODULE_INSTANCE     my_module1;          /* 动态APP例化1 */
TXM_MODULE_INSTANCE     my_module2;          /* 动态APP例化2 */
TX_QUEUE                ResidentQueue;       /* 消息队列，用于主程序和动态APP通信 */
uint32_t                MessageQueuesBuf[100]; 

#define MODULE_DATA_SIZE           256*1024   /* 供动态APP使用 */
ALIGN_32BYTES (UCHAR  module_data_area[MODULE_DATA_SIZE]);

#define OBJECT_MEM_SIZE            32*1024    /* 供动态APP的动态内存管理申请使用 */
ALIGN_32BYTES (UCHAR  object_memory[OBJECT_MEM_SIZE]);


/* 任务 */
VOID module_manager_entry(ULONG thread_input);
extern void  App_Printf(const char *fmt, ...);

/*
*********************************************************************************************************
*	函 数 名: module_application_define
*	功能说明: 创建任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void module_application_define(void)
{
    /* 创建动态APP管理器任务  */
    if (tx_thread_create(&ModuleManager, 
                         "Module Manager Thread", 
                         module_manager_entry, 
                         0,
                         &AppModuleManagerStk[0], 
                         DEFAULT_STACK_SIZE,
                         MODULE_MANAGER_THREAD_PRIO, 
                         MODULE_MANAGER_THREAD_PREEMPTION_THRESHOLD,
                         TX_NO_TIME_SLICE, 
                         TX_AUTO_START) != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 创建常驻消息队列 */
    if (tx_queue_create(&ResidentQueue, 
                        "Resident Queue", 
                        TX_1_ULONG,
                        MessageQueuesBuf, 
                        16 * sizeof(ULONG)) != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }   
}

/*
*********************************************************************************************************
*	函 数 名: ModuleManagerLoad
*	功能说明: 加载动态APP
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
VOID ModuleManagerLoad(TXM_MODULE_INSTANCE *module_instance, CHAR *name, uint32_t module_location)
{
    UINT   status;
    CHAR   p_msg[64];
    ULONG  r_msg = PROCESSING_NOT_STARTED;
    ULONG  module_properties;


    /* 加载动态APP */
    status = txm_module_manager_in_place_load(module_instance, name, (VOID *) module_location);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 获取动态APP属性 */
    status = txm_module_manager_properties_get(module_instance, &module_properties);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 打印动态APP信息 */
    App_Printf("===============================================================\n");
    App_Printf("Module <%s> is loaded from address 0x%08X\n", module_instance->txm_module_instance_name, module_location);
    App_Printf("Module code section size: %i bytes, data section size: %i\n", (int)module_instance->txm_module_instance_code_size, (int)module_instance->txm_module_instance_data_size);
    App_Printf("Module Attributes:\n");
    App_Printf("  - Compiled for %s compiler\n", ((module_properties >> 25) == 1)? "CubeIDE (GNU)" : ((module_properties >> 24) == 1)? "ARM KEIL" : "IAR EW");
    App_Printf("  - Shared/external memory access is %s\n", ((module_properties & 0x04) == 0)? "Disabled" : "Enabled");
    App_Printf("  - MPU protection is %s\n", ((module_properties & 0x02) == 0)? "Disabled" : "Enabled");
    App_Printf("  - %s mode execution is enabled for the module\n\n", ((module_properties & 0x01) == 0)? "Privileged" : "User");

    /* 启动动态APP */
    status = txm_module_manager_start(module_instance);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    App_Printf("Module execution is started\n");

    /* 等待消息 */
    while(r_msg != PROCESSING_FINISHED)
    {
        if(tx_queue_receive(&ResidentQueue, &r_msg, TX_TIMER_TICKS_PER_SECOND) == TX_SUCCESS)
        {
            /* 转换消息 */
            pretty_msg(p_msg, r_msg);

            App_Printf("%s is executing: %s\n", name, p_msg);

            /* 发生异常 */
            if(memory_faults)
            {
                App_Printf("A memory fault occurred while module executed: %s\n", p_msg);
                break;
            }
        }
    }
}

/*
*********************************************************************************************************
*	函 数 名: ModuleManagerUnload
*	功能说明: 卸载动态APP
*	形    参: --
*	返 回 值: 无
*********************************************************************************************************
*/
VOID ModuleManagerUnload(TXM_MODULE_INSTANCE *module_instance)
{
    UINT   status;
    
    /* 停止动态APP */
    status = txm_module_manager_stop(module_instance);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 卸载动态APP */
    status = txm_module_manager_unload(module_instance);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    
    App_Printf("===============================================================\r\n");
    App_Printf("==========================Module Unload========================\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: module_manager_entry
*	功能说明: 动态加载管理任务。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern void  DispTaskInfo  (void);
VOID module_manager_entry(ULONG thread_input)
{
    UINT   status;
	uint8_t cmd;

    /* 初始化动态加载管理器，主要是给动态APP的数据空间使用  */
    status = txm_module_manager_initialize((VOID *) module_data_area, MODULE_DATA_SIZE);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 供动态APP使用的对象内存池，主要各种控制块申请 */
    status = txm_module_manager_object_pool_create(object_memory, OBJECT_MEM_SIZE);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 注册faults管理 */
    status = txm_module_manager_memory_fault_notify(module_fault_handler);

    if(status != TX_SUCCESS)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    while(1) 
    {
        if (comGetChar(COM1, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
		{
			switch (cmd)
			{
                /* 加载APP1 */
				case '1':
                    ModuleManagerLoad(&my_module1, "my module1", MODULE1_FLASH_ADDRESS);
					break;

                /* 卸载APP1 */
				case '2':
                    ModuleManagerUnload(&my_module1);
					break;

                 /* 加载APP2 */
				case '3':
                    ModuleManagerLoad(&my_module2, "my module2", MODULE2_FLASH_ADDRESS);
					break;

                /* 卸载APP2 */
				case '4':
                    ModuleManagerUnload(&my_module2);
					break;

                /* 打印任务执行情况 */
				case '5':
                    DispTaskInfo();
					break;
				
				default:
					break;
			}
		}
        
        tx_thread_sleep(1);
    }
}

/*
*********************************************************************************************************
*	函 数 名: module_fault_handler
*	功能说明: 监测faults
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
VOID module_fault_handler(TX_THREAD *thread, TXM_MODULE_INSTANCE *module)
{
    /* 统计错误消息  */
    memory_faults++;
}

/*
*********************************************************************************************************
*	函 数 名: pretty_msg
*	功能说明: 消息明示
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
VOID pretty_msg(char *p_msg, ULONG r_msg)
{
    memset(p_msg, 0, 64);

    switch(r_msg)
    {
        case WRITING_TO_READWRITE:
            memcpy(p_msg, "Writing to ReadWrite Region", 27);
            break;
        
        case WRITING_TO_READONLY:
            memcpy(p_msg, "Writing to ReadOnly Region", 26);
            break;
        
        case READING_FROM_READWRITE:
            memcpy(p_msg, "Reading from ReadWrite Region", 29);
            break;
        
        case READING_FROM_READONLY:
            memcpy(p_msg, "Reading from ReadOnly Region", 28);
            break;
        
        case PROCESSING_FINISHED:
            memcpy(p_msg, "All operations were done", 24);
            break;
        
        default:
            memcpy(p_msg, "Invalid option", 14);
            break;
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
