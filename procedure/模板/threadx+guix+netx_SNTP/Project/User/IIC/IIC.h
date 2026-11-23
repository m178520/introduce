#ifndef __IIC_H
#define __IIC_H

#include "main.h"

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */

//开漏时直接SDA可读
#define Touch_IIC_SCL_H  HAL_GPIO_WritePin(T_SCK_GPIO_Port,T_SCK_Pin,GPIO_PIN_SET)
#define Touch_IIC_SCL_L  HAL_GPIO_WritePin(T_SCK_GPIO_Port,T_SCK_Pin,GPIO_PIN_RESET)

#define Touch_IIC_SDA_H  HAL_GPIO_WritePin(T_MOSI_GPIO_Port,T_MOSI_Pin,GPIO_PIN_SET)
#define Touch_IIC_SDA_L  HAL_GPIO_WritePin(T_MOSI_GPIO_Port,T_MOSI_Pin,GPIO_PIN_RESET)

#define Touch_IIC_SCL_READ HAL_GPIO_ReadPin(T_SCK_GPIO_Port,T_SCK_Pin)
#define Touch_IIC_SDA_READ HAL_GPIO_ReadPin(T_MOSI_GPIO_Port,T_MOSI_Pin)

//推挽输出时需手动更改I/O状态
//IO方向设置
#define PCF8574T_SDA_IN()  {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=0<<5*2;}	//PH5输入模式
#define PCF8574T_SDA_OUT() {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=1<<5*2;} //PH5输出模式
//IO操作
#define PCF8574T_IIC_SCL(n)  (n?HAL_GPIO_WritePin(PCF8574T_IIC_SCL_GPIO_Port,PCF8574T_IIC_SCL_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(PCF8574T_IIC_SCL_GPIO_Port,PCF8574T_IIC_SCL_Pin,GPIO_PIN_RESET)) //SCL
#define PCF8574T_IIC_SDA(n)  (n?HAL_GPIO_WritePin(PCF8574T_IIC_SDA_GPIO_Port,PCF8574T_IIC_SDA_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(PCF8574T_IIC_SDA_GPIO_Port,PCF8574T_IIC_SDA_Pin,GPIO_PIN_RESET)) //SDA
#define PCF8574T_READ_SDA    HAL_GPIO_ReadPin(PCF8574T_IIC_SDA_GPIO_Port,PCF8574T_IIC_SDA_Pin)  //输入SDA

void Touch_InitI2C(void);
void Touch_i2c_Start(void);
void Touch_i2c_Stop(void);
void Touch_i2c_SendByte(uint8_t _ucByte);
uint8_t Touch_i2c_ReadByte(void);
uint8_t Touch_i2c_WaitAck(void);
void Touch_i2c_Ack(void);
void Touch_i2c_NAck(void);
uint8_t Touch_i2c_CheckDevice(uint8_t _Address);

void PCF8574T_InitI2C(void);
void PCF8574T_i2c_Start(void);
void PCF8574T_i2c_Stop(void);
void PCF8574T_i2c_SendByte(uint8_t _ucByte);
uint8_t PCF8574T_i2c_ReadByte(unsigned char ack);
uint8_t PCF8574T_i2c_WaitAck(void);
void PCF8574T_i2c_Ack(void);
void PCF8574T_i2c_NAck(void);
uint8_t PCF8574T_i2c_CheckDevice(uint8_t _Address);

#endif
