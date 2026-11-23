#ifndef __GT911_H
#define __GT911_H

#define GT911_ENABLE

#include "IIC.h"

#define GT911_IIC_ADDR1	0xBA

#define GT911_READ_XY_REG    0x814E /* 坐标寄存器 */ 
#define GT911_CLEARBUF_REG   0x814E /* 清除坐标寄存器 */ 
#define GT911_CONFIG_REG     0x8047 /* 配置参数寄存器 */ 
#define GT911_COMMAND_REG    0x8040 /* 实时命令 */ 
#define GT911_PRODUCT_ID_REG 0x8140 /* 芯片ID */ 
#define GT911_VENDOR_ID_REG  0x814A /* 当前模组选项信息 */ 
#define GT911_CONFIG_VERSION_REG   0x8047 /* 配置文件版本号 */ 
#define GT911_CONFIG_CHECKSUM_REG  0x80FF /* 配置文件校验码 */ 
#define GT911_FIRMWARE_VERSION_REG 0x8144 /* 固件版本号 */ 


#define IIC_RST  HAL_GPIO_WritePin(T_CS_GPIO_Port,T_CS_Pin,GPIO_PIN_SET)

#define IIC_PEN  HAL_GPIO_ReadPin(T_PEN_GPIO_Port,T_PEN_Pin)



void GT911_InitHard(void);
uint32_t GT911_ReadID(void);
uint16_t GT911_ReadVersion(void);
void GT911_Scan(void);
void GT911_ReadMaxXY(uint16_t *_X, uint16_t *_Y);

typedef struct
{
	uint8_t Enable;
	uint8_t i2c_addr;
	
	uint8_t TouchpointFlag;
	uint8_t Touchkeystate;

	uint16_t X0;
	uint16_t Y0;
	uint16_t P0;

	uint16_t X1;
	uint16_t Y1;
	uint16_t P1;

	uint16_t X2;
	uint16_t Y2;
	uint16_t P2;

	uint16_t X3;
	uint16_t Y3;
	uint16_t P3;

	uint16_t X4;
	uint16_t Y4;
	uint16_t P4;
}GT911_T;

extern GT911_T g_GT911;


#endif


