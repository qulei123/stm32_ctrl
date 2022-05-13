#ifndef __DS18B20_H_
#define __DS18B20_H_


#include "drv_msp.h"
#include "drv_delay.h"

typedef struct
{  
  void  (*Init)(void);
  int   (*Start)(void);
  int   (*ReadTemp)(float *);
  int   bFlagStart;
}T_DrvTempFunc, *PT_DrvTempFunc;

extern T_DrvTempFunc tDs18b20_Drv;

#if 0

#define DS18B20_DQ_PIN              GPIO_PIN_6
#define DS18B20_DQ_PORT             GPIOD
#define DS18B20_DQ_CLK_EN()         __HAL_RCC_GPIOD_CLK_ENABLE()

/* IO方向设置, 方便操作使用寄存器方式 */
#define DS18B20_IO_IN()             {GPIOD->CRL &= 0XF0FFFFFF; GPIOD->CRL |= 8 << 24;}  /* 上拉输入 */
#define DS18B20_IO_OUT()            {GPIOD->CRL &= 0XF0FFFFFF; GPIOD->CRL |= 3 << 24;}  /* 推挽高速输出 */

//IO读写
#define DS18B20_DQ_OUT              PDout(6)
#define DS18B20_DQ_IN               PDin(6)

#endif

float Ds18b20_Get_Tempt(void);

#endif //__DS18B20_H_
