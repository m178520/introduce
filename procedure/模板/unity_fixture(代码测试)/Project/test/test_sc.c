#include "unity_fixture.h"

#include "test.h"
#include "xmzTypeDefine.h"
#include <stdint.h>
#include <string.h>


// 被测函数外部声明
extern Bool FieldWrite(Bool EN, DInt INDEX, const void* VALUE, void* MEMBER, XMZ_DATE_TYPE oper_type, size_t mem_len);
extern Bool SET_BF_BITS(Bool en, UInt count, uint8_t* base, uint16_t bit_start);
extern Bool RESET_BF_BITS(Bool en, UInt count, uint8_t* base, uint16_t bit_start);
extern Bool Swap(Bool enable, const void* inValue, uint8_t sizeBytes, void* outValue);

TEST_GROUP(FieldUtilTest);
TEST_SETUP(FieldUtilTest)
{
}
TEST_TEAR_DOWN(FieldUtilTest)
{
}

/**************************** FieldWrite 写入域指令测试 ****************************/
/**
 * @brief 测试目的：FieldWrite 使能输入EN为false时不执行写入
 * 输入数据：EN=false，INDEX=1，VALUE=&val(0x1234)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 * 预期输出：返回值 = false，数组arr内容保持不变
 */
TEST(FieldUtilTest, FieldWrite_EN_False)
{
    Word arr[4] = {0};
    Word val = 0x1234;
    Bool ret = FieldWrite(false, 1, &val, arr, XMZ_DATE_TYPE_WORD, 4);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：FieldWrite 输入空指针异常分支（VALUE为NULL、MEMBER为NULL）
 * 输入数据：①EN=true，INDEX=1，VALUE=NULL，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 *          ②EN=true，INDEX=1，VALUE=&val(0x5678)，MEMBER=NULL，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 * 预期输出：两次调用均返回 false
 */
TEST(FieldUtilTest, FieldWrite_NULL_Ptr)
{
    Word arr[4] = {0};
    Word val = 0x5678;
    Bool ret1 = FieldWrite(true,1,NULL,arr,XMZ_DATE_TYPE_WORD,4);
    Bool ret2 = FieldWrite(true,1,&val,NULL,XMZ_DATE_TYPE_WORD,4);
    TEST_ASSERT_FALSE(ret1);
    TEST_ASSERT_FALSE(ret2);
}

/**
 * @brief 测试目的：FieldWrite 索引INDEX为负数，触发越界判断
 * 输入数据：EN=true，INDEX=-1，VALUE=&val(0xA5A5)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 * 预期输出：返回 false，不执行数组写入
 */
TEST(FieldUtilTest, FieldWrite_Index_Negative)
{
    Word arr[4] = {0};
    Word val = 0xA5A5;
    Bool ret = FieldWrite(true, -1, &val, arr, XMZ_DATE_TYPE_WORD,4);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：FieldWrite 索引INDEX等于数组长度，上边界越界
 * 输入数据：EN=true，INDEX=4，VALUE=&val(0xA5A5)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 * 预期输出：返回 false，不执行数组写入；数组下标合法范围：0~3
 */
TEST(FieldUtilTest, FieldWrite_Index_OverUpper)
{
    Word arr[4] = {0};
    Word val = 0xA5A5;
    Bool ret = FieldWrite(true,4,&val,arr,XMZ_DATE_TYPE_WORD,4);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：FieldWrite WORD类型正常写入，合法索引位置
 * 输入数据：EN=true，INDEX=2，VALUE=&val(0xA5A5)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 * 预期输出：返回 true；arr[2] = 0xA5A5
 */
TEST(FieldUtilTest, FieldWrite_Write_Word_Normal)
{
    Word arr[4] = {0,0,0,0};
    Word val = 0xA5A5;
    Bool ret = FieldWrite(true,2,&val,arr,XMZ_DATE_TYPE_WORD,4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT16(0xA5A5, arr[2]);
}

/**
 * @brief 测试目的：FieldWrite REAL浮点数类型写入，验证浮点类型分支
 * 输入数据：EN=true，INDEX=1，VALUE=&val(3.14f)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_REAL，mem_len=3
 * 预期输出：返回 true；arr[1] ≈3.14f（浮点允许微小误差）
 */
TEST(FieldUtilTest, FieldWrite_Write_Real)
{
    Real arr[3] = {0.0f,0.0f,0.0f};
    Real val = 3.14f;
    Bool ret = FieldWrite(true,1,&val,arr,XMZ_DATE_TYPE_REAL,3);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, arr[1]);
}

/**
 * @brief 测试目的：FieldWrite 传入未实现的数据类型 XMZ_DATE_TYPE_DT，验证未实现分支return false
 * 输入数据：EN=true，INDEX=0，VALUE=&val(100)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_DT，mem_len=2
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, FieldWrite_NotImplType_DT)
{
    Date arr[2];
    Date val = 100;
    Bool ret = FieldWrite(true,0,&val,arr,XMZ_DATE_TYPE_DT,2);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：FieldWrite 传入非法无效枚举类型，命中switch default分支
 * 输入数据：EN=true，INDEX=0，VALUE=&val(0x11)，MEMBER=arr，oper_type=(XMZ_DATE_TYPE)99，mem_len=2
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, FieldWrite_Default_BadType)
{
    Byte arr[2];
    Byte val = 0x11;
    Bool ret = FieldWrite(true,0,&val,arr,(XMZ_DATE_TYPE)99,2);
    TEST_ASSERT_FALSE(ret);
}

/**************************** SET_BF_BITS 打包位域置位 ****************************/
/**
 * @brief 测试目的：SET_BF_BITS 异常入参：使能关闭en=false、base空指针
 * 输入数据：①en=false，count=3，base=buf，bit_start=0；②en=true，count=3，base=NULL，bit_start=0
 * 预期输出：两次调用返回 false
 */
TEST(FieldUtilTest, SET_BF_BITS_InvalidArg)
{
    uint8_t buf[2] = {0};
    Bool ret1 = SET_BF_BITS(false,3,buf,0);
    Bool ret2 = SET_BF_BITS(true,3,NULL,0);
    TEST_ASSERT_FALSE(ret1);
    TEST_ASSERT_FALSE(ret2);
}

/**
 * @brief 测试目的：SET_BF_BITS 正常跨字节置位；bit_start=6，连续置位3个位
 * 输入数据：en=true，count=3，base=buf，bit_start=6；buf初始{0,0}
 * 预期输出：返回 true；buf[0]=0xC0（bit6、bit7置1），buf[1]=0x01（bit0置1）
 */
TEST(FieldUtilTest, SET_BF_BITS_Normal_CrossByte)
{
    uint8_t buf[2] = {0,0};
    Bool ret = SET_BF_BITS(true,3,buf,6);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0xC0, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0x01, buf[1]);
}

/**************************** RESET_BF_BITS 打包位域复位 ****************************/
/**
 * @brief 测试目的：RESET_BF_BITS 异常入参：使能关闭en=false、base空指针
 * 输入数据：①en=false，count=3，base=buf，bit_start=0；②en=true，count=3，base=NULL，bit_start=0
 * 预期输出：两次调用返回 false
 */
TEST(FieldUtilTest, RESET_BF_BITS_InvalidArg)
{
    uint8_t buf[2] = {0xFF,0xFF};
    Bool ret1 = RESET_BF_BITS(false,3,buf,0);
    Bool ret2 = RESET_BF_BITS(true,3,NULL,0);
    TEST_ASSERT_FALSE(ret1);
    TEST_ASSERT_FALSE(ret2);
}

/**
 * @brief 测试目的：RESET_BF_BITS 跨字节复位；bit_start=6，连续复位3个位
 * 输入数据：en=true，count=3，base=buf，bit_start=6；buf初始 {0xFF,0xFF}
 * 预期输出：返回 true；buf[0]=0x3F，buf[1]=0xFE
 */
TEST(FieldUtilTest, RESET_BF_BITS_Normal_CrossByte)
{
    uint8_t buf[2] = {0xFF,0xFF};
    Bool ret = RESET_BF_BITS(true,3,buf,6);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x3F, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0xFE, buf[1]);
}

/**************************** Swap 西门子SWAP字节反转指令 ****************************/
/**
 * @brief 测试目的：Swap 使能关闭enable=false，不执行字节反转
 * 输入数据：enable=false，inValue=&in(0x1234)，sizeBytes=2，outValue=&out
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, Swap_EN_False)
{
    Word in = 0x1234;
    Word out = 0;
    Bool ret = Swap(false, &in, 2, &out);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：Swap 空指针异常：inValue为NULL，outValue为NULL
 * 输入数据：①enable=true，inValue=NULL，sizeBytes=2，outValue=&out；
 *          ②enable=true，inValue=&in(0x1234)，sizeBytes=2，outValue=NULL
 * 预期输出：两次调用返回 false
 */
TEST(FieldUtilTest, Swap_NULL_Ptr)
{
    Word in =0x1234;
    Word out=0;
    Bool ret1 = Swap(true, NULL,2,&out);
    Bool ret2 = Swap(true,&in,2,NULL);
    TEST_ASSERT_FALSE(ret1);
    TEST_ASSERT_FALSE(ret2);
}

/**
 * @brief 测试目的：Swap WORD(2字节)字节反转；0x1234 →0x3412
 * 输入数据：enable=true，inValue=&in(0x1234)，sizeBytes=2，outValue=&out
 * 预期输出：返回 true，out =0x3412
 */
TEST(FieldUtilTest, Swap_WORD)
{
    Word in = 0x1234;
    Word out;
    Bool ret = Swap(true, &in, 2, &out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT16(0x3412, out);
}

/**
 * @brief 测试目的：Swap DWORD(4字节)字节反转；0x12345678 →0x78563412
 * 输入数据：enable=true，inValue=&in(0x12345678U)，sizeBytes=4，outValue=&out
 * 预期输出：返回 true，out=0x78563412U
 */
TEST(FieldUtilTest, Swap_DWORD)
{
    DWord in = 0x12345678U;
    DWord out;
    Bool ret = Swap(true, &in,4,&out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT32(0x78563412U, out);
}

/**
 * @brief 测试目的：Swap LWORD(8字节)字节反转；0x0123456789ABCDEF →0xEFCDAB8967452301
 * 输入数据：enable=true，inValue=&in(0x0123456789ABCDEFULL)，sizeBytes=8，outValue=&out
 * 预期输出：返回 true，out =0xEFCDAB8967452301ULL
 */
TEST(FieldUtilTest, Swap_LWORD)
{
    LWord in = 0x0123456789ABCDEFULL;
    LWord out;
    Bool ret = Swap(true, &in,8,&out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT64(0xEFCDAB8967452301ULL, out);
}

/**
 * @brief 测试目的：Swap sizeBytes非法(1字节)，命中switch default分支
 * 输入数据：enable=true，inValue=&in(0x11)，sizeBytes=1，outValue=&out
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, Swap_BadSize)
{
    Byte in =0x11;
    Byte out;
    Bool ret = Swap(true,&in,1,&out);
    TEST_ASSERT_FALSE(ret);
}


/**************************** FieldWrite 补充测试（多类型/边界） ****************************/

/**
 * @brief 测试目的：FieldWrite BOOL类型正常写入
 * 输入数据：EN=true，INDEX=0，VALUE=&val(true)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_BOOL，mem_len=4
 * 预期输出：返回 true；arr[0] = true
 */
TEST(FieldUtilTest, FieldWrite_Write_Bool)
{
    Bool arr[4] = {false, false, false, false};
    Bool val = true;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_BOOL, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_TRUE(arr[0]);
}

/**
 * @brief 测试目的：FieldWrite BYTE类型正常写入
 * 输入数据：EN=true，INDEX=1，VALUE=&val(0xAB)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_BYTE，mem_len=4
 * 预期输出：返回 true；arr[1] = 0xAB
 */
TEST(FieldUtilTest, FieldWrite_Write_Byte)
{
    Byte arr[4] = {0};
    Byte val = 0xAB;
    Bool ret = FieldWrite(true, 1, &val, arr, XMZ_DATE_TYPE_BYTE, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0xAB, arr[1]);
}

/**
 * @brief 测试目的：FieldWrite DWORD类型正常写入
 * 输入数据：EN=true，INDEX=2，VALUE=&val(0xDEADBEEFU)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_DWORD，mem_len=4
 * 预期输出：返回 true；arr[2] = 0xDEADBEEFU
 */
TEST(FieldUtilTest, FieldWrite_Write_DWord)
{
    DWord arr[4] = {0};
    DWord val = 0xDEADBEEFU;
    Bool ret = FieldWrite(true, 2, &val, arr, XMZ_DATE_TYPE_DWORD, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT32(0xDEADBEEFU, arr[2]);
}

/**
 * @brief 测试目的：FieldWrite LWORD类型正常写入
 * 输入数据：EN=true，INDEX=0，VALUE=&val(0x0123456789ABCDEFULL)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_LWORD，mem_len=2
 * 预期输出：返回 true；arr[0] = 0x0123456789ABCDEFULL
 */
TEST(FieldUtilTest, FieldWrite_Write_LWord)
{
    LWord arr[2] = {0};
    LWord val = 0x0123456789ABCDEFULL;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_LWORD, 2);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT64(0x0123456789ABCDEFULL, arr[0]);
}

/**
 * @brief 测试目的：FieldWrite INT类型正常写入（有符号负数）
 * 输入数据：EN=true，INDEX=1，VALUE=&val(-100)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_INT，mem_len=4
 * 预期输出：返回 true；arr[1] = -100
 */
TEST(FieldUtilTest, FieldWrite_Write_Int)
{
    Int arr[4] = {0};
    Int val = -100;
    Bool ret = FieldWrite(true, 1, &val, arr, XMZ_DATE_TYPE_INT, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_INT(-100, arr[1]);
}

/**
 * @brief 测试目的：FieldWrite DINT类型写入最小负数值
 * 输入数据：EN=true，INDEX=0，VALUE=&val(-2147483648)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_DINT，mem_len=4
 * 预期输出：返回 true；arr[0] = -2147483648
 */
TEST(FieldUtilTest, FieldWrite_Write_DInt_Min)
{
    DInt arr[4] = {0};
    DInt val = -2147483647 - 1;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_DINT, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_INT32(-2147483647 - 1, arr[0]);
}

/**
 * @brief 测试目的：FieldWrite LREAL(double)类型正常写入
 * 输入数据：EN=true，INDEX=1，VALUE=&val(2.718281828)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_LREAL，mem_len=3
 * 预期输出：返回 true；arr[1] ≈ 2.718281828
 */
TEST(FieldUtilTest, FieldWrite_Write_LReal)
{
    LReal arr[3] = {0.0, 0.0, 0.0};
    LReal val = 2.718281828;
    Bool ret = FieldWrite(true, 1, &val, arr, XMZ_DATE_TYPE_LREAL, 3);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_DOUBLE_WITHIN(0.0001, 2.718281828, arr[1]);
}

/**
 * @brief 测试目的：FieldWrite INDEX=0边界测试（写入首元素）
 * 输入数据：EN=true，INDEX=0，VALUE=&val(0xFFFF)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 * 预期输出：返回 true；arr[0] = 0xFFFF
 */
TEST(FieldUtilTest, FieldWrite_Index_Zero)
{
    Word arr[4] = {0};
    Word val = 0xFFFF;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_WORD, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, arr[0]);
}

/**
 * @brief 测试目的：FieldWrite INDEX=mem_len-1边界测试（写入末尾元素）
 * 输入数据：EN=true，INDEX=3，VALUE=&val(0xBBBB)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=4
 * 预期输出：返回 true；arr[3] = 0xBBBB
 */
TEST(FieldUtilTest, FieldWrite_Index_LastElement)
{
    Word arr[4] = {0};
    Word val = 0xBBBB;
    Bool ret = FieldWrite(true, 3, &val, arr, XMZ_DATE_TYPE_WORD, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT16(0xBBBB, arr[3]);
}

/**
 * @brief 测试目的：FieldWrite mem_len=0边界测试（数组长度为0，任何索引均应越界）
 * 输入数据：EN=true，INDEX=0，VALUE=&val(0x1234)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_WORD，mem_len=0
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, FieldWrite_MemLen_Zero)
{
    Word arr[4] = {0};
    Word val = 0x1234;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_WORD, 0);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：FieldWrite DATE类型写入
 * 输入数据：EN=true，INDEX=0，VALUE=&val(500)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_DATE，mem_len=2
 * 预期输出：返回 true；arr[0] = 500
 */
TEST(FieldUtilTest, FieldWrite_Write_Date)
{
    Date arr[2] = {0};
    Date val = 500;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_DATE, 2);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_HEX64(500, arr[0]);
}

/**
 * @brief 测试目的：FieldWrite CHAR类型写入
 * 输入数据：EN=true，INDEX=1，VALUE=&val('A')，MEMBER=arr，oper_type=XMZ_DATE_TYPE_CHAR，mem_len=4
 * 预期输出：返回 true；arr[1] = 'A'
 */
TEST(FieldUtilTest, FieldWrite_Write_Char)
{
    Char arr[4] = {0};
    Char val = 'A';
    Bool ret = FieldWrite(true, 1, &val, arr, XMZ_DATE_TYPE_CHAR, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_HEX8('A', arr[1]);
}

/**
 * @brief 测试目的：FieldWrite USINT类型写入（无符号8位最大值）
 * 输入数据：EN=true，INDEX=0，VALUE=&val(255)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_USINT，mem_len=4
 * 预期输出：返回 true；arr[0] = 255
 */
TEST(FieldUtilTest, FieldWrite_Write_USInt)
{
    USInt arr[4] = {0};
    USInt val = 255;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_USINT, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(255, arr[0]);
}

/**
 * @brief 测试目的：FieldWrite SINT类型写入（有符号8位最小值）
 * 输入数据：EN=true，INDEX=0，VALUE=&val(-128)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_SINT，mem_len=4
 * 预期输出：返回 true；arr[0] = -128
 */
TEST(FieldUtilTest, FieldWrite_Write_SInt)
{
    SInt arr[4] = {0};
    SInt val = -128;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_SINT, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_INT8(-128, arr[0]);
}

/**
 * @brief 测试目的：FieldWrite LTOD未实现类型分支
 * 输入数据：EN=true，INDEX=0，VALUE=&val(100)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_LTOD，mem_len=2
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, FieldWrite_NotImplType_LTOD)
{
    Date arr[2];
    Date val = 100;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_LTOD, 2);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：FieldWrite LDT未实现类型分支
 * 输入数据：EN=true，INDEX=0，VALUE=&val(100)，MEMBER=arr，oper_type=XMZ_DATE_TYPE_LDT，mem_len=2
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, FieldWrite_NotImplType_LDT)
{
    Date arr[2];
    Date val = 100;
    Bool ret = FieldWrite(true, 0, &val, arr, XMZ_DATE_TYPE_LDT, 2);
    TEST_ASSERT_FALSE(ret);
}

/**************************** SET_BF_BITS 补充测试（边界/异常） ****************************/

/**
 * @brief 测试目的：SET_BF_BITS 单字节内正常置位（bit_start=0，count=3）
 * 输入数据：en=true，count=3，base=buf，bit_start=0；buf初始{0x00,0x00}
 * 预期输出：返回 true；buf[0]=0x07（bit0、bit1、bit2置1），buf[1]=0x00
 */
TEST(FieldUtilTest, SET_BF_BITS_Normal_SingleByte)
{
    uint8_t buf[2] = {0, 0};
    Bool ret = SET_BF_BITS(true, 3, buf, 0);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x07, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, buf[1]);
}

/**
 * @brief 测试目的：SET_BF_BITS count=1边界测试
 * 输入数据：en=true，count=1，base=buf，bit_start=4；buf初始{0x00,0x00}
 * 预期输出：返回 true；buf[0]=0x10（bit4置1）
 */
TEST(FieldUtilTest, SET_BF_BITS_Count_One)
{
    uint8_t buf[2] = {0, 0};
    Bool ret = SET_BF_BITS(true, 1, buf, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x10, buf[0]);
}

/**
 * @brief 测试目的：SET_BF_BITS count=0边界测试（不操作任何位）
 * 输入数据：en=true，count=0，base=buf，bit_start=0；buf初始{0xFF,0xFF}
 * 预期输出：返回 true；buf内容不变
 */
TEST(FieldUtilTest, SET_BF_BITS_Count_Zero)
{
    uint8_t buf[2] = {0xFF, 0xFF};
    Bool ret = SET_BF_BITS(true, 0, buf, 0);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0xFF, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, buf[1]);
}

/**
 * @brief 测试目的：SET_BF_BITS bit_start=7（字节内最后一个bit位置）
 * 输入数据：en=true，count=1，base=buf，bit_start=7；buf初始{0x00,0x00}
 * 预期输出：返回 true；buf[0]=0x80（bit7置1）
 */
TEST(FieldUtilTest, SET_BF_BITS_BitStart_LastBit)
{
    uint8_t buf[2] = {0, 0};
    Bool ret = SET_BF_BITS(true, 1, buf, 7);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x80, buf[0]);
}

/**
 * @brief 测试目的：SET_BF_BITS count=9（超过8位，跨越两个完整字节）
 * 输入数据：en=true，count=9，base=buf，bit_start=0；buf初始{0x00,0x00}
 * 预期输出：返回 true；buf[0]=0xFF，buf[1]=0x01
 */
TEST(FieldUtilTest, SET_BF_BITS_Count_Gt8)
{
    uint8_t buf[4] = {0};
    Bool ret = SET_BF_BITS(true, 9, buf, 0);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0xFF, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0x01, buf[1]);
}

/**
 * @brief 测试目的：SET_BF_BITS 在已有数据上置位（部分位已为1，验证不影响其他位）
 * 输入数据：en=true，count=4，base=buf，bit_start=2；buf初始{0b10101010,0x00}
 * 预期输出：返回 true；buf[0]=0b10111110（bit2~bit5置1，其他位保持不变）
 */
TEST(FieldUtilTest, SET_BF_BITS_OnExistingData)
{
    uint8_t buf[2] = {0x55, 0x00};
    Bool ret = SET_BF_BITS(true, 4, buf, 2);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x7D, buf[0]);
}

