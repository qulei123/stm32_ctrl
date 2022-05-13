/* 
 * 文件名称：common.c
 * 摘要：
 *  
 * 修改历史        版本号     Author  修改内容
 *--------------------------------------------------
 * 2022.01.13   v1      ql     创建文件
 *--------------------------------------------------
 */
 
#include "common.h"


U32 u32_Bit_Reverse(U32 n)
{
    n = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
    n = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
    n = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
    n = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
    n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);

    return n;
}

U16 u16_Bit_Reverse(U16 n)
{
    n = ((n >> 1) & 0x5555) | ((n << 1) & 0xaaaa);
    n = ((n >> 2) & 0x3333) | ((n << 2) & 0xcccc);
    n = ((n >> 4) & 0x0f0f) | ((n << 4) & 0xf0f0);
    n = ((n >> 8) & 0x00ff) | ((n << 8) & 0xff00);

    return n;
}

/* 统计二进制数中1的个数 */
U8 BitCount(U32 n) 
{ 
    n = (n &0x55555555) + ((n >>1) &0x55555555) ; 
    n = (n &0x33333333) + ((n >>2) &0x33333333) ; 
    n = (n &0x0f0f0f0f) + ((n >>4) &0x0f0f0f0f) ; 
    n = (n &0x00ff00ff) + ((n >>8) &0x00ff00ff) ; 
    n = (n &0x0000ffff) + ((n >>16) &0x0000ffff) ; 

    return n ; 
}

/* 计算CRC校验码 */
U8 Calc_ChkSum(U8 *pcBuf, U16 u16Len)
{
    U32 u32Sum = 0;
    
    for(int i = 0; i < u16Len; i++)
    {
        u32Sum += pcBuf[i];
    }
    
    return (U8)(u32Sum % 256);
}

#if 0
void Dev_RST(void)
{
    DBG_String("RST\n");
    msDelay(1);
    __set_FAULTMASK(1);	 	// 屏蔽所有中断
    NVIC_SystemReset(); 	// 软件复位	
}
#endif
