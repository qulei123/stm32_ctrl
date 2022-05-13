#ifndef __DRV_DELAY_H
#define __DRV_DELAY_H

#include "stm32f1xx_hal.h"

#define USE_DWT_DELAY   1           /* 使用dwt内核精确延时 */


#if USE_DWT_DELAY

/* 获取内核每us频率 */
#define SysClkFreqPerUs         (SystemCoreClock / 1000000)


void Drv_DWT_Deday_Init(void);
void Drv_usDeday(uint32_t usDelay);


#define usDeday(us)     Drv_usDeday(us)
#define msDeday(ms)     Drv_usDeday(ms * 1000)


#endif

#endif /* __DRV_DELAY_H */

