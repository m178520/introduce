#ifndef __TEST_H
#define __TEST_H

#include "xmzTypeDefine.h"

Bool FieldWrite(Bool EN, DInt INDEX, const void* VALUE, void* MEMBER, XMZ_DATE_TYPE oper_type, size_t mem_len);
Bool SET_BF_BITS(Bool en, UInt count, uint8_t* base, uint16_t bit_start);
Bool RESET_BF_BITS(Bool en, UInt count, uint8_t* base, uint16_t bit_start);
Bool Swap(Bool enable, const void* inValue, uint8_t sizeBytes, void* outValue) ;
	
#endif
