#include "shiftCycle.h"
#include <stdint.h>
#include <string.h> // For memcpy

Bool SHR(Bool en, XMZ_DATE_TYPE sh_type, const void* in, int n, void* out) {
  if(!en || in == NULL || out == NULL) {
    return false;
  }

  switch(sh_type) {
    case XMZ_DATE_TYPE_USINT: {
      const USInt* in_val  = (const USInt*) in;
      uint8_t      result  = *in_val >> n;
      USInt*       out_val = (USInt*) out;
      *out_val             = result;
      break;
    }
    case XMZ_DATE_TYPE_UINT: {
      const UInt* in_val  = (const UInt*) in;
      uint16_t    result  = *in_val >> n;
      UInt*       out_val = (UInt*) out;
      *out_val            = result;
      break;
    }
    case XMZ_DATE_TYPE_WORD: {
      const Word* in_val  = (const Word*) in;
      uint16_t    result  = *in_val >> n;
      Word*       out_val = (Word*) out;
      *out_val            = result;
      break;
    }
    case XMZ_DATE_TYPE_DWORD: {
      const DWord* in_val  = (const DWord*) in;
      uint32_t     result  = *in_val >> n;
      DWord*       out_val = (DWord*) out;
      *out_val             = result;
      break;
    }
    case XMZ_DATE_TYPE_BYTE: {
      const Byte* in_val  = (const Byte*) in;
      uint8_t     result  = *in_val >> n;
      Byte*       out_val = (Byte*) out;
      *out_val            = result;
      break;
    }
    case XMZ_DATE_TYPE_UDINT: {
      const UDInt* in_val  = (const UDInt*) in;
      uint32_t     result  = *in_val >> n;
      UDInt*       out_val = (UDInt*) out;
      *out_val             = result;
      break;
    }
    case XMZ_DATE_TYPE_SINT: {
      const SInt* in_val = (const SInt*) in;
      int8_t      result;
      if(n == 0) {
        result = *in_val;
      } else {
        if(*in_val < 0) {
          // Arithmetic shift: sign extend
          result = *in_val >> n;
          if(n < 8) {
            result |= (-1 << (8 - n));
          } else {
            result =
                -1; // 若 n 大于等于 8，则为
                    // 0；但由于右移（>>）操作会用符号位填充（空位），而对于完全移位（情况），（移位后）所有位都将是符号位。
          }
        } else {
          result = *in_val >> n;
        }
      }
      SInt* out_val = (SInt*) out;
      *out_val      = result;
      break;
    }
    case XMZ_DATE_TYPE_INT: {
      const Int* in_val = (const Int*) in;
      int16_t    result;
      if(n == 0) {
        result = *in_val;
      } else {
        if(*in_val < 0) {
          result = *in_val >> n;
          if(n < 16) {
            result |= (-1 << (16 - n));
          } else {
            result = -1;
          }
        } else {
          result = *in_val >> n;
        }
      }
      Int* out_val = (Int*) out;
      *out_val     = result;
      break;
    }
    case XMZ_DATE_TYPE_DINT: {
      const DInt* in_val = (const DInt*) in;
      int32_t     result;
      if(n == 0) {
        result = *in_val;
      } else {
        if(*in_val < 0) {
          result = *in_val >> n;
          if(n < 32) {
            result |= (-1 << (32 - n));
          } else {
            result = -1;
          }
        } else {
          result = *in_val >> n;
        }
      }
      DInt* out_val = (DInt*) out;
      *out_val      = result;
      break;
    }
    default:
      // For CYCLE types, no operation or copy? But according to spec, SHR doesn't handle cycle, so return false or
      // copy? Assuming only shift types are supported, return false
      return false;
  }
  return true;
}

