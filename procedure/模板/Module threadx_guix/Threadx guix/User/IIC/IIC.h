#ifndef __IIC_H
#define __IIC_H

#include "main.h"
#include "dwt.h"

#define I2C_WR	0		/* Ð´¿ØÖÆbit */
#define I2C_RD	1		/* ¶Á¿ØÖÆbit */

#define IIC_SCL_H  HAL_GPIO_WritePin(T_SCK_GPIO_Port,T_SCK_Pin,GPIO_PIN_SET)
#define IIC_SCL_L  HAL_GPIO_WritePin(T_SCK_GPIO_Port,T_SCK_Pin,GPIO_PIN_RESET)

#define IIC_SDA_H  HAL_GPIO_WritePin(T_MOSI_GPIO_Port,T_MOSI_Pin,GPIO_PIN_SET)
#define IIC_SDA_L  HAL_GPIO_WritePin(T_MOSI_GPIO_Port,T_MOSI_Pin,GPIO_PIN_RESET)

#define IIC_SCL_READ HAL_GPIO_ReadPin(T_SCK_GPIO_Port,T_SCK_Pin)
#define IIC_SDA_READ HAL_GPIO_ReadPin(T_MOSI_GPIO_Port,T_MOSI_Pin)


void bsp_InitI2C(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);



#endif
