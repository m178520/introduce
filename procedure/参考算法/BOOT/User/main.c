/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : LCD小字库的实现和2D图形显示。
*              实验目的：
*                1. 学习LCD的小字库实现和2D图形显示。
*              实验内容：
*                1、小字库通过此软件生成：
*                   http://www.armbbs.cn/forum.php?mod=viewthread&tid=202 。 
*                2、LCD界面上显示了汉字和2D图形。
*                3、启动1个200ms的自动重装定时器，让LED2每200ms翻转一次。
*                4、同时在LCD界面上实现一个简单计数，每200ms加1，计数到255后继续从0开始。            
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT查看打印信息，波特率115200，数据位8，奇偶校验位无，停止位1。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2020-08-07   Eric2013     1. CMSIS软包版本 V5.7.0
*                                         2. HAL库版本 V1.9.0
*                                         
*	Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "bsp.h"			 /* 底层硬件驱动 */


/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V7-LCD的汉字显示和2D图形显示（小字库）"
#define EXAMPLE_DATE	"2020-08-09"
#define DEMO_VER		"1.0"


static void PrintfLogo(void);
static void JumpToApp(void);


/* 设置未初始化变量 */
#if defined (__ICCARM__)  /* IAR */

#pragma location = ".NoInit"  
uint32_t g_JumpInit;

#elif defined(__CC_ARM)   /* MDK AC5 */

uint32_t g_JumpInit __attribute__((at(0x20000000), zero_init)); 
 
#elif  (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* MDK AC6 */

uint32_t g_JumpInit __attribute__( ( section( ".bss.NoInit"))); 

#elif  defined(__GNUC__)      /* GCC */

__attribute__((section( ".NoInit"))) uint32_t g_JumpInit;

#endif

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参: 无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;
	
	FONT_T tFont12;			    /* 定义一个字体结构体变量，用于设置字体参数 */
	FONT_T tFont16;				/* 定义一个字体结构体变量，用于设置字体参数 */
	uint8_t buf[100], count = 0;
	
	//////////////////变量操作//////////////////////////////
	if (g_JumpInit == 0xAA553344)	        /* 软件复位后再进入APP，提供一个干净的CPU环境给APP */
	{	
		JumpToApp();	                    /* 去执行APP程序 */
	}
	else if (g_JumpInit == 0x5AA51234)	    
	{
		
		/* 用户可以自己加处理，比如APP请求升级固件, 直接进入固件升级状态 */
	}
	else
	{
		/* 用户可以自己加处理 */
	}
	//////////////////////////////////////////////////////
	

	/* 设置字体参数 */
	{
		tFont12.FontCode = FC_ST_12;	    /* 字体代码 12点阵 */
		tFont12.FrontColor = CL_WHITE;		/* 字体颜色 */
		tFont12.BackColor = CL_BLUE;	    /* 文字背景颜色 */
		tFont12.Space = 0;					/* 文字间距，单位 = 像素 */
	}
	
	/* 设置字体参数 */
	{
		tFont16.FontCode = FC_ST_16;	    /* 字体代码 16点阵 */
		tFont16.FrontColor = CL_WHITE;		/* 字体颜色 */
		tFont16.BackColor = CL_BLUE;	    /* 文字背景颜色 */
		tFont16.Space = 0;					/* 文字间距，单位 = 像素 */
	}
	
	
	bsp_Init();		/* 硬件初始化 */
	PrintfLogo();	/* 打印例程名称和版本等信息 */

	/* 延迟200ms再点亮背光，避免瞬间高亮 */
	bsp_DelayMS(200); 
	
	/* 清屏 */
	LCD_ClrScr(CL_BLUE);

	/* 显示汉字 */
	LCD_DispStr(5, 3, "故人西辞黄鹤楼，烟花三月下扬州。", &tFont12); 
	LCD_DispStr(5, 18, "孤帆远影碧空尽，唯见长江天际流。", &tFont12); 
	LCD_DispStr(5, 38, "故人西辞黄鹤楼，烟花三月下扬州。", &tFont16); 
	LCD_DispStr(5, 58, "孤帆远影碧空尽，唯见长江天际流。", &tFont16); 
	
	/* 绘制2D图形 */
	LCD_DrawLine(5, 120, 100, 220, CL_RED);
	LCD_DrawRect(120, 120, 100, 100, CL_RED);
	LCD_DrawCircle(280, 170, 50, CL_RED);
	LCD_Fill_Rect (340, 120, 100, 100, CL_BUTTON_GREY);
	
	/* 界面整体显示完毕后，再打开背光，设置为缺省亮度 */
	bsp_DelayMS(100); 
	LCD_SetBackLight(BRIGHT_DEFAULT);	
	
	bsp_StartAutoTimer(0, 200); /* 启动1个200ms的自动重装的定时器，软件定时器0 */
	
	while (1)
	{
		/* 判断软件定时器0是否超时 */
		if(bsp_CheckTimer(0))
		{
			/* 每隔200ms 进来一次 */  
			bsp_LedToggle(4);
			
			sprintf((char *)buf, "count = %03d", count++);
			LCD_DispStr(5, 90, (char *)buf, &tFont16); 
		}
		
		ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* 有键按下 */
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:		/* K1按键按下 */
					g_JumpInit = 0xAA553344;
					NVIC_SystemReset();	/* 复位CPU */
					break;

				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: JumpToApp
*	功能说明: 跳转到APP
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void JumpToApp(void)
{
#if 1
	void (*SysMemBootJump)(void);        /* 声明一个函数指针 */
	__IO uint32_t BootAddr = 0x08100000; /* STM32H7的系统BootLoader地址 */

	g_JumpInit = 0xAA553344;

	/* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
	SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

	/* 设置主堆栈指针 */
	__set_MSP(*(uint32_t *)BootAddr);
	

	/* 跳转到系统BootLoader */
	SysMemBootJump(); 

	/* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
	while (1)
	{

	}
#else
	uint32_t i=0;
	void (*SysMemBootJump)(void);        /* 声明一个函数指针 */
	__IO uint32_t BootAddr = 0x08100000; /* STM32H7的系统BootLoader地址 */
	

	/* 关闭全局中断 */
	DISABLE_INT(); 

	/* 关闭滴答定时器，复位到默认值 */
	SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

	/* 设置所有时钟到默认状态，使用HSI时钟 */
	HAL_RCC_DeInit();

	/* 关闭所有中断，清除所有中断挂起标志 */
	for (i = 0; i < 8; i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}	

	/* 使能全局中断 */
	ENABLE_INT();

	/* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
	SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

	/* 设置主堆栈指针 */
	__set_MSP(*(uint32_t *)BootAddr);
	
	/* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
	__set_CONTROL(0);

	/* 跳转到系统BootLoader */
	SysMemBootJump(); 

	/* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
	while (1)
	{

	}
	
#endif
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	
	/* 检测CPU ID */
	{
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;
		
		CPU_Sn0 = *(__IO uint32_t*)(0x1FF1E800);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FF1E800 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FF1E800 + 8);

		printf("\r\nCPU : STM32H743XIH6, BGA240, 主频: %dMHz\r\n", SystemCoreClock / 1000000);
		printf("UID = %08X %08X %08X\n\r", CPU_Sn2, CPU_Sn1, CPU_Sn0);
	}

	printf("\n\r");
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST的HAL库版本 */
	printf("* HAL库版本  : V1.9.0 (STM32H7xx HAL Driver)\r\n");
	printf("* \r\n");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* 旺旺  : armfly\r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* 微信公众号: armfly_com \r\n");
	printf("* 淘宝店: armfly.taobao.com\r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
