#ifndef __UTIL_OW_BITOPS_H_
#define __UTIL_OW_BITOPS_H_

#include "stdint.h"

#define SDA_MODE_INPUT   0
#define SDA_MODE_OUTPUT  1

/* 对于stm32开漏输出，对输入数据寄存器的读访问可得到I/O状态
 * 不需要频繁切换gpio口输入输出方式
 */
#define DRAIN_MODE


/* 硬件操作结构体 */
typedef struct tOneWire_Ops
{
    void (*Sda_Cfg)(int iMode);
    void (*Sda_Set)(int iStatus);
    int (*Sda_Get)(void);
    void (*DelayUs)(uint32_t us);
}T_OneWire_Ops, *PT_OneWire_Ops;

int Ow_Reset(PT_OneWire_Ops ptOneWire);
uint8_t Ow_Read_Byte(PT_OneWire_Ops ptOneWire);
void Ow_Write_Byte(PT_OneWire_Ops ptOneWire, uint8_t u8Byte);

#endif  // __UTIL_OW_BITOPS_H_

