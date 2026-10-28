#ifndef SHIFTCYCLE_H
#define SHIFTCYCLE_H
#include "xmzTypeDefine.h"

/**
 * @brief 可以使用“右移”指令将输入 IN 中操作数的内容按位向右移位，并在输出 OUT 中查询结果。参数 N
用于指定将指定值移位的位数。
如果参数 N 的值为“0”，则将输入 IN 的值复制到输出 OUT 的操作数中。
如果参数 N的值大于位数，则输入 IN 的操作数值将向右移动该位数个位置。
无符号值移位时，用零填充操作数左侧区域中空出的位。
如果指定值有符号，则用符号位的信号状态填充空出的位。
 *
 * @param en
 * @param shr_type
 * @param in
 * @param n
 * @param out
 * @return Bool
 */
Bool SHR(Bool en, XMZ_DATE_TYPE sh_type, const void* in, int n, void* out);

/**
 * @brief 可以使用“左移”指令将输入 IN 中操作数的内容按位向左移位，并在输出 OUT 中查询结果。参数 N
用于指定将指定值移位的位数。
如果参数 N 的值为“0”，则将输入 IN 的值复制到输出 OUT 的操作数中。
如果参数 N的值大于位数，则输入 IN 的操作数值将向右移动该位数个位置。
用零填充操作数右侧部分因移位空出的位。
 *
 * @param en
 * @param shl_type
 * @param in
 * @param n
 * @param out
 * @return Bool
 */
Bool SHL(Bool en, XMZ_DATE_TYPE sh_type, const void* in, int n, void* out);

/**
 * @brief 可以使用“循环右移”指令将输入 IN 中操作数的内容按位向右循环移位，并在输出 OUT 中查询结果。参数 N
用于指定循环移位中待移动的位数。用移出的位填充因循环移位而空出的位。
如果参数 N 的值为“0”，则将输入 IN 的值复制到输出OUT 的操作数中。
如果参数 N 的值大于可用位数，则输入 IN 中的操作数值仍会循环移动指定位数。
 *
 * @param en
 * @param cycly_type
 * @param in
 * @param n
 * @param out
 * @return Bool
 */
Bool ROR(Bool en, XMZ_DATE_TYPE cycly_type, const void* in, int n, void* out);

/**
 * @brief 可以使用“循环左移”指令将输入 IN 中操作数的内容按位向左循环移位，并在输出 OUT 中查询结果。参数 N
用于指定循环移位中待移动的位数。用移出的位填充因循环移位而空出的位。
如果参数 N 的值为“0”，则将输入 IN 的值复制到输出OUT 的操作数中。
如果参数 N 的值大于可用位数，则输入 IN 中的操作数值仍会循环移动指定位数

 *
 * @param en
 * @param cycly_type
 * @param in
 * @param n
 * @param out
 * @return Bool
 */
Bool ROL(Bool en, XMZ_DATE_TYPE cycly_type, const void* in, int n, void* out);
#endif
