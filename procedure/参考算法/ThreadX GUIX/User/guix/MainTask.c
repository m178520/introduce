/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : LCD界面
*              
*	修改记录 :
*		版本号   日期         作者          说明
*		V1.0    2020-07-01   Eric2013  	    首版    
*                                     
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "MainTask.h"



/*
*********************************************************************************************************
*                                               宏定义
*********************************************************************************************************
*/
#define GUI_ID_Timer0   10


/*
*********************************************************************************************************
*                                               变量
*********************************************************************************************************
*/
GX_WINDOW 		*pScreen;
GX_WINDOW_ROOT  *root;

/* 用于星期的显示 */
static const char *ucWeekDay[7] =
{
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

/*
*********************************************************************************************************
*	函 数 名: _cbEventWindow
*	功能说明: 窗口window的事件回调函数
*	形    参: widget     窗口句柄 
*             event_ptr  事件指针
*	返 回 值: 返回0表示成功
*********************************************************************************************************
*/
UINT _cbEventWindow_1(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
	char buf[30];
	
    switch (event_ptr->gx_event_type)
    {
      /* 控件显示事件 */
        case GX_EVENT_SHOW:
        
            /* 启动一个GUIX定时器 */
            gx_system_timer_start((GX_WIDGET *)widget, GUI_ID_Timer0, 1, 800);

            /* 默认事件处理 */
            gx_window_event_process(widget, event_ptr);
            break;

        /* 定时器时间溢出事件*/
        case GX_EVENT_TIMER:
            if (event_ptr->gx_event_payload.gx_event_timer_id == GUI_ID_Timer0)
            {
				RTC_ReadClock();
				
				sprintf(buf, 
						  "%04d/%02d/%02d", 
						  g_tRTC.Year, 
						  g_tRTC.Mon, 
						  g_tRTC.Day);
				gx_prompt_text_set((GX_PROMPT *)&(window_1.window_1_prompt_5), buf);
				
				sprintf(buf, 
						"%02d:%02d:%02d %s", 
						g_tRTC.Hour, 
						g_tRTC.Min,
						g_tRTC.Sec, 
						ucWeekDay[g_tRTC.Week-1]);
				gx_prompt_text_set((GX_PROMPT *)&(window_1.window_1_prompt_6), buf);	
            }
            break;

        default:
            return gx_window_event_process(widget, event_ptr);
    }

    return 0;
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
	
	/*
       触摸校准函数默认是注释掉的，电阻屏需要校准，电容屏无需校准。如果用户需要校准电阻屏的话，执行
	   此函数即可，会将触摸校准参数保存到EEPROM里面，以后系统上电会自动从EEPROM里面加载。
	*/
#if 0
	LCD_SetBackLight(255);
	LCD_InitHard();
    TOUCH_Calibration(2);
#endif
	
	/*初始化配置 */
	gx_initconfig();

	/* 配置显示屏 */
    gx_studio_display_configure(DISPLAY_1, stm32h7_graphics_driver_setup_565rgb,
        LANGUAGE_ENGLISH, DISPLAY_1_THEME_1, &root);
	
    /* 创建窗口 
	  显存 - > canvas画布 - > root window  -> window/widget
      root window 是创建到canvas上的容器，可以认为就是类似emWin的桌面窗口，这样都创建后，一般我们各种窗口控件都可以创建到root window上
      就跟使用emWin一样，将各种窗口创建到桌面窗口。
	*/
	/* 同一个父窗口下，禁止聚焦后，先后顺序显示，如果使能聚焦，聚焦窗口将再最前面显示  */
    gx_studio_named_widget_create("window", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);
	gx_studio_named_widget_create("window_1", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);
	
	gx_studio_named_widget_create("window_2", (GX_WIDGET *)NULL, (GX_WIDGET **)&pScreen);
	

	/* 显示根窗口 */
    gx_widget_show(root);

    /* 启动GUIX */
    gx_system_start();
	
	tx_thread_sleep(300);
	LCD_SetBackLight(255);
	
	while(1)
	{
		tx_thread_sleep(20);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
