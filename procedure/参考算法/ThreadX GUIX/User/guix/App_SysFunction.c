/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI所需的系统函数
*	文件名称 : App_SysFunction.c
*	版    本 : V1.1
*	说    明 : GUIX的系统API
*	修改记录 :
*		版本号    日期          作者         说明
*		V1.0    2020-06-21    Eric2013       首发
*                                     
*	Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "MainTask.h"



/*
*********************************************************************************************************
*	                                        动态内存分配
*********************************************************************************************************
*/
#define 	  GUI_NUMBYTES     1024*1024*24                        /* 设置动态内存大小 */
#define 	  Canvas_Memory    0xC0400000                          /* 设置Canvas地址   */
TX_BYTE_POOL  memory_pool;
uint8_t       *MemoryBlock = (uint8_t *)(0xC0000000 + 1024*1024*8); /* 动态内存地址    */


/*
*********************************************************************************************************
*	                                        变量
*********************************************************************************************************
*/
extern GX_STUDIO_DISPLAY_INFO guiapp_display_table[1];


/*
*********************************************************************************************************
*	                                       动态内存函数
*********************************************************************************************************
*/
VOID *memory_allocate(ULONG size)
{
    VOID *memptr;

    if (tx_byte_allocate(&memory_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }
    return NULL;
}

void memory_free(VOID *mem)
{
    tx_byte_release(mem);
}

/*
*********************************************************************************************************
*	函 数 名: gx_initconfig
*	功能说明: GUIX
*	形    参: 无   	
*	返 回 值: 无
*********************************************************************************************************
*/
void gx_initconfig(void) 
{
	 /* 初始化内存池 */
    tx_byte_pool_create(&memory_pool, "MemoryBlock",  MemoryBlock,  GUI_NUMBYTES);
	
	/* 初始化GUIX */
    gx_system_initialize();

    /* 注册动态内存申请和释放函数 */
    gx_system_memory_allocator_set(memory_allocate, memory_free);
	
	/* 自适应不同分辨率显示屏 */
	switch (g_LcdType)
	{
		
		case LCD_43_480X272:		/* 4.3寸 480 * 272 */	
		case LCD_50_480X272:		/* 5.0寸 480 * 272 */
			guiapp_display_table[0].x_resolution = 480;
			guiapp_display_table[0].y_resolution = 272;
			break;
		
		case LCD_50_800X480:		/* 5.0寸 800 * 480 */
		case LCD_70_800X480:		/* 7.0寸 800 * 480 */	
			guiapp_display_table[0].x_resolution = 800;
			guiapp_display_table[0].y_resolution = 480;
			break;
		
		default:	
			break;
	}

	guiapp_display_table[0].canvas_memory = (GX_COLOR *)Canvas_Memory;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
