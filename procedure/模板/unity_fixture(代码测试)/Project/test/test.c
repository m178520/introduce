#include "test.h"

// 底层位操作桩函数，单元测试模拟实现，项目中替换硬件驱动实现
static void set_bit_value(uint8_t *byte_ptr, int unused1, int bit, int val)
{
    if(val)
    {
        *byte_ptr |=  (1U << bit);
    }
    else
    {
        *byte_ptr &= ~(1U << bit);
    }
}

/**
 * @brief 写入域指令实现
 * 将 VALUE 的值写入数组 MEMBER 中指定索引位置（与 FieldRead 方向相反）
 * @param EN 使能输入
 * @param INDEX 索引位置
 * @param VALUE 输入值指针
 * @param MEMBER 数组指针（域首元素地址）
 * @param oper_type 数组元素的数据类型
 * @param mem_len 数组长度（从 MEMBER 起的剩余元素数）
 * @return Bool 操作是否成功
 */
Bool FieldWrite(Bool EN, DInt INDEX, const void* VALUE, void* MEMBER, XMZ_DATE_TYPE oper_type, size_t mem_len) 
	{
  // 如果未使能，返回 false
  if(!EN) {
    return false;
  }

  // 检查输入参数有效性
  if(MEMBER == NULL || VALUE == NULL) {
    return false;
  }

  // 检查索引是否越界
  if(INDEX < 0 || (size_t) INDEX >= mem_len) {
    return false;
  }

  // 根据操作类型转换为数组并写入数据
  switch(oper_type) {
    case XMZ_DATE_TYPE_BOOL: {
      const Bool* value = (const Bool*) VALUE;
      Bool*      array  = (Bool*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_BYTE: {
      const Byte* value = (const Byte*) VALUE;
      Byte*       array = (Byte*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_WORD: {
      const Word* value = (const Word*) VALUE;
      Word*       array = (Word*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_DWORD: {
      const DWord* value = (const DWord*) VALUE;
      DWord*       array = (DWord*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_LWORD: {
      const LWord* value = (const LWord*) VALUE;
      LWord*       array = (LWord*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_SINT: {
      const SInt* value = (const SInt*) VALUE;
      SInt*       array = (SInt*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_USINT: {
      const USInt* value = (const USInt*) VALUE;
      USInt*       array = (USInt*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_INT: {
      const Int* value = (const Int*) VALUE;
      Int*       array = (Int*) MEMBER;
      array[INDEX]     = *value;
      break;
    }
    case XMZ_DATE_TYPE_UINT: {
      const UInt* value = (const UInt*) VALUE;
      UInt*       array = (UInt*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_DINT: {
      const DInt* value = (const DInt*) VALUE;
      DInt*       array = (DInt*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_UDINT: {
      const UDInt* value = (const UDInt*) VALUE;
      UDInt*       array = (UDInt*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_LINT: {
      const LInt* value = (const LInt*) VALUE;
      LInt*       array = (LInt*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_ULINT: {
      const ULInt* value = (const ULInt*) VALUE;
      ULInt*       array = (ULInt*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_REAL: {
      const Real* value = (const Real*) VALUE;
      Real*       array = (Real*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_LREAL: {
      const LReal* value = (const LReal*) VALUE;
      LReal*       array = (LReal*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_TIME: {
      const Time* value = (const Time*) VALUE;
      Time*       array = (Time*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_LTIME: {
      const LTime* value = (const LTime*) VALUE;
      LTime*       array = (LTime*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_DATE: {
      const Date* value = (const Date*) VALUE;
      Date*       array = (Date*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_TOD: {
      const TimeOfDay* value = (const TimeOfDay*) VALUE;
      TimeOfDay*       array = (TimeOfDay*) MEMBER;
      array[INDEX]           = *value;
      break;
    }
    case XMZ_DATE_TYPE_CHAR: {
      const Char* value = (const Char*) VALUE;
      Char*       array = (Char*) MEMBER;
      array[INDEX]      = *value;
      break;
    }
    case XMZ_DATE_TYPE_WCHAR: {
      const WChar* value = (const WChar*) VALUE;
      WChar*       array = (WChar*) MEMBER;
      array[INDEX]       = *value;
      break;
    }
    case XMZ_DATE_TYPE_LTOD:
    case XMZ_DATE_TYPE_DT:
    case XMZ_DATE_TYPE_LDT:
      return false; // 未实现的数据类型
    default:
      return false; // 无效的操作类型
  }

  return true;
}



/// @brief 置位位域（打包位模式）：从 bit_start 起连续置位 count 个位
/// 与 SET_BF 的区别：SET_BF 按 Bool（1 字节/元素）memset，适用于 DB/L 的 Bool 数组；
/// 本函数按“打包位”逐位操作，适用于 Mem/Output_W 这类 8 位合 1 字节的 I/Q/M 区域。
/// @param en 使能
/// @param count 需要置位的位数
/// @param base 底层字节区域指针（如 Mem + 字节下标）
/// @param bit_start 起始位在 base 字节内的偏移（0~7）
/// @return Bool
Bool SET_BF_BITS(Bool en, UInt count, uint8_t* base, uint16_t bit_start) {
  if(!en || base == NULL)
    return false;
  for(UInt i = 0; i < count; i++) {
    uint16_t pos = bit_start + (uint16_t)i;
    set_bit_value(base + pos / 8, 1, pos % 8, 1);
  }
  return true;
}

/// @brief 复位位域（打包位模式）：从 bit_start 起连续复位 count 个位（适配 I/Q/M 的 8 位/字节存储）
Bool RESET_BF_BITS(Bool en, UInt count, uint8_t* base, uint16_t bit_start) {
  if(!en || base == NULL)
    return false;
  for(UInt i = 0; i < count; i++) {
    uint16_t pos = bit_start + (uint16_t)i;
    set_bit_value(base + pos / 8, 1, pos % 8, 0);
  }
  return true;
}
/*
 * @brief 模拟西门子 SWAP 指令（按字节反转顺序）
 *
 * 西门子语义：
 *   WORD  (2字节): 高低字节互换  0x1234              -> 0x3412
 *   DWORD (4字节): 完全字节反转  0x12345678          -> 0x78563412
 *   LWORD (8字节): 完全字节反转  0x0123456789ABCDEF  -> 0xEFCDAB8967452301
 */
Bool Swap(Bool enable, const void* inValue, uint8_t sizeBytes, void* outValue) {
  if(!enable)
    return false;

  if(inValue == NULL || outValue == NULL)
    return false;

  switch(sizeBytes) {
    case 2: // WORD: 高低字节互换 (0x1234 -> 0x3412)
    {
      uint16_t v            = *(const uint16_t*) inValue;
      uint16_t r            = (uint16_t)((v >> 8) | (v << 8));
      *(uint16_t*) outValue = r;
      return true;
    }
    case 4: // DWORD: 完全字节反转 (西门子: 0x12345678 -> 0x78563412)
    {
      uint32_t v = *(const uint32_t*) inValue;
      uint32_t r = ((v & 0x000000FFU) << 24) |
                   ((v & 0x0000FF00U) <<  8) |
                   ((v & 0x00FF0000U) >>  8) |
                   ((v & 0xFF000000U) >> 24);
      *(uint32_t*) outValue = r;
      return true;
    }
    case 8: // LWORD: 完全字节反转 (西门子: 0x0123456789ABCDEF -> 0xEFCDAB8967452301)
    {
      uint64_t v = *(const uint64_t*) inValue;
      uint64_t r = ((v & 0x00000000000000FFULL) << 56) |
                   ((v & 0x000000000000FF00ULL) << 40) |
                   ((v & 0x0000000000FF0000ULL) << 24) |
                   ((v & 0x00000000FF000000ULL) <<  8) |
                   ((v & 0x000000FF00000000ULL) >>  8) |
                   ((v & 0x0000FF0000000000ULL) >> 24) |
                   ((v & 0x00FF000000000000ULL) >> 40) |
                   ((v & 0xFF00000000000000ULL) >> 56);
      *(uint64_t*) outValue = r;
      return true;
    }
    default:
      return false;
  }
}
