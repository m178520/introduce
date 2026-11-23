#调试lvgl遇到的坑

##freertos 中硬件中断的优先级
freertos 中硬件中断的优先级必须大于8，不然会卡死，freertos做的保护，高于8的抢断任务freertos认为会有危险，
就主动卡死

```test
为什么是8呢
STM32 实际只实现高 4 位优先级（大部分芯片），低 4 位是无效保留位：
CubeMX 配置 `n` → 寄存器值 = `n << 4`
- CubeMX 优先级 0 → 寄存器值 = `0 << 4 = 0`  → 0 < 128 → 触发 ASSERT
- CubeMX 优先级 7 → 寄存器值 = `7 << 4 = 112` → 112 < 128 → 触发 ASSERT
- CubeMX 优先级 8 → 寄存器值 = `8 << 4 = 128` → 128 >=128 ✅ 合法
- CubeMX 优先级 10 → `10 <<4 = 160` → 160 >=128 ✅ 合法
```

##硬件中断配置
>LDTC与DMA2D的中断必须都打开，有一些同步的信号量需要在中断中触发

##I/O巨坑
>LTDC配置时一定需要将其I/O口配置为very HIGH的模式

##FMC巨坑
>fmc的突发必须使能，不然会白色条纹


##fmc还需要将sdram芯片初始化序列
```c
#define SDRAM_TIMEOUT                    ((uint32_t)0xFFFF)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/*
*********************************************************************************************************
*	函 数 名: SDRAM初始化序列
*	功能说明: 完成SDRAM序列初始化
*	形    参: hsdram: SDRAM句柄
*			  Command: 命令结构体指针
*	返 回 值: None
*********************************************************************************************************
*/
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
	__IO uint32_t tmpmrd =0;
 
    /*##-1- 时钟使能命令 ##################################################*/
	Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	Command->AutoRefreshNumber = 1;
	Command->ModeRegisterDefinition = 0;

	/* 发送命令 */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-2- 插入延迟，至少100us ##################################################*/
	HAL_Delay(1);

    /*##-3- 整个SDRAM预充电命令，PALL(precharge all) #############################*/
	Command->CommandMode = FMC_SDRAM_CMD_PALL;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	Command->AutoRefreshNumber = 1;
	Command->ModeRegisterDefinition = 0;

	/* 发送命令 */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-4- 自动刷新命令 #######################################################*/
	Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	Command->AutoRefreshNumber = 8;
	Command->ModeRegisterDefinition = 0;

	/* 发送命令 */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-5- 配置SDRAM模式寄存器 ###############################################*/
	tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
					 SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
					 SDRAM_MODEREG_CAS_LATENCY_2           |
					 SDRAM_MODEREG_OPERATING_MODE_STANDARD |
					 SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	Command->AutoRefreshNumber = 1;
	Command->ModeRegisterDefinition = tmpmrd;

	/* 发送命令 */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-6- 设置自刷新率 ####################################################*/
    /*
        SDRAM refresh period / Number of rows）*SDRAM时钟速度 – 20
      = 64ms / 8192 *100MHz - 20
      = 1542.5 取值1543
    */
	HAL_SDRAM_ProgramRefreshRate(hsdram, 677); 
}
```