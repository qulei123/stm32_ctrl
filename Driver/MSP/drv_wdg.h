#ifndef __DRV_WDG_H_
#define __DRV_WDG_H_

#include "drv_msp.h"

int Drv_Iwdg_Init(uint32_t timeout);
void Drv_Iwdg_Feed(void);

#endif /* __DRV_WDG_H_ */

