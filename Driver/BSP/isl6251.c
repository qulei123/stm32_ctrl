/* 
 * 文件名称：charge.c
 * 摘要：使用 ISL6251A 芯片进行充电管理
 *  
 * 修改历史   版本号   Author  修改内容
 *--------------------------------------------------
 * 2022.03.14   v1      ql     创建文件
 *--------------------------------------------------
 */
 
#include "isl6251.h"
#include "drv_gpio.h"

/* 初始化 */
void ISL6251_Init(void)
{
    /* gpio初始化统一在msp中处理 */
    return;
}

/* 适配器检测: 通过ISL6251A中的ACPRN引脚判断
 *   1. ACPRN   -> PB3 -> Int
 *   2. ADP2_DT -> PB4
 *   1/2 功能一样使用一个即可
 */
int ISL6251_DetectAdapter(void)
{
    /* 通过 int/poll(同按键一块处理) 获取状态 */
    return 0;
}


/* 充电启停: 控制ISL6251A中的EN引脚
 *  1. EN -> PB5
 */
void ISL6251_SetChargeStatus(int iVal)
{
    Drv_Charge_Enable(iVal);
}

/* 充电模式: 控制ISL6251A中的CHLIM引脚
 *  1. CHLIM -> PB6, 高电平为快速充电
 */
void ISL6251_SetChargeMode(int iVal)
{
    Drv_Charge_Mode(iVal);
}

/* 控制电池供电: 这个不是ISL6251A芯片的功能
 *  AC_OK_H -> PB8 -> 高电平有效
 */
void ISL6251_SupplyCtrl(int iStat)
{
    if (1 == iStat)
    {
        Drv_Battery2Mcu_Start();
    }
    else
    {
        Drv_Battery2Mcu_Stop();
    }
}


/* 获取充电电流: 通过ISL6251A中的ICM计算获取
 *  1. ICM -> PC4 -> AD
 *     ICM = 19.9 * I * R2(20mR)
 */
int Charge_Current_Get(void)
{
    /* 暂时不实现 */
    return 1;
}


T_DrvChargeFunc tISL6251_Drv =
{
    0,
    0,
    0,
    ISL6251_Init,
    ISL6251_DetectAdapter,    
    ISL6251_SetChargeStatus,
    ISL6251_SetChargeMode,
    ISL6251_SupplyCtrl,
};

