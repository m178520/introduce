/*主要用于扩展，适配不同的屏幕与按键*/

#include "Touch.h"
#include "GT911.h"
#include "stdio.h"


TOUCH_T g_tTP;

uint8_t g_TouchType;
uint8_t g_LcdType;

static void bsp_DetectLcdType(void);

/*
*********************************************************************************************************
*	函 数 名: TOUCH_InitHard
*	功能说明: 初始化触摸芯片。 再之前，必须先执行 bsp_DetectLcdType() 识别触摸出触摸芯片型号.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_InitHard(void)
{	
	
	//初始化IIC
	bsp_InitI2C();
	
	bsp_DetectLcdType();	/* 自动识别触摸芯片型号 */
	
    g_tTP.Enable = 0;
	
	switch (g_TouchType)
	{
		case CT_GT811:			/* 电容触摸 7寸 */
			#ifdef GT811_ENABLE
			GT811_InitHard();
			#endif
			break;

		case CT_GT911:			/* 电容触摸 7寸 */
			#ifdef GT911_ENABLE
			GT911_InitHard();
			#endif
			break;		
		
		case CT_FT5X06:			/* 电容触摸 4.3寸 */
			#ifdef FT5X06_ENABLE
			FT5X06_InitHard();
			#endif
			break;
		
		case CT_STMPE811:		/* 电阻的 */
			#ifdef STMPE811_ENABLE
			g_tTP.usMaxAdc = 4095;	/* 12位ADC */	
		
			TOUCH_LoadParam();	/* 读取校准参数 */
			g_tTP.Write = g_tTP.Read = 0;
			g_tTP.Enable = 1;
			#endif
			break;
		
		default:
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_PenInt
*	功能说明: 判断触摸按下
*	形    参: 无
*	返 回 值: 0表示无触笔按下，1表示有触笔按下
*********************************************************************************************************
*/
uint8_t TOUCH_PenInt(void)
{
	if (HAL_GPIO_ReadPin(T_PEN_GPIO_Port,T_PEN_Pin) == 0)
	{
		return 1;
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_PutKey
*	功能说明: 将1个触摸点坐标值压入触摸FIFO缓冲区。电阻触摸屏形参是ADC值，电容触摸屏形参是坐标值
*	形    参: _usX, _usY 坐标值
*	返 回 值: 无
*********************************************************************************************************
*/
//extern GX_STUDIO_DISPLAY_INFO TEST_display_table[1];
void TOUCH_PutKey(uint8_t _ucEvent, uint16_t _usX, uint16_t _usY)
{
//	GX_EVENT event;
//	
//	/* 按下, 移动和松手事件 */
//	switch (_ucEvent)
//	{
//		case TOUCH_DOWN:
//			event.gx_event_type = GX_EVENT_PEN_DOWN;
//			event.gx_event_payload.gx_event_pointdata.gx_point_x = _usX;
//			event.gx_event_payload.gx_event_pointdata.gx_point_y = _usY;
//			event.gx_event_sender = 0;
//			event.gx_event_target = 0;
//			event.gx_event_display_handle = 0xC0000000;
//			gx_system_event_send(&event);
//			break;

//		case TOUCH_MOVE:
//			event.gx_event_type = GX_EVENT_PEN_DRAG;
//			event.gx_event_payload.gx_event_pointdata.gx_point_x = _usX;
//			event.gx_event_payload.gx_event_pointdata.gx_point_y = _usY;
//			event.gx_event_sender = 0;
//			event.gx_event_target = 0;
//			event.gx_event_display_handle = 0xC0000000;
//			gx_system_event_fold(&event);
//			break;

//		case TOUCH_RELEASE:
//			event.gx_event_type = GX_EVENT_PEN_UP;
//			event.gx_event_payload.gx_event_pointdata.gx_point_x = _usX;
//			event.gx_event_payload.gx_event_pointdata.gx_point_y = _usY;
//			event.gx_event_sender = 0;
//			event.gx_event_target = 0;
//			event.gx_event_display_handle = 0xC0000000;
//			gx_system_event_send(&event);
//			break;

//		default:
//			break;
//	}
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_CapScan
*	功能说明: I2C接口电容触摸板扫描函数，放在 bsp_Idle()执行！
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_CapScan(int32_t * x, int32_t * y)
{
	#ifdef GT811_ENABLE
	if (g_GT811.Enable == 1)
	{
		GT811_Scan();
		return;
	}
	#endif
	#ifdef FT5X06_ENABLE
//	if (g_tFT5X06.Enable == 1)
//	{
//		FT5X06_Scan();
//		return;
//	}
	#endif
	#ifdef GT911_ENABLE
	if (g_GT911.Enable == 1)
	{
		GT911_Scan(x,y);
		return;
	}
	#endif
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DetectLcdType
*	功能说明: 通过I2C触摸芯片，识别LCD模组类型。结果存放在全局变量 g_LcdType 和 g_TouchType
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectLcdType(void)
{
	uint8_t i;
	
	g_TouchType = 0xFF;
	g_LcdType = 0xFF;
	
	bsp_DelayUS(50000);
	
	printf("正在识别触摸屏型号\r\n");
		/* 50ms，等待GT811复位就绪，才能探测GT811芯片 ID */
	for (i = 0; i < 5; i++)
	{		
		/*
			GT811电容触摸板和GT911的I2C地址相同
			一般就 0x28 、 0xBA 两种。
			通过读取触摸IC的芯片ID来识别。
		*/
		if (i2c_CheckDevice(0x28) == 0)
		{
			uint32_t id;
	
			bsp_DelayUS(500);
			g_GT911.i2c_addr = 0x28;
			id = GT911_ReadID();			
			if (id == 0x00313139)
			{			
				uint16_t MaxX, MaxY;
				
				GT911_ReadMaxXY(&MaxX, &MaxY);
				if (MaxX == 480 && MaxY == 272)
				{
					g_TouchType = CT_GT911;
					g_LcdType = LCD_43_480X272;		
					printf("检测到4.3寸电容触摸屏GT911(0x28) 480x272\r\n");
				}				
				else if(MaxX == 800 && MaxY == 480)
				{					
					g_TouchType = CT_GT911;
					g_LcdType = LCD_70_800X480;
					printf("检测到7.0寸电容触摸屏GT911(0x28) 800x480\r\n");
				}
				else if 
					(MaxX == 1024 && MaxY == 600)
				{					
					g_TouchType = CT_GT911;
					g_LcdType = LCD_70_1024X600;
					printf("检测到7.0寸电容触摸屏GT911(0x28) 1024x600\r\n");
				}
				else
				{
					printf("检测到7.0寸电容触摸屏GT911(0x28),但库中并无此尺寸 尺寸为%d，%d\r\n",MaxX,MaxY);
				}
			}
			else 	/* GT811 */
			{
				
//				g_GT811.i2c_addr = 0x28;
				g_TouchType = CT_GT811;
				g_LcdType = LCD_70_800X480;		
				printf("检测到7.0寸电容触摸屏GT811(0x28) 800x480\r\n");
			}
			break;
		}

		if (i2c_CheckDevice(0xBA) == 0)
		{
			uint32_t id;
	
			bsp_DelayUS(500);
			g_GT911.i2c_addr = 0xBA;
			id = GT911_ReadID();			
			if (id == 0x00313139)
			{			
				uint16_t MaxX, MaxY;
				
				GT911_ReadMaxXY(&MaxX, &MaxY);
				if (MaxX == 480 && MaxY == 272)
				{
					g_TouchType = CT_GT911;
					g_LcdType = LCD_43_480X272;		
					printf("检测到4.3寸电容触摸屏GT911(0x28) 480x272\r\n");
				}				
				else if(MaxX == 800 && MaxY == 480)
				{					
					g_TouchType = CT_GT911;
					g_LcdType = LCD_70_800X480;
					printf("检测到7.0寸电容触摸屏GT911(0x28) 800x480\r\n");
				}
				else if 
					(MaxX == 1024 && MaxY == 600)
				{					
					g_TouchType = CT_GT911;
					g_LcdType = LCD_70_1024X600;
					printf("检测到7.0寸电容触摸屏GT911(0x28) 1024x600\r\n");
				}
				else
				{
					printf("检测到7.0寸电容触摸屏GT911(0x28),但库中并无此尺寸 尺寸为%d，%d\r\n",MaxX,MaxY);
				}
			}
			else 	/* GT811 */
			{
//				g_GT811.i2c_addr = 0xBA;
				g_TouchType = CT_GT811;
				g_LcdType = LCD_70_800X480;		
				printf("检测到7.0寸电容触摸屏GT811(0xBA) 800x480\r\n");
			}
			break;
		}				
		
//		/* FT系列电容触摸触摸 : 4.3寸id = 0x55    5.0寸id = 0x0A  7.0寸id = 0x06 */
//		if (i2c_CheckDevice(FT5X06_I2C_ADDR) == 0)
//		{
//			
//			}
//			break;
//		}

//		/* 电阻触摸板 */		
//		if (i2c_CheckDevice(STMPE811_I2C_ADDRESS) == 0)
//		{
//		
//		}		
		
		bsp_DelayMS(10);
	}
	if (i == 5)
	{
		printf("未识别出显示模块\r\n");
	}
}
