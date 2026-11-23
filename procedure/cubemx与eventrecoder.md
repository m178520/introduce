## CubeMX + Keil Event Recorder 配置说明
CubeMX 产生代码要选择 MDKV5.32，不然产生的会报错。

CubeMX 生成代码会产生 `Core/Inc/RTE_Components.h` 文件；
Keil5 也会在 `MDK-ARM/RTE` 中产生一个 `RTE_Components.h` 文件。

我们的 Event Recorder 是在 Keil 中添加的，所以需要将 Keil 产生的 `RTE_Components.h` 文件内容复制进 CubeMX 产生的文件中，并添加以下定义：

```c
#define CMSIS_device_header "stm32h7xx.h"
/* Keil::Compiler&ARM Compiler:Event Recorder&DAP@1.5.1 */
#define RTE_Compiler_EventRecorder
#define RTE_Compiler_EventRecorder_DAP
/* Keil::Compiler&ARM Compiler:I/O:STDOUT&EVR@1.2.0 */
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
#define RTE_Compiler_IO_STDOUT_EVR      /* Compiler I/O: STDOUT EVR */
