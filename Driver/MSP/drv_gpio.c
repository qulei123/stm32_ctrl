/*  
 * 文件名称：drv_gpio.c
 * 摘要：
 *  
 * 修改历史   版本号   Author  修改内容
 *--------------------------------------------------
 * 2022.03.29   v1      ql     创建文件
 *--------------------------------------------------
 */

#include <string.h>
#include "util_log.h"
#include "drv_gpio.h"


static PT_GPIO_InitInfo Drv_Gpio_Get_Info(uint32_t Id)
{
    int idx = 0;
    PT_GPIO_InitInfo ptGpioInfo = Drv_Msp_GpioInfo();

    for (idx = 0; idx < ID_MAX; idx++)    
    {
        if (ptGpioInfo[idx].Id == Id)
        {
            return ptGpioInfo + idx;
        }
    }

    return NULL;
}

uint8_t Drv_Gpio_Read(uint32_t Id)
{
    PT_GPIO_InitInfo ptGpioInfo = NULL;
    
    ptGpioInfo = Drv_Gpio_Get_Info(Id);
    if (NULL == ptGpioInfo)
    {
        log_w("deft gpio[0]\n");
        ptGpioInfo = Drv_Msp_GpioInfo();
    }

    return HAL_GPIO_ReadPin(ptGpioInfo->GPIOx, ptGpioInfo->GPIO_Init.Pin);
}

/* 输出唤醒 */
void Drv_Wakeup_Ctrl(int Val)
{
    HAL_GPIO_WritePin(WKUP_GPIO_PORT, WKUP_GPIO_PIN, (GPIO_PinState)Val);
}


/* 充电相关接口 */
void Drv_Charge_Enable(int Val)
{
    HAL_GPIO_WritePin(CHARGE_EN_PORT, CHARGE_EN_PIN, (GPIO_PinState)Val);
}

void Drv_Charge_Mode(int Val)
{
    HAL_GPIO_WritePin(CHARGE_MODE_PORT, CHARGE_MODE_PIN, (GPIO_PinState)Val);
}

/* 控制电池供电:  
 *  AC_OK_H -> PB8 -> 高电平有效
 */
void Drv_Battery2Mcu_Start(void)
{
    HAL_GPIO_WritePin(CHARGE_CTRL_PORT, CHARGE_CTRL_PIN, GPIO_PIN_SET);
}

void Drv_Battery2Mcu_Stop(void)
{
    HAL_GPIO_WritePin(CHARGE_CTRL_PORT, CHARGE_CTRL_PIN, GPIO_PIN_RESET);
}

/* led */
void Drv_Led_Blink(void)
{
    HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
}

/* Ired -> 低电平使能 */
void Drv_Ired_Enable(int Val)
{
    HAL_GPIO_WritePin(IRED_EN_GPIO_PORT, IRED_EN_GPIO_PIN, (GPIO_PinState)!Val);
}

/* 舵机供电:  
 *  DJ_PWR_CTR_H -> PB7 -> 高电平有效
 */
void Drv_Motor_Power(int Val)
{
    HAL_GPIO_WritePin(MOTOR_PWR_PORT, MOTOR_PWR_PIN, (GPIO_PinState)Val);
}