/**************************** RESET_BF_BITS 补充测试（边界/异常） ****************************/

/**
 * @brief 测试目的：RESET_BF_BITS 单字节内正常复位（bit_start=0，count=3）
 * 输入数据：en=true，count=3，base=buf，bit_start=0；buf初始{0xFF,0xFF}
 * 预期输出：返回 true；buf[0]=0xF8（bit0、bit1、bit2清零），buf[1]=0xFF
 */
TEST(FieldUtilTest, RESET_BF_BITS_Normal_SingleByte)
{
    uint8_t buf[2] = {0xFF, 0xFF};
    Bool ret = RESET_BF_BITS(true, 3, buf, 0);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0xF8, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, buf[1]);
}

/**
 * @brief 测试目的：RESET_BF_BITS count=1边界测试
 * 输入数据：en=true，count=1，base=buf，bit_start=4；buf初始{0xFF,0xFF}
 * 预期输出：返回 true；buf[0]=0xEF（bit4清零）
 */
TEST(FieldUtilTest, RESET_BF_BITS_Count_One)
{
    uint8_t buf[2] = {0xFF, 0xFF};
    Bool ret = RESET_BF_BITS(true, 1, buf, 4);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0xEF, buf[0]);
}

/**
 * @brief 测试目的：RESET_BF_BITS count=0边界测试（不操作任何位）
 * 输入数据：en=true，count=0，base=buf，bit_start=0；buf初始{0x00,0x00}
 * 预期输出：返回 true；buf内容不变
 */
