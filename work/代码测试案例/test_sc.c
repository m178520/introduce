#include "shiftCycle.h"
#include "unity_fixture.h"
#include "xmzTypeDefine.h"
#include <stdint.h>

TEST_GROUP(ShiftCycle);

TEST_SETUP(ShiftCycle) {
  // 测试前置设置，无需特殊操作
}

TEST_TEAR_DOWN(ShiftCycle) {
  // 测试后置清理，无需特殊操作
}

/**
 * SHR函数测试用例
 */
TEST(ShiftCycle, SHR_Normal_USINT) {
  // 测试ID: SHR_001
  // 测试目的: 验证无符号8位整数正常右移
  // 输入数据: en=true, 类型=USINT, in=0x80(128), n=1
  // 预期输出: out=0x40(64), 返回值=true
  USInt in = 0x80;
  USInt out;
  Bool result = SHR(true, XMZ_DATE_TYPE_USINT, &in, 1, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT8(0x40, out);
}

TEST(ShiftCycle, SHR_Normal_SINT_Negative) {
  // 测试ID: SHR_002
  // 测试目的: 验证有符号8位负数右移(算术移位)
  // 输入数据: en=true, 类型=SINT, in=-128(0x80), n=1
  // 预期输出: out=-64(0xC0), 返回值=true
  SInt in = -128;
  SInt out;
  Bool result = SHR(true, XMZ_DATE_TYPE_SINT, &in, 1, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT8(-64, out);
}

TEST(ShiftCycle, SHR_Boundary_NEqualBits) {
  // 测试ID: SHR_003
  // 测试目的: 验证移位位数等于数据位数的边界情况
  // 输入数据: en=true, 类型=BYTE, in=0xFF, n=8
  // 预期输出: out=0xFF(有符号数特性), 返回值=true
  Byte in = 0xFF;
  Byte out;
  Bool result = SHR(true, XMZ_DATE_TYPE_SINT, &in, 8, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT8(0xFF, out);
}

TEST(ShiftCycle, SHR_Boundary_NZero) {
  // 测试ID: SHR_004
  // 测试目的: 验证移位位数为0的边界情况
  // 输入数据: en=true, 类型=WORD, in=0x1234, n=0
  // 预期输出: out=0x1234, 返回值=true
  Word in = 0x1234;
  Word out;
  Bool result = SHR(true, XMZ_DATE_TYPE_WORD, &in, 0, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT16(0x1234, out);
}

TEST(ShiftCycle, SHR_Exception_EnFalse) {
  // 测试ID: SHR_005
  // 测试目的: 验证使能信号为false的异常情况
  // 输入数据: en=false, 类型=DWORD, in=0x12345678, n=4
  // 预期输出: 返回值=false
  DWord in = 0x12345678;
  DWord out;
  Bool result = SHR(false, XMZ_DATE_TYPE_DWORD, &in, 4, &out);
  TEST_ASSERT_FALSE(result);
}

TEST(ShiftCycle, SHR_Exception_InNull) {
  // 测试ID: SHR_006
  // 测试目的: 验证输入指针为NULL的异常情况
  // 输入数据: en=true, 类型=UDINT, in=NULL, n=2
  // 预期输出: 返回值=false
  UDInt out;
  Bool result = SHR(true, XMZ_DATE_TYPE_UDINT, NULL, 2, &out);
  TEST_ASSERT_FALSE(result);
}

TEST(ShiftCycle, SHR_Exception_OutNull) {
  // 测试ID: SHR_007
  // 测试目的: 验证输出指针为NULL的异常情况
  // 输入数据: en=true, 类型=DINT, in=0x12345678, n=2, out=NULL
  // 预期输出: 返回值=false
  DInt in = 0x12345678;
  Bool result = SHR(true, XMZ_DATE_TYPE_DINT, &in, 2, NULL);
  TEST_ASSERT_FALSE(result);
}

TEST(ShiftCycle, SHR_Exception_UnsupportedType) {
  // 测试ID: SHR_008
  // 测试目的: 验证不支持的数据类型
  // 输入数据: en=true, 类型=REAL, in=任意, n=1
  // 预期输出: 返回值=false
  Real in = 123.45f;
  Real out;
  Bool result = SHR(true, XMZ_DATE_TYPE_REAL, &in, 1, &out);
  TEST_ASSERT_FALSE(result);
}

/**
 * SHL函数测试用例
 */
TEST(ShiftCycle, SHL_Normal_UINT) {
  // 测试ID: SHL_001
  // 测试目的: 验证无符号16位整数正常左移
  // 输入数据: en=true, 类型=UINT, in=0x0001, n=4
  // 预期输出: out=0x0010, 返回值=true
  UInt in = 0x0001;
  UInt out;
  Bool result = SHL(true, XMZ_DATE_TYPE_UINT, &in, 4, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT16(0x0010, out);
}

TEST(ShiftCycle, SHL_Normal_SINT) {
  // 测试ID: SHL_002
  // 测试目的: 验证有符号8位整数左移(逻辑移位)
  // 输入数据: en=true, 类型=SINT, in=0x0F, n=2
  // 预期输出: out=0x3C, 返回值=true
  SInt in = 0x0F;
  SInt out;
  Bool result = SHL(true, XMZ_DATE_TYPE_SINT, &in, 2, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT8(0x3C, out);
}

TEST(ShiftCycle, SHL_Boundary_NExceedBits) {
  // 测试ID: SHL_003
  // 测试目的: 验证移位位数超过数据位数的边界情况
  // 输入数据: en=true, 类型=BYTE, in=0xFF, n=9
  // 预期输出: out=0x00, 返回值=true
  Byte in = 0xFF;
  Byte out;
  Bool result = SHL(true, XMZ_DATE_TYPE_BYTE, &in, 9, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT8(0x00, out);
}

TEST(ShiftCycle, SHL_Boundary_SignedOverflow) {
  // 测试ID: SHL_004
  // 测试目的: 验证有符号数左移溢出情况
  // 输入数据: en=true, 类型=INT, in=0x4000, n=1
  // 预期输出: out=0x8000(负数), 返回值=true
  Int in = 0x4000;
  Int out;
  Bool result = SHL(true, XMZ_DATE_TYPE_INT, &in, 1, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT16(0x8000, out);
}

TEST(ShiftCycle, SHL_Exception_InvalidType) {
  // 测试ID: SHL_005
  // 测试目的: 验证不支持的数据类型
  // 输入数据: en=true, 类型=TIME, in=任意, n=1
  // 预期输出: 返回值=false
  Time in = 1000;
  Time out;
  Bool result = SHL(true, XMZ_DATE_TYPE_TIME, &in, 1, &out);
  TEST_ASSERT_FALSE(result);
}

/**
 * ROR函数测试用例
 */
TEST(ShiftCycle, ROR_Normal_BYTE) {
  // 测试ID: ROR_001
  // 测试目的: 验证字节类型正常循环右移
  // 输入数据: en=true, 类型=BYTE, in=0b10000001, n=1
  // 预期输出: out=0b11000000, 返回值=true
  // Byte in = 0b10000001;
  Byte in = 0x81;
  Byte out;
  Bool result = ROR(true, XMZ_DATE_TYPE_BYTE, &in, 1, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT8(0xC0, out);
}

TEST(ShiftCycle, ROR_Normal_DWORD) {
  // 测试ID: ROR_002
  // 测试目的: 验证双字类型正常循环右移
  // 输入数据: en=true, 类型=DWORD, in=0x80000001, n=2
  // 预期输出: out=0xC0800000, 返回值=true
  DWord in = 0x80000001;
  DWord out;
  Bool result = ROR(true, XMZ_DATE_TYPE_DWORD, &in, 2, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT32(0x60000000, out);
}

TEST(ShiftCycle, ROR_Boundary_NEqualBits) {
  // 测试ID: ROR_003
  // 测试目的: 验证移位位数等于数据位数的边界情况
  // 输入数据: en=true, 类型=WORD, in=0x1234, n=16
  // 预期输出: out=0x1234(循环移位16位等于不移位), 返回值=true
  Word in = 0x1234;
  Word out;
  Bool result = ROR(true, XMZ_DATE_TYPE_WORD, &in, 16, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT16(0x1234, out);
}

TEST(ShiftCycle, ROR_Boundary_NExceedBits) {
  // 测试ID: ROR_004
  // 测试目的: 验证移位位数超过数据位数的边界情况
  // 输入数据: en=true, 类型=BYTE, in=0b1001, n=10(等价于n=2)
  // 预期输出: out=0b01000010, 返回值=true
  // Byte in = 0b00001001;
  Byte in = 0x9;
  Byte out;
  Bool result = ROR(true, XMZ_DATE_TYPE_BYTE, &in, 10, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT8(0x42, out);
}

TEST(ShiftCycle, ROR_Exception_EnFalse) {
  // 测试ID: ROR_005
  // 测试目的: 验证使能信号为false的异常情况
  // 输入数据: en=false, 类型=WORD, in=0x1234, n=2
  // 预期输出: 返回值=false
  Word in = 0x1234;
  Word out;
  Bool result = ROR(false, XMZ_DATE_TYPE_WORD, &in, 2, &out);
  TEST_ASSERT_FALSE(result);
}

/**
 * ROL函数测试用例
 */
TEST(ShiftCycle, ROL_Normal_BYTE) {
  // 测试ID: ROL_001
  // 测试目的: 验证字节类型正常循环左移
  // 输入数据: en=true, 类型=BYTE, in=0b10000001, n=1
  // 预期输出: out=0b00000011, 返回值=true
  // Byte in = 0b10000001;
  Byte in = 0x81;
  Byte out;
  Bool result = ROL(true, XMZ_DATE_TYPE_BYTE, &in, 1, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT8(0x3, out);
}

TEST(ShiftCycle, ROL_Normal_WORD) {
  // 测试ID: ROL_002
  // 测试目的: 验证字类型正常循环左移
  // 输入数据: en=true, 类型=WORD, in=0x8001, n=2
  // 预期输出: out=0x0006, 返回值=true
  Word in = 0x8001;
  Word out;
  Bool result = ROL(true, XMZ_DATE_TYPE_WORD, &in, 2, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT16(0x0006, out);
}

TEST(ShiftCycle, ROL_Boundary_NEqualBits) {
  // 测试ID: ROL_003
  // 测试目的: 验证移位位数等于数据位数的边界情况
  // 输入数据: en=true, 类型=DWORD, in=0x12345678, n=32
  // 预期输出: out=0x12345678(循环移位32位等于不移位), 返回值=true
  DWord in = 0x12345678;
  DWord out;
  Bool result = ROL(true, XMZ_DATE_TYPE_DWORD, &in, 32, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT32(0x12345678, out);
}

TEST(ShiftCycle, ROL_Boundary_NExceedBits) {
  // 测试ID: ROL_004
  // 测试目的: 验证移位位数超过数据位数的边界情况
  // 输入数据: en=true, 类型=BYTE, in=0b1001, n=10(等价于n=2)
  // 预期输出: out=0b0100100, 返回值=true
  // Byte in = 0b1001;
  Byte in = 0x9;
  Byte out;
  Bool result = ROL(true, XMZ_DATE_TYPE_BYTE, &in, 10, &out);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_UINT8(0x24, out);
}

TEST(ShiftCycle, ROL_Exception_UnsupportedType) {
  // 测试ID: ROL_005
  // 测试目的: 验证不支持的数据类型
  // 输入数据: en=true, 类型=INT, in=任意, n=1
  // 预期输出: 返回值=false
  Int in = 123;
  Int out;
  Bool result = ROL(true, XMZ_DATE_TYPE_INT, &in, 1, &out);
  TEST_ASSERT_FALSE(result);
}

TEST_GROUP_RUNNER(ShiftCycle) {
  RUN_TEST_CASE(ShiftCycle, SHR_Normal_USINT);
  RUN_TEST_CASE(ShiftCycle, SHR_Normal_SINT_Negative);
  RUN_TEST_CASE(ShiftCycle, SHR_Boundary_NEqualBits);
  RUN_TEST_CASE(ShiftCycle, SHR_Boundary_NZero);
  RUN_TEST_CASE(ShiftCycle, SHR_Exception_EnFalse);
  RUN_TEST_CASE(ShiftCycle, SHR_Exception_InNull);
  RUN_TEST_CASE(ShiftCycle, SHR_Exception_OutNull);
  RUN_TEST_CASE(ShiftCycle, SHR_Exception_UnsupportedType);

  RUN_TEST_CASE(ShiftCycle, SHL_Normal_UINT);
  RUN_TEST_CASE(ShiftCycle, SHL_Normal_SINT);
  RUN_TEST_CASE(ShiftCycle, SHL_Boundary_NExceedBits);
  RUN_TEST_CASE(ShiftCycle, SHL_Boundary_SignedOverflow);
  RUN_TEST_CASE(ShiftCycle, SHL_Exception_InvalidType);

  RUN_TEST_CASE(ShiftCycle, ROR_Normal_BYTE);
  RUN_TEST_CASE(ShiftCycle, ROR_Normal_DWORD);
  RUN_TEST_CASE(ShiftCycle, ROR_Boundary_NEqualBits);
  RUN_TEST_CASE(ShiftCycle, ROR_Boundary_NExceedBits);
  RUN_TEST_CASE(ShiftCycle, ROR_Exception_EnFalse);

  RUN_TEST_CASE(ShiftCycle, ROL_Normal_BYTE);
  RUN_TEST_CASE(ShiftCycle, ROL_Normal_WORD);
  RUN_TEST_CASE(ShiftCycle, ROL_Boundary_NEqualBits);
  RUN_TEST_CASE(ShiftCycle, ROL_Boundary_NExceedBits);
  RUN_TEST_CASE(ShiftCycle, ROL_Exception_UnsupportedType);
}

int shift_cycle_run_ai() {
  UNITY_BEGIN();
  RUN_TEST_GROUP(ShiftCycle);
  return UNITY_END();
}

/*
.TEST(ShiftCycle, SHR_Normal_USINT): PASS
.TEST(ShiftCycle, SHR_Normal_SINT_Negative): PASS
.TEST(ShiftCycle, SHR_Boundary_NEqualBits): PASS
.TEST(ShiftCycle, SHR_Boundary_NZero): PASS
.TEST(ShiftCycle, SHR_Exception_EnFalse): PASS
.TEST(ShiftCycle, SHR_Exception_InNull): PASS
.TEST(ShiftCycle, SHR_Exception_OutNull): PASS
.TEST(ShiftCycle, SHR_Exception_UnsupportedType): PASS
.TEST(ShiftCycle, SHL_Normal_UINT): PASS
.TEST(ShiftCycle, SHL_Normal_SINT): PASS
.TEST(ShiftCycle, SHL_Boundary_NExceedBits): PASS
.TEST(ShiftCycle, SHL_Boundary_SignedOverflow): PASS
.TEST(ShiftCycle, SHL_Exception_InvalidType): PASS
.TEST(ShiftCycle, ROR_Normal_BYTE): PASS
.todoTest\test_sc.c:196:TEST(ShiftCycle, ROR_Normal_DWORD):FAIL: Expected
3229614080 Was 1610612736 .TEST(ShiftCycle, ROR_Boundary_NEqualBits): PASS
.todoTest\test_sc.c:221:TEST(ShiftCycle, ROR_Boundary_NExceedBits):FAIL:
Expected 52 Was 66 .TEST(ShiftCycle, ROR_Exception_EnFalse): PASS
.TEST(ShiftCycle, ROL_Normal_BYTE): PASS
.todoTest\test_sc.c:260:TEST(ShiftCycle, ROL_Normal_WORD):FAIL: Expected 67 Was
6 .TEST(ShiftCycle, ROL_Boundary_NEqualBits): PASS .TEST(ShiftCycle,
ROL_Boundary_NExceedBits): PASS .TEST(ShiftCycle,
ROL_Exception_UnsupportedType): PASS

-----------------------
23 Tests 3 Failures 0 Ignore
*/