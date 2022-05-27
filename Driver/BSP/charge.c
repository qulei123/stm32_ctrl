/*  
 * 文件名称：charge.c
 * 摘要：使用 ISL6251A 芯片进行充电管理
 *  
 * 修改历史   版本号   Author  修改内容
 *--------------------------------------------------
 * 2022.03.14   v1      ql     创建文件
 *--------------------------------------------------
 */

#include "common.h"
#include "drv_adc.h"
#include "isl6251.h"
#include "ds18b20.h"
#include "mod_timer.h"
#include "host_link.h"
#include "charge.h"


static PT_DrvChargeFunc ptChargeDrv = &tISL6251_Drv;

/* int/poll -> 适配器接入 */
void OnAdapter_Handler(void)
{
    log("OnAdapter_Handler\n");
    /* 0. 启动电流检测, 温度检测 */
    
    /* 1. 启动充电, 快充模式, 电池供电停止 */
    ptChargeDrv->Set_Status(CHARGE_START);
    ptChargeDrv->Set_Mode(CHARGE_MODE_FAST);
    ptChargeDrv->Supply_Ctrl(BATTERY_STOP);
    ptChargeDrv->AdapterStatus = TRUE; 
}

/* int/poll -> 适配器断开 */
void OffAdapter_Handler(void)
{
    log("OffAdapter_Handler\n");
    
    ptChargeDrv->Supply_Ctrl(BATTERY_START);
    ptChargeDrv->Set_Mode(CHARGE_MODE_NOMAL);
    ptChargeDrv->Set_Status(CHARGE_STOP);
    ptChargeDrv->AdapterStatus = FALSE;
}   


MOD_TIMER_INSTANCE(tTimerTemp, 1000, 0);

/*   温度保护策略
 *     只要温度超过45度,  切换为慢充，不再切换回来
 */
void Temp_Handler(void)
{
    INT iRet;
    FLOAT ftempt;
    
    iRet = tDs18b20_Drv.ReadTemp(&ftempt);
    if (0 != iRet)
    {
        log_e("temp read fail");
        return;
    }
    
    if (ftempt > 65)
    {
        OffAdapter_Handler();
        /* 此时适配器并没有拔下 */
        ptChargeDrv->AdapterStatus = TRUE;
    }
    else if (ftempt > 45)
    {
        ptChargeDrv->Set_Mode(CHARGE_MODE_NOMAL);
    }
    else
    {
        //ptChargeDrv->Set_Mode(CHARGE_MODE_FAST);
    }
    tDs18b20_Drv.bFlagStart = FALSE;
    //log("wendu %0.2f\n", ftempt);
}

void ChargeProtect(void)
{
    INT iRet;
    U16 u16mVolt;
    //log("ChargeProtect\n");
    
    /* 1. 判断充电电池压, adc -> battery volt */
    u16mVolt = Drv_Obtain_Baty_Volt();
    if (u16mVolt > 8400)
    {
        OffAdapter_Handler();
        return;
    }
    /* 策略, 电压不主动上报, rk3399查询 */
#if REPORT_ENABLE  
    //Report_Battery_Cmd(u16mVolt);
#endif

    /*  2. 温度保护策略
     *     只要温度超过45度,  切换为慢充，不再切换回来
     */
    //ptChargeDrv->AdapterStatus = TRUE;
    if ((TRUE == ptChargeDrv->AdapterStatus) && 
        (FALSE == tDs18b20_Drv.bFlagStart))
    {
        iRet = tDs18b20_Drv.Start();        
        if (0 != iRet)
        {
            log_e("temp start fail");
            return;
        }
        Mod_SoftTimer_Regist(&tTimerTemp, Temp_Handler);
        /* ds18b20 1s后读取温度, 此时该任务(5s一次)还没有执行, 可以不用保护 */
        tDs18b20_Drv.bFlagStart = TRUE;
    }
}

#if 0
/** 
 *  为了达到充电的动态效果，注册了一个timer，每30s电压加5个mv，
 *  充到最大电压4150mv为full，此时电量为100%，电池温度设置为30。
 *  电量是通过电压按一点算法获取。
 */

/* Set ADC sample rate (30sec)*/
#define ADC_SAMPLE_RATE        30

#define OCVREG0                0         // 3.1328
#define OCVREG1                0         // 3.2736
#define OCVREG2                0         // 3.5000
#define OCVREG3                3         // 3.5552
#define OCVREG4                7         // 3.6256
#define OCVREG5                13        // 3.6608
#define OCVREG6                18        // 3.6960
#define OCVREG7                27        // 3.7312
#define OCVREG8                36        // 3.7664
#define OCVREG9                46        // 3.8016
#define OCVREGA                53        // 3.8368
#define OCVREGB                62        // 3.8720
#define OCVREGC                73        // 3.9424
#define OCVREGD                85        // 4.0128
#define OCVREGE                93        // 4.0832
#define OCVREGF                100       // 4.1536