TEST(FieldUtilTest, RESET_BF_BITS_Count_Zero)
{
    uint8_t buf[2] = {0x00, 0x00};
    Bool ret = RESET_BF_BITS(true, 0, buf, 0);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x00, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, buf[1]);
}

/**
 * @brief 测试目的：RESET_BF_BITS bit_start=7（字节内最后一个bit位置）
 * 输入数据：en=true，count=1，base=buf，bit_start=7；buf初始{0xFF,0xFF}
 * 预期输出：返回 true；buf[0]=0x7F（bit7清零）
 */
TEST(FieldUtilTest, RESET_BF_BITS_BitStart_LastBit)
{
    uint8_t buf[2] = {0xFF, 0xFF};
    Bool ret = RESET_BF_BITS(true, 1, buf, 7);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x7F, buf[0]);
}

/**
 * @brief 测试目的：RESET_BF_BITS count=9（超过8位，跨越两个完整字节）
 * 输入数据：en=true，count=9，base=buf，bit_start=0；buf初始{0xFF,0xFF,0xFF,0xFF}
 * 预期输出：返回 true；buf[0]=0x00，buf[1]=0xFE
 */
TEST(FieldUtilTest, RESET_BF_BITS_Count_Gt8)
{
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    Bool ret = RESET_BF_BITS(true, 9, buf, 0);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x00, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0xFE, buf[1]);
}

