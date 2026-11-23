#include "user_global.h"
#include "rtc.h"

TX_MUTEX   PrintfSemp;	/* 用于printf互斥 */

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
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
void  RTC_LOG_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1]; /* 特别注意，如果printf的变量较多，注意此局部变量的大小是否够用 */
    va_list   v_args;

		
    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

		char buf[64];
    if(g_tRTC.Enable == 1)
    {
        RTC_ReadClock(); // 填充 g_tRTC.Year/Mon/Day/Hour/Min/Sec/Week
        sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d %s ",
                g_tRTC.Year,
                g_tRTC.Mon,
                g_tRTC.Day,
                g_tRTC.Hour,
                g_tRTC.Min,
                g_tRTC.Sec,
                ucWeekDay[g_tRTC.Week - 1]);
    }
		else
    {
        buf[0] = '\0'; // RTC未就绪，前缀为空
    }
									 
	/* 互斥操作 */
    tx_mutex_get(&PrintfSemp, TX_WAIT_FOREVER);

    printf("%s%s", buf, buf_str);

    tx_mutex_put(&PrintfSemp);
}
