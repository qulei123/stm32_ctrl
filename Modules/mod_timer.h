
#ifndef __MOD_TIMER_H__
#define __MOD_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define Mod_GetTick     HAL_GetTick
#define TICK_FRQ_HZ     (1000 / HAL_TICK_FREQ_DEFAULT)
#define MS2TICKS(ms)    (ms / (1000 / TICK_FRQ_HZ))


typedef void (*PF_TimerHandler)(void);

typedef struct tSoftTimer
{
    int                 iRepeat;
    uint32_t            u32Tick;
    uint32_t            u32Period;
    PF_TimerHandler     pfHandler;
    struct tSoftTimer   *ptNext;
} T_SoftTimer, *PT_SoftTimer;

typedef T_SoftTimer T_SoftTimerInstance;
typedef PT_SoftTimer PT_SoftTimerInstance;



#define MOD_TIMER_INSTANCE(name, _period, _repeat) \
    T_SoftTimerInstance name = {.u32Period = _period, .iRepeat = _repeat}


///< pTimerInstance \ref bTIMER_INSTANCE
void Mod_SoftTimerCore(void);
int Mod_SoftTimer_Regist(PT_SoftTimerInstance ptTimerInstance, PF_TimerHandler pfHandler);
int Mod_SoftTimer_Stop(PT_SoftTimerInstance ptTimerInstance);
int Mod_SoftTimer_Reset(PT_SoftTimerInstance ptTimerInstance);
int Mod_SoftTimer_SetPeriod(PT_SoftTimerInstance ptTimerInstance, uint32_t ms);


#ifdef __cplusplus
}
#endif

#endif

