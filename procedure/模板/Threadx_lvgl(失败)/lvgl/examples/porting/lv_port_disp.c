/**
 * @file lv_port_disp_template.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "fmc.h"
#include "ltdc.h"
#include "dma2d.h"

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
//    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    1024
#endif

#ifndef MY_DISP_VER_RES
//    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    600
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */
#define REN_ROW_NUM     40

#define FrameBufer   SDRAM_LCD_BUF1   // LTDC硬件帧缓存地址

//#define 	  Canvas_Memory    0xC0400000                          /* 设置Canvas地址   */
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/* 定义在32MB SDRAM(0xC0400000)里面的变量 */
__attribute__((section (".bss.SDRAM_D2"), aligned(8))) uint8_t Canvas_Memory[MY_DISP_HOR_RES * REN_ROW_NUM * BYTE_PER_PIXEL];

/**********************
 *      MACROS
 **********************/


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);

    /* Example 1
     * One buffer for partial rendering*/
		//设置画布
//    LV_ATTRIBUTE_MEM_ALIGN
    lv_display_set_buffers(disp, Canvas_Memory, NULL, MY_DISP_HOR_RES * REN_ROW_NUM * BYTE_PER_PIXEL, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Example 2
     * Two buffers for partial rendering
     * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_2_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];

//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_2_2[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];
//    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

//    /* Example 3
//     * Two buffers screen sized buffer for double buffering.
//     * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];

//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
//    lv_display_set_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISPLAY_RENDER_MODE_DIRECT);
		lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
	
	MX_DMA2D_Init();//DMA2D初始化
	MX_LTDC_Init(); //屏幕初始化
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
			// px_map 指向你那块小画布Canvas_Memory，LVGL已经渲染好这一小块
			uint32_t src_addr = (uint32_t)px_map;
			
			uint32_t dst_addr = (uint32_t)FrameBufer
                        + (area->y1 * MY_DISP_HOR_RES * 2U)
                        + (area->x1 * 2U);
			
		int32_t copy_w = area->x2 - area->x1 + 1;
    int32_t copy_h = area->y2 - area->y1 + 1;
			
		
		DMA2D->CR = DMA2D_M2M;
		DMA2D->FGMAR = src_addr;
    DMA2D->FGOR = 0;
    DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565 ;

    DMA2D->OMAR = dst_addr;
    DMA2D->OOR = MY_DISP_HOR_RES - copy_w;   // 目标行偏移，屏幕宽度减去拷贝宽度
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

    DMA2D->NLR = (((uint32_t)copy_w) << 16U) | (uint16_t)copy_h;

    // H7关键：清理DCache，SDRAM外部内存
    SCB_CleanInvalidateDCache();

    DMA2D->CR |= DMA2D_CR_START;

    // 调试用轮询等待DMA完成；产品建议换成DMA2D中断
    while((DMA2D->CR & DMA2D_CR_START) != 0);

    }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
