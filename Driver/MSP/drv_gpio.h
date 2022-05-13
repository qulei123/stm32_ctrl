
#ifndef __DRV_GPIO_H_
#define __DRV_GPIO_H_

#include "drv_msp.h"

#define DRV_DISABLE                 0
#define DRV_ENABLE                  1

uint8_t Drv_Gpio_Read(uint32_t Id);
/* 输出唤醒 */
void Drv_Wakeup_Ctrl(int Val);
/* 充电相关接口 */
void Drv_Charge_Enable(int Val);
void Drv_Charge_Mode(int Val);
/* 控制电池供电 */
void Drv_Battery2Mcu_Start(void);
void Drv_Battery2Mcu_Stop(void);
/* led闪烁 */
void Drv_Led_Blink(void);
/* 红外功能使能 */
void Drv_Ired_Enable(int Val);
/* 舵机供电控制 */
void Drv_Motor_Power(int Val);


#endif  // __DRV_GPIO_H_
