/**
 * @file lv_mem_core_rtthread.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_STDLIB_MALLOC == LV_OS_CUSTOM
//#include <rtthread.h>
#include "app_threadx.h"

//#ifndef RT_USING_HEAP
//    #error "lv_mem_core_threadx: RT_USING_HEAP is required. Define it in rtconfig.h"
//#endif

/*********************
 *      DEFINES
 *********************/
     
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
#define TX_LVGL_MEM_POOL_SIZE            1024*1024*24                        /* 设置动态内存大小 */     
//uint8_t       *MemoryBlock = (uint8_t *)(0xC0000000 + 1024*1024*8); /* 动态内存地址    */

uint8_t MemoryBlock[TX_LVGL_MEM_POOL_SIZE] __attribute__((section(".bss.SDRAM_D3"), aligned(8)));

TX_BYTE_POOL tx_lvgl_byte_pool;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_mem_init(void)
{
	/* 初始化内存池 */
	if (tx_byte_pool_create(&tx_lvgl_byte_pool, "MemoryBlock",  MemoryBlock,  TX_LVGL_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    /* USER CODE BEGIN TX_Byte_Pool_Error */
		return;
    /* USER CODE END TX_Byte_Pool_Error */
  }
     /*Nothing to init*/
}

void lv_mem_deinit(void)
{
    return; /*Nothing to deinit*/
}

lv_mem_pool_t lv_mem_add_pool(void * mem, size_t bytes)
{
    /*Not supported*/
//		LV_UNUSED(mem);
//		LV_UNUSED(bytes);
		 
    return &tx_lvgl_byte_pool;
}

void lv_mem_remove_pool(lv_mem_pool_t pool)
{
//    /*Not supported*/
//    LV_UNUSED(pool);
	tx_byte_pool_delete(&tx_lvgl_byte_pool);
    return;
}

void * lv_malloc_core(size_t size)
{
	void *memory_ptr;
	if(tx_byte_allocate(&tx_lvgl_byte_pool,&memory_ptr,size,TX_NO_WAIT) == TX_SUCCESS)
	{
		return memory_ptr;
	}
    return NULL;
}

void * lv_realloc_core(void * p, size_t new_size)
{
  void *new_ptr;
	if (new_size == 0)
    {
       if(p != NULL)
       {
           tx_byte_release(p);
       }
       return NULL;
   }
	if(p == NULL)
   {
		if(tx_byte_allocate(&tx_lvgl_byte_pool, &new_ptr, new_size, TX_NO_WAIT) == TX_SUCCESS)
    return new_ptr;
   }

    // 3. 正常重分配：新开一块 + memcpy + 释放旧块
    if(tx_byte_allocate(&tx_lvgl_byte_pool, new_ptr, new_size, TX_NO_WAIT) == TX_SUCCESS)
    {
			tx_byte_release(p);
			return new_ptr;
    }
		return NULL;
		
}

void lv_free_core(void * p)
{
    if (p == NULL)
    {
        return;
    }
    tx_byte_release(p);
}

void lv_mem_monitor_core(lv_mem_monitor_t * mon_p)
{
    /*Not supported*/
    LV_UNUSED(mon_p);
    return;
}

lv_result_t lv_mem_test_core(void)
{
    /*Not supported*/
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_STDLIB_RTTHREAD*/