/**
 * @brief 测试目的：RESET_BF_BITS 在已有数据上复位（部分位已为0，验证不影响其他位）
 * 输入数据：en=true，count=4，base=buf，bit_start=2；buf初始{0b01010101,0xFF}
 * 预期输出：返回 true；buf[0]=0b01000001（bit2~bit5清零，其他位保持不变）
 */
TEST(FieldUtilTest, RESET_BF_BITS_OnExistingData)
{
    uint8_t buf[2] = {0x55,0xFF};
    Bool ret = RESET_BF_BITS(true, 4, buf, 2);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(0x41, buf[0]);
}

/**************************** Swap 补充测试（边界/特殊值） ****************************/

/**
 * @brief 测试目的：Swap sizeBytes=0边界测试（不支持的字节数）
 * 输入数据：enable=true，inValue=&in(0x12)，sizeBytes=0，outValue=&out
 * 预期输出：返回 false
 */
TEST(FieldUtilTest, Swap_Size_Zero)
{
    Byte in = 0x12;
    Byte out;
    Bool ret = Swap(true, &in, 0, &out);
    TEST_ASSERT_FALSE(ret);
}

/**
 * @brief 测试目的：Swap WORD回文值（对称值，反转后不变）
 * 输入数据：enable=true，inValue=&in(0xFFFF)，sizeBytes=2，outValue=&out
 * 预期输出：返回 true，out = 0xFFFF
 */
