#include "unity_fixture.h"
#include "shiftCycle.h"
#include <stdint.h>
#include <string.h>

// 测试分组
TEST_GROUP(ShiftCycleTest);

TEST_SETUP(ShiftCycleTest) {}
TEST_TEAR_DOWN(ShiftCycleTest) {}

// ------------------------------ SHR 测试 ------------------------------
/**
 * SHR 覆盖场景：
 * 1. 使能关闭（en=false）、空指针（in/out=NULL）
 * 2. 无效类型（default分支）
 * 3. 无符号类型（USINT/UINT/WORD/DWORD/BYTE/UDINT）：
 *    - 移位0位、正常移位、移位等于位数、移位超过位数
 * 4. 有符号类型（SINT/INT/DINT）：
 *    - 正数：正常移位、移位等于位数、移位超过位数
 *    - 负数：算术移位（符号位扩展）、移位等于位数、移位超过位数
 */

// 测试1：SHR - 异常入参（使能关闭、空指针）
TEST(ShiftCycleTest, SHR_InvalidArgs) {
  USInt in = 0x80, out;
  // 使能关闭
  TEST_ASSERT_FALSE(SHR(false, XMZ_DATE_TYPE_USINT, &in, 1, &out));
  // in=NULL
  TEST_ASSERT_FALSE(SHR(true, XMZ_DATE_TYPE_USINT, NULL, 1, &out));
  // out=NULL
  TEST_ASSERT_FALSE(SHR(true, XMZ_DATE_TYPE_USINT, &in, 1, NULL));
}

// 测试2：SHR - 无效类型
TEST(ShiftCycleTest, SHR_InvalidType) {
  USInt in = 0x80, out;
  TEST_ASSERT_FALSE(SHR(true, XMZ_DATE_TYPE_COUNT, &in, 1, &out));
}

// 测试3：SHR - 无符号类型（USINT/Byte，8位）
TEST(ShiftCycleTest, SHR_Unsigned8) {
  // USINT测试
  USInt us_in = 0xAA, us_out;
  // 移位0位
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_USINT, &us_in, 0, &us_out));
  TEST_ASSERT_EQUAL(0xAA, us_out);
  // 正常移位（3位）
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_USINT, &us_in, 3, &us_out));
  TEST_ASSERT_EQUAL(0x15, us_out); // 10101010 >>3 = 00010101
  // 移位等于位数（8位）
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_USINT, &us_in, 8, &us_out));
  TEST_ASSERT_EQUAL(0, us_out);
  // 移位超过位数（10位）
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_USINT, &us_in, 10, &us_out));
  TEST_ASSERT_EQUAL(0, us_out);

  // Byte测试（与USINT逻辑一致）
  Byte b_in = 0xCC, b_out;
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_BYTE, &b_in, 4, &b_out));
  TEST_ASSERT_EQUAL(0x0C, b_out);
}

// 测试4：SHR - 无符号类型（UINT/Word，16位）
TEST(ShiftCycleTest, SHR_Unsigned16) {
  UInt ui_in = 0xCCAA, ui_out;
  // 移位5位
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_UINT, &ui_in, 5, &ui_out));
  TEST_ASSERT_EQUAL(0x0665, ui_out); // 1100110010101010 >>5 = 0000011001100101
  // 移位16位
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_UINT, &ui_in, 16, &ui_out));
  TEST_ASSERT_EQUAL(0, ui_out);

  Word w_in = 0xABCD, w_out;
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_WORD, &w_in, 8, &w_out));
  TEST_ASSERT_EQUAL(0xAB, w_out); // 0xABCD >>8 = 0xAB
}

// 测试5：SHR - 无符号类型（UDINT/DWORD，32位）
TEST(ShiftCycleTest, SHR_Unsigned32) {
  UDInt udi_in = 0x12345678, udi_out;
  // 移位12位
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_UDINT, &udi_in, 12, &udi_out));
  TEST_ASSERT_EQUAL(0x00012345, udi_out); // 0x12345678 >>12 = 0x00012345

  DWord dw_in = 0xFEDCBA98, dw_out;
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_DWORD, &dw_in, 24, &dw_out));
  TEST_ASSERT_EQUAL(0xFE, dw_out); // 0xFEDCBA98 >>24 = 0xFE
}

