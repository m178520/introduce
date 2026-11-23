#include "IIC.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_InitI2C
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void PCF8574T_InitI2C(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOH_CLK_ENABLE();
	
  /*Configure GPIO pins : T_SCK_Pin T_PEN_Pin */
  GPIO_InitStruct.Pin = PCF8574T_IIC_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PCF8574T_IIC_SCL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : T_M0SI_Pin */
  GPIO_InitStruct.Pin = PCF8574T_IIC_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PCF8574T_IIC_SDA_GPIO_Port, &GPIO_InitStruct);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	PCF8574T_i2c_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void PCF8574T_i2c_Delay(uint32_t ns)
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
	bsp_DelayUS(ns);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void PCF8574T_i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	PCF8574T_SDA_OUT();
	PCF8574T_IIC_SDA(1);
	PCF8574T_IIC_SCL(1);
	PCF8574T_i2c_Delay(4);
	PCF8574T_IIC_SDA(0);
	PCF8574T_i2c_Delay(4);
	PCF8574T_IIC_SCL(0);
}
/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void PCF8574T_i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	PCF8574T_SDA_OUT();//sda线输出
	PCF8574T_IIC_SCL(0);
	PCF8574T_IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	PCF8574T_i2c_Delay(4);
	PCF8574T_IIC_SCL(1); 
	PCF8574T_IIC_SDA(1);//发送I2C总线结束信号
	PCF8574T_i2c_Delay(4);	
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void PCF8574T_i2c_SendByte(uint8_t _ucByte)
{
	uint8_t t,i;
	PCF8574T_SDA_OUT(); 
	PCF8574T_IIC_SCL(0);//拉低时钟开始数据传输
	/* 先发送字节的高位bit7 */
    for(t=0;t<8;t++)
    {              
			PCF8574T_IIC_SDA((_ucByte&0x80)>>7);
			_ucByte<<=1; 	  
			PCF8574T_i2c_Delay(2);   //对TEA5767这三个延时都是必须的
			PCF8574T_IIC_SCL(1);
			PCF8574T_i2c_Delay(2); 
			PCF8574T_IIC_SCL(0);	
			PCF8574T_i2c_Delay(2);
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
uint8_t PCF8574T_i2c_ReadByte(unsigned char ack)
{
	unsigned char i,receive=0;
	PCF8574T_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
		PCF8574T_IIC_SCL(0); 
		PCF8574T_i2c_Delay(2);
		PCF8574T_IIC_SCL(1);
		receive<<=1;
		if(PCF8574T_READ_SDA)receive++;   
		PCF8574T_i2c_Delay(1); 
    }					 
    if (!ack)
        PCF8574T_i2c_NAck();//发送nACK
    else
        PCF8574T_i2c_Ack(); //发送ACK   
    return receive;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参:  无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t PCF8574T_i2c_WaitAck(void)
{
	uint8_t ucErrTime=0;
	PCF8574T_SDA_IN();      //SDA设置为输入  
	PCF8574T_IIC_SDA(1);PCF8574T_i2c_Delay(1);	   
	PCF8574T_IIC_SCL(1);PCF8574T_i2c_Delay(1);	 
	while(PCF8574T_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			PCF8574T_i2c_Stop();
			return 1;
		}
	}
	PCF8574T_IIC_SCL(0);//时钟输出0 	   
	return 0;  
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void PCF8574T_i2c_Ack(void)
{
	PCF8574T_IIC_SCL(0);
	PCF8574T_SDA_OUT();
	PCF8574T_IIC_SDA(0);
	PCF8574T_i2c_Delay(2);
	PCF8574T_IIC_SCL(1);
	PCF8574T_i2c_Delay(2);
	PCF8574T_IIC_SCL(0);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void PCF8574T_i2c_NAck(void)
{
	PCF8574T_IIC_SCL(0);
	PCF8574T_SDA_OUT();
	PCF8574T_IIC_SDA(1);
	PCF8574T_i2c_Delay(2);
	PCF8574T_IIC_SCL(1);
	PCF8574T_i2c_Delay(2);
	PCF8574T_IIC_SCL(0);
}

