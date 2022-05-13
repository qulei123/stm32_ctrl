
#ifndef __STM32F1XX_CLK_H
#define __STM32F1XX_CLK_H

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config(void);
uint32_t Drv_UpdateLSIFreq(void);
uint32_t Drv_GetLSIFreq(void);

#ifdef __cplusplus
}
#endif /* cplusplus */

#endif /* system_clk.h */


