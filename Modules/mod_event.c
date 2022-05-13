/**  Copyright (s) 2022 西安天和
 *   All rights reserved
 * 
 *   文件名称：mod_event.c
 *   摘要：
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.18   v1      ql     创建文件
 *   --------------------------------------------------
 */
#include <stdio.h>
#include "mod_event.h"


static PT_EventInstance ptEventInstanceHead = NULL;


/* 事件处理 */
void Mod_EventCore(void)
{
    PT_EventInfo ptmp = ptEventInstanceHead;
    while (ptmp)
    {
        if (ptmp->iTrigger)
        {
            ptmp->iTrigger = 0;
            ptmp->pfHandler();
        }
        ptmp = ptmp->ptNext;
    }
}


/**
 * \brief Regist event instance
 * \param pInstance Pointer to the event instance
 * \param handler The Callback function
 * \retval Result
 *          \arg 0  OK
 *          \arg -1 ERR
 */
int Mod_Event_Register(PT_EventInstance ptInstance, PF_EventHandler pfhandler)
{
    if (pfhandler == NULL || ptInstance == NULL)
    {
        return -1;
    }
    
    if (ptEventInstanceHead == NULL)
    {
        ptEventInstanceHead = ptInstance;
        ptInstance->ptNext = NULL;
    }
    else
    {
        /* 插入表头 */        
        ptInstance->ptNext          = ptEventInstanceHead->ptNext;
        ptEventInstanceHead->ptNext = ptInstance;
    }
    ptInstance->iTrigger  = 0;
    ptInstance->pfHandler = pfhandler;
    
    return 0;
}

/**
 * \brief Call this function after the event is generated
 * \param pInstance Pointer to the event instance
 * \retval Result
 *          \arg 0  OK
 *          \arg -1 ERR
 */
int Mod_Event_Trigger(PT_EventInstance ptInstance)
{
    if (ptInstance == NULL)
    {
        return -1;
    }
    ptInstance->iTrigger = 1;
    
    return 0;
}

int Mod_Event_IsIdle()
{
    PT_EventInfo ptmp = ptEventInstanceHead;
    while (ptmp)
    {
        if (ptmp->iTrigger)
        {
            return -1;
        }
        ptmp = ptmp->ptNext;
    }
    
    return 0;
}


