#include "MainTask.h"
#include "tim.h"
#include "app_threadx.h"
static uint32_t lvgl_tick_from_tx(void)
{
    ULONG tx_ticks = tx_time_get();
    // 换算成毫秒，这一步是核心！tx_time_get拿到的是节拍，不是ms
    return (uint32_t)tx_ticks;
}
/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{
	/* 避免上电后瞬间的撕裂感 */
	LCD_SetBackLight(0);
	
	lv_tick_set_cb(lvgl_tick_from_tx);
	
	/*初始化配置 */
	lv_init();
	

	/* 配置显示屏 */
  lv_port_disp_init();
	lv_port_indev_init();
	
	
    /* 创建窗口 */
	ui_init();
	
	tx_thread_sleep(300);
	LCD_SetBackLight(255);
	
	while(1)
	{
		lv_timer_handler();
		tx_thread_sleep(1);
	}
}
