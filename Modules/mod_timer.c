/**  文件名称：mod_timer.c
 *   摘要：使用SysTick计时，单位ms
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.19   v1      ql     创建文件
 *   --------------------------------------------------
 */
#include <stdio.h>
#include "mod_timer.h"


static PT_SoftTimerInstance ptSoftTimerHead = NULL;

static int _SoftTimerDelete(PT_SoftTimerInstance ptTimerInstance)
{
    PT_SoftTimerInstance ptmp = ptSoftTimerHead;
        
    if (ptSoftTimerHead == ptTimerInstance)
    {
        ptSoftTimerHead = ptSoftTimerHead->ptNext;
        return 0;
    }
    
    while (ptmp)
    {
        if (ptmp->ptNext == ptTimerInstance)
        {
            ptmp->ptNext = ptTimerInstance->ptNext;
            return 0;
        }
        ptmp = ptmp->ptNext;
    }
    
    return -1;
}


/* 定时处理处理 */
void Mod_SoftTimerCore(void)
{
    PT_SoftTimerInstance ptmp = ptSoftTimerHead;
    while (ptmp)
    {
        if (Mod_GetTick() - ptmp->u32Tick >= MS2TICKS(ptmp->u32Period))
        {
            ptmp->pfHandler();
            if (ptmp->iRepeat)
            {
                ptmp->u32Tick = Mod_GetTick();
            }
            else
            {
                _SoftTimerDelete(ptmp);
            }
        }
        ptmp = ptmp->ptNext;
    }
}

int Mod_SoftTimer_Regist(PT_SoftTimerInstance ptTimerInstance, PF_TimerHandler pfHandler)
{
    if (ptTimerInstance == NULL || pfHandler == NULL)
    {
        return -1;
    }
    
    if (ptSoftTimerHead == NULL)
    {
        ptSoftTimerHead         = ptTimerInstance;
        ptTimerInstance->ptNext = NULL;
    }
    else
    {
        /* 插入链表头 */
        ptTimerInstance->ptNext = ptSoftTimerHead->ptNext;
        ptSoftTimerHead->ptNext = ptTimerInstance;
    }
    ptTimerInstance->pfHandler = pfHandler;
    ptTimerInstance->u32Tick   = Mod_GetTick();
    
    return 0;
}

int Mod_SoftTimer_Stop(PT_SoftTimerInstance ptTimerInstance)
{
    if (ptTimerInstance == NULL)
    {
        return -1;
    }
    
    _SoftTimerDelete(ptTimerInstance);
    return 0;
}

int Mod_SoftTimer_Reset(PT_SoftTimerInstance ptTimerInstance)
{
    if (ptTimerInstance == NULL)
    {
        return -1;
    }
    
    ptTimerInstance->u32Tick = Mod_GetTick();
    return 0;
}

int Mod_SoftTimer_SetPeriod(PT_SoftTimerInstance ptTimerInstance, uint32_t ms)
{
    if (ptTimerInstance == NULL)
    {
        return -1;
    }
    
    ptTimerInstance->u32Period = ms;
    return 0;
}

