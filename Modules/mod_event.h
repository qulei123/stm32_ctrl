
#ifndef __MOD_EVENT_H__
#define __MOD_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


typedef void (*PF_EventHandler)(void);

typedef struct tEventInfo
{
    int                iTrigger;
    PF_EventHandler    pfHandler;
    struct tEventInfo  *ptNext;
} T_EventInfo, *PT_EventInfo;

typedef T_EventInfo   T_EventInstance;
typedef PT_EventInfo  PT_EventInstance;

#define EVENT(name)                 tEvent##name
#define MOD_EVENT_INSTANCE(name)    T_EventInstance name
#define MOD_EVENT_EXTERN(name)      extern T_EventInstance name

///< pInstance \ref bEVENT_INSTANCE
void Mod_EventCore(void);
int Mod_Event_Register(PT_EventInstance ptInstance, PF_EventHandler pfhandler);
int Mod_Event_Trigger(PT_EventInstance ptInstance);
int Mod_Event_IsIdle(void);



#ifdef __cplusplus
}
#endif

#endif

/************************ Copyright (c) 2019 Bean *****END OF FILE****/