#define OCVVOL0                3132
#define OCVVOL1                3273
#define OCVVOL2                3500
#define OCVVOL3                3555
#define OCVVOL4                3625
#define OCVVOL5                3660
#define OCVVOL6                3696
#define OCVVOL7                3731
#define OCVVOL8                3766
#define OCVVOL9                3801
#define OCVVOLA                3836
#define OCVVOLB                3872
#define OCVVOLC                3942
#define OCVVOLD                4012
#define OCVVOLE                4090//4083
#define OCVVOLF                4150//4150


/** 
 *  根据电压获取当前电量，此算法简单实用，根据不同电池需调节
 *  最大电压，最小电压以及每段的电压及对应的电量，可达到充电快慢的程度，
 *  实际中需要通过内阻，开路电压及充放电电流按一定算法获取后
 *  并经过库仑计校准得到电量，实际电量的计算是很复杂的，往往无法达到理想的检测效果
 */
static uint8_t Battery_Restcap(int ocv)
{
    if(ocv >= OCVVOLF)
    {
        return OCVREGF;
    }
    else if(ocv < OCVVOL0)
    {
        return OCVREG0;
    }
    else if(ocv < OCVVOL1)
    {
        return OCVREG0 + (OCVREG1 - OCVREG0) * (ocv - OCVVOL0) / (OCVVOL1 - OCVVOL0);
    }
    else if(ocv < OCVVOL2)
    {
        return OCVREG1 + (OCVREG2 - OCVREG1) * (ocv - OCVVOL1) / (OCVVOL2 - OCVVOL1);
    }
    else if(ocv < OCVVOL3)
    {
        return OCVREG2 + (OCVREG3 - OCVREG2) * (ocv - OCVVOL2) / (OCVVOL3 - OCVVOL2);
    }
    else if(ocv < OCVVOL4)
    {
        return OCVREG3 + (OCVREG4 - OCVREG3) * (ocv - OCVVOL3) / (OCVVOL4 - OCVVOL3);
    }
    else if(ocv < OCVVOL5)
    {
        return OCVREG4 + (OCVREG5 - OCVREG4) * (ocv - OCVVOL4) / (OCVVOL5 - OCVVOL4);
    }
    else if(ocv < OCVVOL6)                               
    {
        return OCVREG5 + (OCVREG6 - OCVREG5) * (ocv - OCVVOL5) / (OCVVOL6 - OCVVOL5);
    }
    else if(ocv < OCVVOL7)
    {
        return OCVREG6 + (OCVREG7 - OCVREG6) * (ocv - OCVVOL6) / (OCVVOL7 - OCVVOL6);
    }
    else if(ocv < OCVVOL8)
    {
        return OCVREG7 + (OCVREG8 - OCVREG7) * (ocv - OCVVOL7) / (OCVVOL8 - OCVVOL7);
    }
    else if(ocv < OCVVOL9)
    {
        return OCVREG8 + (OCVREG9 - OCVREG8) * (ocv - OCVVOL8) / (OCVVOL9 - OCVVOL8);
    }
    else if(ocv < OCVVOLA)
    {
        return OCVREG9 + (OCVREGA - OCVREG9) * (ocv - OCVVOL9) / (OCVVOLA - OCVVOL9);
    }
    else if(ocv < OCVVOLB)
    {
        return OCVREGA + (OCVREGB - OCVREGA) * (ocv - OCVVOLA) / (OCVVOLB - OCVVOLA);
    }
    else if(ocv < OCVVOLC)
    {
        return OCVREGB + (OCVREGC - OCVREGB) * (ocv - OCVVOLB) / (OCVVOLC - OCVVOLB);
    }
    else if(ocv < OCVVOLD)
    {
        return OCVREGC + (OCVREGD - OCVREGC) * (ocv - OCVVOLC) / (OCVVOLD - OCVVOLC);
    }
    else if(ocv < OCVVOLE)
    {
        return OCVREGD + (OCVREGE - OCVREGD) * (ocv - OCVVOLD) / (OCVVOLE - OCVVOLD);
    }
    else if(ocv < OCVVOLF)
    {
        return OCVREGE + (OCVREGF - OCVREGE) * (ocv - OCVVOLE) / (OCVVOLF - OCVVOLE);
    }
    else
    {
        return 0;
    }
}

static void Get_battery_preference(void)
{
    uint8_t voltage_level = Battery_Restcap(battery_vol);
    uint8_t battery_vol = 5;        //每30s轮询一次，电压加5mv
    uint8_t battery_temp = 30;
    
    if (voltage_level > 100)
    {
        voltage_level = 100;
    }

    if(battery_vol > 4150)
    {
        battery_vol = 4150;
    }
}
#endif


