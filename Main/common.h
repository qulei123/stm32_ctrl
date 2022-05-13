#ifndef __COMMON_H
#define __COMMON_H

#include "deftypes.h"

U32 u32_Bit_Reverse(U32 n);
U16 u16_Bit_Reverse(U16 n);
U8 BitCount(U32 n);
U8 Calc_ChkSum(U8 *pcBuf, U16 u16Len);

/* Number of elements in an array */
#define N_ELEMENTS(array)    (sizeof(array) / sizeof(array[0]))

#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/********************** 获得该地址上的数据 **********************/
#define GET_1B_MEM(x)      (*((U8 *)(x)))
#define GET_2B_MEM(x)      (*((U16 *)(x)))
#define GET_4B_MEM(x)      (*((U32 *)(x)))

/***************** 将主机数据类型转化为大端模式（字符串模式） *******************/
#define HOST_TO_BE16(a) ((((U16) (a) << 8) & 0xff00) | (((U16) (a) >> 8) & 0xff))
#define HOST_TO_BE32(a) ((((U32) (a) << 24) & 0xff000000)      | \
                         (((U32) (a) << 8) & 0xff0000)          | \
                         (((U32) (a) >> 8) & 0xff00)            | \
                         (((U32) (a) >> 24) & 0xff))

#define BE16_TO_HOST   HOST_TO_BE16
#define BE32_TO_HOST   HOST_TO_BE32

#endif  //__COMMON_H
