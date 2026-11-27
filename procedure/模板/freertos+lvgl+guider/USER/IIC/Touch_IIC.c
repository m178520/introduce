#include "user_global.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_InitI2C
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void Touch_InitI2C(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
	
  /*Configure GPIO pins : T_SCK_Pin T_PEN_Pin */
  GPIO_InitStruct.Pin = T_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(T_SCK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : T_M0SI_Pin */
  GPIO_InitStruct.Pin = T_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(T_MOSI_GPIO_Port, &GPIO_InitStruct);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	Touch_i2c_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void Touch_i2c_Delay(void)
{
	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	//for (i = 0; i < 30; i++);
	//for (i = 0; i < 60; i++);
	//bsp_DelayUS(2); 229.57KHz时钟
	bsp_DelayUS(2);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void Touch_i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	Touch_IIC_SDA_H;
	Touch_IIC_SCL_H;
	Touch_i2c_Delay();
	Touch_IIC_SDA_L;
	Touch_i2c_Delay();
	
	Touch_IIC_SCL_L;
	Touch_i2c_Delay();
}
/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void Touch_i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	Touch_IIC_SDA_L;
	Touch_i2c_Delay();
	Touch_IIC_SCL_H;
	Touch_i2c_Delay();
	Touch_IIC_SDA_H;
	Touch_i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void Touch_i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			Touch_IIC_SDA_H;
		}
		else
		{
			Touch_IIC_SDA_L;
		}
		Touch_i2c_Delay();
		Touch_IIC_SCL_H;
		Touch_i2c_Delay();
		Touch_IIC_SCL_L;
//		IIC_SCL_L;	/* 2019-03-14 针对GT811电容触摸，添加一行，相当于延迟几十ns */
		if (i == 7)
		{
			 Touch_IIC_SDA_H; // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */	
	}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参:  无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t Touch_i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		Touch_IIC_SCL_H;
		Touch_i2c_Delay();
		if (Touch_IIC_SDA_READ)
		{
			value++;
		}
		Touch_IIC_SCL_L;
		Touch_i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参:  无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t Touch_i2c_WaitAck(void)
{
	uint8_t re;

	Touch_IIC_SDA_H;	/* CPU释放SDA总线 */
	Touch_i2c_Delay();
	Touch_IIC_SCL_H;	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	Touch_i2c_Delay();
	if (Touch_IIC_SDA_READ)	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	Touch_IIC_SCL_L;
	Touch_i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void Touch_i2c_Ack(void)
{
	Touch_IIC_SDA_L;	/* CPU驱动SDA = 0 */
	Touch_i2c_Delay();
	Touch_IIC_SCL_H;	/* CPU产生1个时钟 */
	Touch_i2c_Delay();
	Touch_IIC_SCL_L;
	Touch_i2c_Delay();
	Touch_IIC_SDA_H;	/* CPU释放SDA总线 */
	
	Touch_i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void Touch_i2c_NAck(void)
{
	Touch_IIC_SDA_H;	/* CPU驱动SDA = 1 */
	Touch_i2c_Delay();
	Touch_IIC_SCL_H;	/* CPU产生1个时钟 */
	Touch_i2c_Delay();
	Touch_IIC_SCL_L;
	Touch_i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参:  _Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t Touch_i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	if (Touch_IIC_SDA_READ && Touch_IIC_SCL_READ)
	{
		Touch_i2c_Start();		/* 发送启动信号 */

		/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
		Touch_i2c_SendByte(_Address | I2C_WR);
		ucAck = Touch_i2c_WaitAck();	/* 检测设备的ACK应答 */

		Touch_i2c_Stop();			/* 发送停止信号 */

		return ucAck;
	}
	return 1;	/* I2C总线异常 */
}
