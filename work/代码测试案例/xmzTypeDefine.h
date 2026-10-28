/**
 * @file xmzTypeDefine.h
 * @author your name (you@domain.com)
 * @brief 西门子数据类型定义
 * @version 0.1
 * @date 2025-04-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __XMZ_TYPE_DEFINE_H__
#define __XMZ_TYPE_DEFINE_H__
#include <stdint.h>
/*二进制数*/
#define true 1
#define false 0
#define True 1
#define False 0
/*BOOL（位）*/
typedef uint8_t Bool;
/*位字符串*/
typedef uint8_t  Byte; /*数据类型 BYTE 的操作数是位字符串，有 8 位。*/
typedef uint8_t  BYTE;
typedef uint16_t Word; /*数据类型 WORD 的操作数是字，有 16 位。*/
typedef uint8_t  WORD;
typedef uint32_t DWord; /*数据类型 DWORD 的操作数是双字，有 32 位。*/
typedef uint64_t LWord; /*数据类型 LWORD 的操作数是四字，有 64 位。*/

/*整数*/
typedef int8_t   SInt;  /*数据类型 (Short INT) 的操作数是字节，有 8 位。*/
typedef uint8_t  USInt; /*数据类型 USINT (Unsigned Short INT) 的操作数长度为 8 位，包括无符号数值*/
typedef int16_t  Int;   /*数据类型 INT 的操作数是字，有 16 位。*/
typedef uint16_t UInt;  /*数据类型 UINT (Unsigned INT) 的操作数是字，有 16 位。*/
typedef int32_t  DInt;  /*数据类型 DINT (Double INT) 的操作数是双字，有 32 位。*/
typedef uint32_t UDInt; /*数据类型 UDINT (Unsigned Double INT) 的操作数是双字，有 32 位。*/
typedef int64_t  LInt;  /*数据类型 LINT (Long INT) 的操作数是四字，有 64 位。*/
typedef uint64_t ULInt; /*数据类型 ULINT (Unsigned Long INT) 的操作数是四字，有 64 位。*/

/*浮点数 */
typedef float  Real;  /*数据类型 REAL (Real) 的操作数是单精度浮点数，有 32 位。*/
typedef double LReal; /*数据类型 LREAL (Long Real) 的操作数是双精度浮点数，有 64 位。*/

/*定时器*/
typedef int32_t Time;  /*数据类型 TIME 的操作数是双字，有 32 位。有符号的持续时间数据类型为 TIME
                          的操作数内容以毫秒表示。表示信息包括天 (d)、小时 (h)、分钟 (m)、秒 (s) 和毫秒 (ms)。*/
typedef int64_t LTime; /*数据类型 LTIME 的操作数是四字，有 64 位。有符号的持续时间*/

/*日期和时间*/
typedef uint16_t Date;      /*DATE 数据类型将日期作为无符号整数保存。表示法中包括年、月和日。数据类型 DATE
                             * 的操作数为十六进制形式，对应于自 01-01-1990 以来的日期值 (16#0000)。通过天数来计算日期
                             * D#1990-01-01 到 D#2169-06-06
                             */
typedef uint32_t TimeOfDay; /*TIME_OF_DAY 数据类型将时间作为无符号整数保存。表示法中包括小时、分钟、秒和毫秒。数据类型
TIME_OF_DAY 的操作数为十六进制形式，对应于自 00:00:00 以来的时间值 (16#00000000)。通过秒数来计算时间 T#00:00:00 到
T#23:59:59 您必须指定小时、分钟和秒。 毫秒可选。*/

/*字符串*/
/*字符*/
typedef char     Char;  /*CHAR (Character) 数据类型的变量长度为 8 位，占用一个 BYTE 的内存。*/
typedef uint16_t WChar; /*数据类型 WCHAR (Wide Character) 的变量长度为 16 位，占用一个 WORD 的内存。*/
typedef struct {
  uint8_t MaxLen;    // 固定为 254
  uint8_t ActualLen; // 实际使用长度
  char    Data[254]; // 字符内容
} S7String;
/*系统FB*/

/*系统FC*/

/**时钟计数满足西门子内部特殊变量 */
typedef struct {
  uint16_t tick_10Hz;
  uint16_t tick_5Hz;
  uint16_t tick_2_5Hz;
  uint16_t tick_2Hz;
  uint16_t tick_1_25Hz;
  uint16_t tick_1Hz;
  uint16_t tick_0_625Hz;
  uint16_t tick_0_5Hz;
} Clock_Tick_S;
/*翻转周期*/
#define CLOCK_10HZ 50     // 100ms 全周期 → 50ms 翻转
#define CLOCK_5HZ 100     // 200ms 全周期 → 100ms 翻转
#define CLOCK_2_5HZ 200   // 400ms 全周期 → 200ms 翻转
#define CLOCK_2HZ 250     // 500ms 全周期 → 250ms 翻转
#define CLOCK_1_25HZ 400  // 800ms 全周期 → 400ms 翻转
#define CLOCK_1HZ 500     // 1000ms 全周期 → 500ms 翻转
#define CLOCK_0_625HZ 800 // 1600ms 全周期 → 800ms 翻转
#define CLOCK_0_5HZ 1000  // 2000ms 全周期 → 1000ms 翻转

typedef enum {
  XMZ_DATE_TYPE_BOOL,
  XMZ_DATE_TYPE_BYTE,
  XMZ_DATE_TYPE_WORD,
  XMZ_DATE_TYPE_DWORD,
  XMZ_DATE_TYPE_LWORD,
  XMZ_DATE_TYPE_SINT,
  XMZ_DATE_TYPE_USINT,
  XMZ_DATE_TYPE_INT,
  XMZ_DATE_TYPE_UINT,
  XMZ_DATE_TYPE_DINT,
  XMZ_DATE_TYPE_UDINT,
  XMZ_DATE_TYPE_LINT,
  XMZ_DATE_TYPE_ULINT,
  XMZ_DATE_TYPE_REAL,
  XMZ_DATE_TYPE_LREAL,
  XMZ_DATE_TYPE_TIME,
  XMZ_DATE_TYPE_LTIME,
  XMZ_DATE_TYPE_DATE,
  XMZ_DATE_TYPE_TOD,
  XMZ_DATE_TYPE_LTOD, //!< 未实现
  XMZ_DATE_TYPE_DT,   //!< 未实现DATE_AND_TIME
  XMZ_DATE_TYPE_LDT,  //!< 未实现LDT (DATE_AND_LTIME)
  XMZ_DATE_TYPE_CHAR,
  XMZ_DATE_TYPE_WCHAR,
  XMZ_DATE_TYPE_DTL,
  XMZ_DATE_TYPE_COUNT,
} XMZ_DATE_TYPE;

/**
 * @brief 定时器操作类型枚举
 *
 */
typedef enum {
  TIMER_OP_TON,
  TIMER_OP_TONR,
  TIMER_OP_TP,
  TIMER_OP_TOF,
  TIMER_OP_NULL,
} TIMER_OP_TYPE;
/*************************** 指令版本 *******************/
#define VERSION_TIMER_OPERATION 2
/*************************** 指令版本 *******************/
#endif
