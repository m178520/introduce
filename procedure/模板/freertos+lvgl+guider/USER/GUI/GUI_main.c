#include "GUI_main.h"

#include "cmsis_os2.h"

#include "tim.h"

gg_ui_t guider_ui;

static uint32_t lv_tick_get_cb_rtos(void)
{
    return osKernelGetTickCount();   /* 单位是 tick，不是 ms */
}

static void lv_delay_cb_rtos(uint32_t ms)
{
    if (osKernelGetState() >= osKernelRunning) {
        osDelay(ms);                        /* 参数就是 ms，不用 pdMS_TO_TICKS */
    } else {
        uint32_t start = osKernelGetTickCount();   /* 单位仍是 tick */
        while ((uint32_t)(osKernelGetTickCount() - start) < ms) { }
    }
}

void GUI_main(void)
{
		LCD_SetBackLight(0);
	
		/*Initialize LVGL*/
    lv_init();
		
		/*一定放在初始化后面*/
		/*lvgl时钟*/
		lv_tick_set_cb(lv_tick_get_cb_rtos);
		lv_delay_set_cb(lv_delay_cb_rtos);
	
    /*Initialize the display and input devices*/
	
    lv_port_disp_init();
		lv_port_indev_init();
	
    setup_ui(&guider_ui);

		osDelay(300);
		LCD_SetBackLight(255);
    while(1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
        osDelay(5);
    }
}