// 测试6：SHR - 有符号类型（SINT，8位）
TEST(ShiftCycleTest, SHR_Signed8) {
  SInt s_in, s_out;

  // 正数移位
  s_in = 0x65; // 101
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_SINT, &s_in, 2, &s_out));
  TEST_ASSERT_EQUAL(0x19, s_out); // 101 >>2 = 25

  // 负数移位（算术移位，符号位扩展）
  s_in = -0x10; // -16（二进制11110000）
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_SINT, &s_in, 3, &s_out));
  TEST_ASSERT_EQUAL(-0x02, s_out); // -16 >>3 = -2（二进制11111110）

  // 移位等于位数（8位）
  s_in = -1; // 0b11111111
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_SINT, &s_in, 8, &s_out));
  TEST_ASSERT_EQUAL(-1, s_out); // 按函数逻辑，n>=8返回-1

  // 移位超过位数（10位）
  s_in = 0x80; // -128
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_SINT, &s_in, 10, &s_out));
  TEST_ASSERT_EQUAL(-1, s_out);
}

// 测试7：SHR - 有符号类型（INT，16位）
TEST(ShiftCycleTest, SHR_Signed16) {
  Int i_in, i_out;

  // 正数移位
  i_in = 0x1008; // 4104
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_INT, &i_in, 4, &i_out));
  TEST_ASSERT_EQUAL(0x0100, i_out); // 4104 >>4 = 256

  // 负数移位
  i_in = -0x0100; // -256（二进制1111111011111111）
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_INT, &i_in, 3, &i_out));
  TEST_ASSERT_EQUAL(-0x0020, i_out); // -256 >>3 = -32

  // 移位等于位数（16位）
  i_in = -123;
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_INT, &i_in, 16, &i_out));
  TEST_ASSERT_EQUAL(-1, i_out);
}

// 测试8：SHR - 有符号类型（DINT，32位）
TEST(ShiftCycleTest, SHR_Signed32) {
  DInt di_in, di_out;

  // 正数移位
  di_in = 0x12345678;
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_DINT, &di_in, 16, &di_out));
  TEST_ASSERT_EQUAL(0x00001234, di_out);

  // 负数移位
  di_in = -0x00010000; // -65536
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_DINT, &di_in, 8, &di_out));
  TEST_ASSERT_EQUAL(-0x00000100, di_out); // -65536 >>8 = -256

  // 移位超过位数（40位）
  di_in = -1;
  TEST_ASSERT_TRUE(SHR(true, XMZ_DATE_TYPE_DINT, &di_in, 40, &di_out));
  TEST_ASSERT_EQUAL(-1, di_out);
}

// ------------------------------ SHL 测试 ------------------------------
/**
 * SHL 覆盖场景：
 * 1. 异常入参（使能关闭、空指针）
 * 2. 无效类型（default分支）
 * 3. 所有支持类型（8/16/32位，有符号/无符号）：
 *    - 移位0位、正常移位、移位等于位数、移位超过位数
 *    - 负数左移（逻辑左移，符号位被覆盖）
 */

// 测试9：SHL - 异常入参
TEST(ShiftCycleTest, SHL_InvalidArgs) {
  UInt in = 0x1234, out;
  TEST_ASSERT_FALSE(SHL(false, XMZ_DATE_TYPE_UINT, &in, 1, &out));
  TEST_ASSERT_FALSE(SHL(true, XMZ_DATE_TYPE_UINT, NULL, 1, &out));
  TEST_ASSERT_FALSE(SHL(true, XMZ_DATE_TYPE_UINT, &in, 1, NULL));
}

// 测试10：SHL - 无效类型
TEST(ShiftCycleTest, SHL_InvalidType) {
  UInt in = 0x1234, out;
  TEST_ASSERT_FALSE(SHL(true, XMZ_DATE_TYPE_COUNT, &in, 1, &out));
}

// 测试11：SHL - 8位类型（SINT/USINT/BYTE）
TEST(ShiftCycleTest, SHL_8bit) {
  // USINT左移
  USInt us_in = 0x15, us_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_USINT, &us_in, 3, &us_out));
  TEST_ASSERT_EQUAL(0xA8, us_out); // 00010101 <<3 = 10101000

  // SINT负数左移（逻辑左移，符号位被覆盖）
  SInt s_in = -0x08; // -8（0b11111000）
  SInt s_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_SINT, &s_in, 2, &s_out));
  TEST_ASSERT_EQUAL((SInt) 0xE0, s_out); // 11111000 <<2 = 11100000（-32）

  // BYTE移位等于位数（8位）
  Byte b_in = 0xFF, b_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_BYTE, &b_in, 8, &b_out));
  TEST_ASSERT_EQUAL(0x00, b_out); // 左移8位后全0
}

