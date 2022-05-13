/**  
 *   文件名称：core.c
 *   摘要：
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.18   v1      ql     创建文件
 *   --------------------------------------------------
 */
#include <stdio.h>
#include "deftypes.h"
#include "config.h"
#include "host_link.h"
#include "user_keys.h"
#include "charge.h"
#include "common.h"
#include "core.h"


static void Core_Monitor(void)
{
    //log("run\n");
    Drv_Led_Blink();
#if _WDG_ENABLE    
    Drv_Iwdg_Feed();
#endif
}

const PF_Polling pfPolling_Core[] = {
    Mod_EventCore,
    Mod_SoftTimerCore
};


/**
 * \brief  Call this function inside the while(1)
 * \retval Result
 *          \arg 0  OK
 *          \arg -1 ERR
 */
void Core_Polling(void)
{
    int i;
    
    for (i = 0; i < N_ELEMENTS(pfPolling_Core); i++)
    {
        (*pfPolling_Core[i])();
    }
}


/******************************************/
MOD_EVENT_INSTANCE(tEventDoCmd);
MOD_EVENT_INSTANCE(tEventOnAdapter);
MOD_EVENT_INSTANCE(tEventOffAdapter);
MOD_EVENT_INSTANCE(tEventShell);
MOD_TIMER_INSTANCE(tTimerKeys, 20, 1);
MOD_TIMER_INSTANCE(tTimerCharge, 5000, 1);
MOD_TIMER_INSTANCE(tTimerIdle, 500, 1);

void Core_Polling_Init(void)
{
    Mod_Event_Register(&tEventDoCmd, DoCmd_Event_Handler);
    //Mod_Event_Register(&tEventOnAdapter, OnAdapter_Handler);
    //Mod_Event_Register(&tEventOffAdapter, OffAdapter_Handler);
    Mod_SoftTimer_Regist(&tTimerKeys, Keys_Scan_Handler);
    Mod_SoftTimer_Regist(&tTimerCharge, ChargeProtect);
    Mod_SoftTimer_Regist(&tTimerIdle, Core_Monitor);
#if _NR_MICRO_SHELL_ENABLE    
    Mod_Event_Register(&tEventShell, Dbg_Shell_Handler);
#endif
}