Bool SHL(Bool en, XMZ_DATE_TYPE sh_type, const void* in, int n, void* out) {
  if(!en || in == NULL || out == NULL) {
    return false;
  }

  // 对于 SHL（左移）操作，所有情况均为逻辑左移，右侧用 0
  // 填充；由于无论是有符号数还是无符号数，其正数的左移结果都是相同的，
  // 但（左移）需要为负数保留符号位吗？不需要，规范明确要求用 0 填充，因此所有（左移）情况均为逻辑左移。不过在 C
  // 语言中，对于负数执行 <<（左移）操作，
  // 在某些情况下结果是未定义的，但我们可以先将其强制转换为无符号类型再进行移位操作

  switch(sh_type) {
    case XMZ_DATE_TYPE_USINT:
    case XMZ_DATE_TYPE_SINT:
    case XMZ_DATE_TYPE_BYTE: {
      const uint8_t* in_val  = (const uint8_t*) in;   // Treat as unsigned for shift
      uint8_t        result  = (*in_val << n) & 0xFF; // Mask to 8 bits
      uint8_t*       out_val = (uint8_t*) out;
      *out_val               = result;
      break;
    }
    case XMZ_DATE_TYPE_UINT:
    case XMZ_DATE_TYPE_WORD:
    case XMZ_DATE_TYPE_INT: {
      const uint16_t* in_val  = (const uint16_t*) in;
      uint16_t        result  = (*in_val << n) & 0xFFFF;
      uint16_t*       out_val = (uint16_t*) out;
      *out_val                = result;
      break;
    }
    case XMZ_DATE_TYPE_DWORD:
    case XMZ_DATE_TYPE_UDINT:
    case XMZ_DATE_TYPE_DINT: {
      const uint32_t* in_val  = (const uint32_t*) in;
      uint32_t        result  = (*in_val << n) & 0xFFFFFFFFU;
      uint32_t*       out_val = (uint32_t*) out;
      *out_val                = result;
      break;
    }
    default:
      return false;
  }
  return true;
}
Bool ROR(Bool en, XMZ_DATE_TYPE cycly_type, const void* in, int n, void* out) {
  if(!en || in == NULL || out == NULL) {
    return false;
  }

  // Normalize n to avoid unnecessary rotations
  switch(cycly_type) {
    case XMZ_DATE_TYPE_BYTE: {
      const Byte* in_val = (const Byte*) in;
      uint8_t     val    = *in_val;
      n                  = n % 8; // Normalize to 0-7
      uint8_t result     = (val >> n) | (val << (8 - n));
      Byte*   out_val    = (Byte*) out;
      *out_val           = result;
      break;
    }
    case XMZ_DATE_TYPE_WORD: {
      const Word* in_val = (const Word*) in;
      uint16_t    val    = *in_val;
      n                  = n % 16; // Normalize to 0-15
      uint16_t result    = (val >> n) | (val << (16 - n));
      Word*    out_val   = (Word*) out;
      *out_val           = result;
      break;
    }
    case XMZ_DATE_TYPE_DWORD: {
      const DWord* in_val = (const DWord*) in;
      uint32_t     val    = *in_val;
      n                   = n % 32; // Normalize to 0-31
      uint32_t result     = (val >> n) | (val << (32 - n));
      DWord*   out_val    = (DWord*) out;
      *out_val            = result;
      break;
    }
    default:
      return false;
  }
  return true;
}

Bool ROL(Bool en, XMZ_DATE_TYPE cycly_type, const void* in, int n, void* out) {
  if(!en || in == NULL || out == NULL) {
    return false;
  }

  // Normalize n to avoid unnecessary rotations
  switch(cycly_type) {
    case XMZ_DATE_TYPE_BYTE: {
      const Byte* in_val = (const Byte*) in;
      uint8_t     val    = *in_val;
      n                  = n % 8; // Normalize to 0-7
      uint8_t result     = (val << n) | (val >> (8 - n));
      Byte*   out_val    = (Byte*) out;
      *out_val           = result;
      break;
    }
    case XMZ_DATE_TYPE_WORD: {
      const Word* in_val = (const Word*) in;
      uint16_t    val    = *in_val;
      n                  = n % 16; // Normalize to 0-15
      uint16_t result    = (val << n) | (val >> (16 - n));
      Word*    out_val   = (Word*) out;
      *out_val           = result;
      break;
    }
    case XMZ_DATE_TYPE_DWORD: {
      const DWord* in_val = (const DWord*) in;
      uint32_t     val    = *in_val;
      n                   = n % 32; // Normalize to 0-31
      uint32_t result     = (val << n) | (val >> (32 - n));
      DWord*   out_val    = (DWord*) out;
      *out_val            = result;
      break;
    }
    default:
      return false;
  }
  return true;
}