// 测试12：SHL - 16位类型（INT/UINT/WORD）
TEST(ShiftCycleTest, SHL_16bit) {
  // UINT左移
  UInt ui_in = 0x000F, ui_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_UINT, &ui_in, 4, &ui_out));
  TEST_ASSERT_EQUAL(0x00F0, ui_out);

  // INT左移（正数）
  Int i_in = 0x0123, i_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_INT, &i_in, 8, &i_out));
  TEST_ASSERT_EQUAL(0x2300, i_out);

  // WORD移位超过位数（20位）
  Word w_in = 0x1234, w_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_WORD, &w_in, 20, &w_out));
  TEST_ASSERT_EQUAL(0x0000, w_out); // 左移20位后全0
}

// 测试13：SHL - 32位类型（DINT/UDINT/DWORD）
TEST(ShiftCycleTest, SHL_32bit) {
  // UDINT左移
  UDInt udi_in = 0x00000001, udi_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_UDINT, &udi_in, 31, &udi_out));
  TEST_ASSERT_EQUAL(0x80000000, udi_out);

  // DINT负数左移
  DInt di_in = -0x80000000; // 最小32位负数
  DInt di_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_DINT, &di_in, 1, &di_out));
  TEST_ASSERT_EQUAL(0x00000000, di_out); // 10000000... <<1 = 00000000...

  // DWORD移位等于位数（32位）
  DWord dw_in = 0xFFFFFFFF, dw_out;
  TEST_ASSERT_TRUE(SHL(true, XMZ_DATE_TYPE_DWORD, &dw_in, 32, &dw_out));
  TEST_ASSERT_EQUAL(0x00000000, dw_out);
}

// ------------------------------ ROR 测试 ------------------------------
/**
 * ROR 覆盖场景：
 * 1. 异常入参（使能关闭、空指针）
 * 2. 无效类型（default分支）
 * 3. 支持类型（BYTE/WORD/DWORD）：
 *    - 移位0位、正常移位、移位等于位数（循环回原值）、移位超过位数（取模后移位）
 */

// 测试14：ROR - 异常入参
TEST(ShiftCycleTest, ROR_InvalidArgs) {
  Byte in = 0xAA, out;
  TEST_ASSERT_FALSE(ROR(false, XMZ_DATE_TYPE_BYTE, &in, 1, &out));
  TEST_ASSERT_FALSE(ROR(true, XMZ_DATE_TYPE_BYTE, NULL, 1, &out));
  TEST_ASSERT_FALSE(ROR(true, XMZ_DATE_TYPE_BYTE, &in, 1, NULL));
}

// 测试15：ROR - 无效类型
TEST(ShiftCycleTest, ROR_InvalidType) {
  Byte in = 0xAA, out;
  TEST_ASSERT_FALSE(ROR(true, XMZ_DATE_TYPE_SINT, &in, 1, &out));
}

// 测试16：ROR - BYTE（8位）
TEST(ShiftCycleTest, ROR_Byte) {
  Byte in = 0x81, out;

  // 移位1位（循环右移1位：最高位补最低位）
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_BYTE, &in, 1, &out));
  TEST_ASSERT_EQUAL(0xC0, out); // 10000001 ROR1 = 11000000

  // 移位3位
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_BYTE, &in, 3, &out));
  TEST_ASSERT_EQUAL(0x30, out); // 10000001 ROR3 = 00110000

  // 移位等于位数（8位，循环后等于原值）
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_BYTE, &in, 8, &out));
  TEST_ASSERT_EQUAL(in, out);

  // 移位超过位数（10位 → 10%8=2位）
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_BYTE, &in, 10, &out));
  TEST_ASSERT_EQUAL(0x60, out); // 等效于ROR2
}