TEST(FieldUtilTest, Swap_WORD_Palindrome)
{
    Word in = 0xFFFF;
    Word out;
    Bool ret = Swap(true, &in, 2, &out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, out);
}

/**
 * @brief 测试目的：Swap WORD零值
 * 输入数据：enable=true，inValue=&in(0x0000)，sizeBytes=2，outValue=&out
 * 预期输出：返回 true，out = 0x0000
 */
TEST(FieldUtilTest, Swap_WORD_Zero)
{
    Word in = 0x0000;
    Word out;
    Bool ret = Swap(true, &in, 2, &out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT16(0x0000, out);
}

/**
 * @brief 测试目的：Swap DWORD回文值（对称值）
 * 输入数据：enable=true，inValue=&in(0x12344321U)，sizeBytes=4，outValue=&out
 * 预期输出：返回 true，out = 0x21433412U
 */
TEST(FieldUtilTest, Swap_DWORD_Palindrome)
{
    DWord in = 0x12344321U;
    DWord out;
    Bool ret = Swap(true, &in, 4, &out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT32(0x2133412U, out);
}

/**
 * @brief 测试目的：Swap LWORD零值
 * 输入数据：enable=true，inValue=&in(0)，sizeBytes=8，outValue=&out
 * 预期输出：返回 true，out = 0
 */
TEST(FieldUtilTest, Swap_LWORD_Zero)
{
    LWord in = 0;
    LWord out;
    Bool ret = Swap(true, &in, 8, &out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT64(0, out);
}

/**
 * @brief 测试目的：Swap LWORD全1值（回文，反转后不变）
 * 输入数据：enable=true，inValue=&in(0xFFFFFFFFFFFFFFFFULL)，sizeBytes=8，outValue=&out
 * 预期输出：返回 true，out = 0xFFFFFFFFFFFFFFFFULL
 */
TEST(FieldUtilTest, Swap_LWORD_AllOnes)
{
    LWord in = 0xFFFFFFFFFFFFFFFFULL;
    LWord out;
    Bool ret = Swap(true, &in, 8, &out);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT64(0xFFFFFFFFFFFFFFFFULL, out);
}

TEST_GROUP_RUNNER(FieldUtilTest)
{
    // FieldWrite
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_EN_False);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_NULL_Ptr);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Index_Negative);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Index_OverUpper);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_Word_Normal);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_Real);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_NotImplType_DT);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Default_BadType);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_Bool);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_Byte);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_DWord);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_LWord);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_Int);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_DInt_Min);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_LReal);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Index_Zero);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Index_LastElement);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_MemLen_Zero);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_Date);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_Char);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_USInt);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_Write_SInt);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_NotImplType_LTOD);
    RUN_TEST_CASE(FieldUtilTest, FieldWrite_NotImplType_LDT);
    // SET_BF_BITS
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_InvalidArg);
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_Normal_CrossByte);
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_Normal_SingleByte);
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_Count_One);
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_Count_Zero);
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_BitStart_LastBit);
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_Count_Gt8);
    RUN_TEST_CASE(FieldUtilTest, SET_BF_BITS_OnExistingData);
    // RESET_BF_BITS
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_InvalidArg);
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_Normal_CrossByte);
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_Normal_SingleByte);
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_Count_One);
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_Count_Zero);
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_BitStart_LastBit);
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_Count_Gt8);
    RUN_TEST_CASE(FieldUtilTest, RESET_BF_BITS_OnExistingData);
    // Swap
    RUN_TEST_CASE(FieldUtilTest, Swap_EN_False);
    RUN_TEST_CASE(FieldUtilTest, Swap_NULL_Ptr);
    RUN_TEST_CASE(FieldUtilTest, Swap_WORD);
    RUN_TEST_CASE(FieldUtilTest, Swap_DWORD);
    RUN_TEST_CASE(FieldUtilTest, Swap_LWORD);
    RUN_TEST_CASE(FieldUtilTest, Swap_BadSize);
    RUN_TEST_CASE(FieldUtilTest, Swap_Size_Zero);
    RUN_TEST_CASE(FieldUtilTest, Swap_WORD_Palindrome);
    RUN_TEST_CASE(FieldUtilTest, Swap_WORD_Zero);
    RUN_TEST_CASE(FieldUtilTest, Swap_DWORD_Palindrome);
    RUN_TEST_CASE(FieldUtilTest, Swap_LWORD_Zero);
    RUN_TEST_CASE(FieldUtilTest, Swap_LWORD_AllOnes);
}

int field_util_run(void)
{
    UNITY_BEGIN();
    RUN_TEST_GROUP(FieldUtilTest);
    return UNITY_END();
}