// 测试17：ROR - WORD（16位）
TEST(ShiftCycleTest, ROR_Word) {
  Word in = 0x8001, out;

  // 移位1位
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_WORD, &in, 1, &out));
  TEST_ASSERT_EQUAL(0xC000, out);

  // 移位15位
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_WORD, &in, 15, &out));
  TEST_ASSERT_EQUAL(0x0003, out); // 1000_0000_0000_0001 ROR15 = 0000_0000_0000_0011

  // 移位超过位数（20位 → 20%16=4位）
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_WORD, &in, 20, &out));
  TEST_ASSERT_EQUAL(0x1800, out); // 等效于ROR4
}

// 测试18：ROR - DWORD（32位）
TEST(ShiftCycleTest, ROR_DWord) {
  DWord in = 0x80000001, out;

  // 移位1位
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_DWORD, &in, 1, &out));
  TEST_ASSERT_EQUAL(0xC0000000, out); // 0x80000001 ROR1 = 0xC0000000

  // 移位32位（等于位数，循环后等于原值）
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_DWORD, &in, 32, &out));
  TEST_ASSERT_EQUAL(in, out);

  // 移位40位（40%32=8位）
  TEST_ASSERT_TRUE(ROR(true, XMZ_DATE_TYPE_DWORD, &in, 40, &out));
  TEST_ASSERT_EQUAL(0x01800000, out); // 等效于ROR8
}

// ------------------------------ ROL 测试 ------------------------------
/**
 * ROL 覆盖场景：
 * 1. 异常入参（使能关闭、空指针）
 * 2. 无效类型（default分支）
 * 3. 支持类型（BYTE/WORD/DWORD）：
 *    - 移位0位、正常移位、移位等于位数（循环回原值）、移位超过位数（取模后移位）
 */

// 测试19：ROL - 异常入参
TEST(ShiftCycleTest, ROL_InvalidArgs) {
  Word in = 0x1234, out;
  TEST_ASSERT_FALSE(ROL(false, XMZ_DATE_TYPE_WORD, &in, 1, &out));
  TEST_ASSERT_FALSE(ROL(true, XMZ_DATE_TYPE_WORD, NULL, 1, &out));
  TEST_ASSERT_FALSE(ROL(true, XMZ_DATE_TYPE_WORD, &in, 1, NULL));
}

// 测试20：ROL - 无效类型
TEST(ShiftCycleTest, ROL_InvalidType) {
  Word in = 0x1234, out;
  TEST_ASSERT_FALSE(ROL(true, XMZ_DATE_TYPE_INT, &in, 1, &out));
}

// 测试21：ROL - BYTE（8位）
TEST(ShiftCycleTest, ROL_Byte) {
  Byte in = 0x81, out;

  // 移位1位（循环左移1位：最低位补最高位）
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_BYTE, &in, 1, &out));
  TEST_ASSERT_EQUAL(0x03, out); // 10000001 ROL1 = 00000011

  // 移位5位
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_BYTE, &in, 5, &out));
  TEST_ASSERT_EQUAL(0x30, out); // 1000_0001 ROL5 = 0011_0000

  // 移位8位（等于位数，循环后等于原值）
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_BYTE, &in, 8, &out));
  TEST_ASSERT_EQUAL(in, out);

  // 移位11位（11%8=3位）
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_BYTE, &in, 11, &out));
  TEST_ASSERT_EQUAL(0x0C, out); // 等效于ROL3
}

// 测试22：ROL - WORD（16位）
TEST(ShiftCycleTest, ROL_Word) {
  Word in = 0x8001, out;

  // 移位1位
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_WORD, &in, 1, &out));
  TEST_ASSERT_EQUAL(0x0003, out);

  // 移位15位
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_WORD, &in, 15, &out));
  TEST_ASSERT_EQUAL(0xC000, out);

  // 移位17位（17%16=1位）
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_WORD, &in, 17, &out));
  TEST_ASSERT_EQUAL(0x0003, out); // 等效于ROL1
}

// 测试23：ROL - DWORD（32位）
TEST(ShiftCycleTest, ROL_DWord) {
  DWord in = 0x80000001, out;

  // 移位1位
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_DWORD, &in, 1, &out));
  TEST_ASSERT_EQUAL(0x00000003, out); // 0x80000001 ROL1 = 0x00000003

  // 移位32位（等于位数，循环后等于原值）
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_DWORD, &in, 32, &out));
  TEST_ASSERT_EQUAL(in, out);

  // 移位35位（35%32=3位）
  TEST_ASSERT_TRUE(ROL(true, XMZ_DATE_TYPE_DWORD, &in, 35, &out));
  TEST_ASSERT_EQUAL(0x0000000C, out); // 等效于ROL3
}

// 注册测试组
TEST_GROUP_RUNNER(ShiftCycleTest) {
  // SHR测试
  RUN_TEST_CASE(ShiftCycleTest, SHR_InvalidArgs);
  RUN_TEST_CASE(ShiftCycleTest, SHR_InvalidType);
  RUN_TEST_CASE(ShiftCycleTest, SHR_Unsigned8);
  RUN_TEST_CASE(ShiftCycleTest, SHR_Unsigned16);
  RUN_TEST_CASE(ShiftCycleTest, SHR_Unsigned32);
  RUN_TEST_CASE(ShiftCycleTest, SHR_Signed8);
  RUN_TEST_CASE(ShiftCycleTest, SHR_Signed16);
  RUN_TEST_CASE(ShiftCycleTest, SHR_Signed32);

  // SHL测试
  RUN_TEST_CASE(ShiftCycleTest, SHL_InvalidArgs);
  RUN_TEST_CASE(ShiftCycleTest, SHL_InvalidType);
  RUN_TEST_CASE(ShiftCycleTest, SHL_8bit);
  RUN_TEST_CASE(ShiftCycleTest, SHL_16bit);
  RUN_TEST_CASE(ShiftCycleTest, SHL_32bit);

  // ROR测试
  RUN_TEST_CASE(ShiftCycleTest, ROR_InvalidArgs);
  RUN_TEST_CASE(ShiftCycleTest, ROR_InvalidType);
  RUN_TEST_CASE(ShiftCycleTest, ROR_Byte);
  RUN_TEST_CASE(ShiftCycleTest, ROR_Word);
  RUN_TEST_CASE(ShiftCycleTest, ROR_DWord);

  // ROL测试
  RUN_TEST_CASE(ShiftCycleTest, ROL_InvalidArgs);
  RUN_TEST_CASE(ShiftCycleTest, ROL_InvalidType);
  RUN_TEST_CASE(ShiftCycleTest, ROL_Byte);
  RUN_TEST_CASE(ShiftCycleTest, ROL_Word);
  RUN_TEST_CASE(ShiftCycleTest, ROL_DWord);
}

// 主函数
int shift_cycle_run(void) {
  UNITY_BEGIN();
  RUN_TEST_GROUP(ShiftCycleTest);
  return UNITY_END();
}
/*
.TEST(ShiftCycleTest, SHR_InvalidArgs): PASS
.TEST(ShiftCycleTest, SHR_InvalidType): PASS
.TEST(ShiftCycleTest, SHR_Unsigned8): PASS
.TEST(ShiftCycleTest, SHR_Unsigned16): PASS
.TEST(ShiftCycleTest, SHR_Unsigned32): PASS
.TEST(ShiftCycleTest, SHR_Signed8): PASS
.TEST(ShiftCycleTest, SHR_Signed16): PASS
.TEST(ShiftCycleTest, SHR_Signed32): PASS
.TEST(ShiftCycleTest, SHL_InvalidArgs): PASS
.TEST(ShiftCycleTest, SHL_InvalidType): PASS
.TEST(ShiftCycleTest, SHL_8bit): PASS
.TEST(ShiftCycleTest, SHL_16bit): PASS
.TEST(ShiftCycleTest, SHL_32bit): PASS
.TEST(ShiftCycleTest, ROR_InvalidArgs): PASS
.TEST(ShiftCycleTest, ROR_InvalidType): PASS
.TEST(ShiftCycleTest, ROR_Byte): PASS
.TEST(ShiftCycleTest, ROR_Word): PASS
.TEST(ShiftCycleTest, ROR_DWord): PASS
.TEST(ShiftCycleTest, ROL_InvalidArgs): PASS
.TEST(ShiftCycleTest, ROL_InvalidType): PASS
.TEST(ShiftCycleTest, ROL_Byte): PASS
.TEST(ShiftCycleTest, ROL_Word): PASS
.TEST(ShiftCycleTest, ROL_DWord): PASS

-----------------------
23 Tests 0 Failures 0 Ignored
OK
*/
