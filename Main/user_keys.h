
#ifndef __USER_KEY_H_
#define __USER_KEY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "flexible_button.h"
#include "deftypes.h"

#define ACTIVE_LOW      0
#define ACTIVE_HIGH     1


typedef void (*PF_KeyHandler)(flex_button_t *ptKey);


/* 每个bit表示一个事件，监控多个事件 */
#define TYPE_TO_BIT(ev)       (0x0001 << ev)


#define BTN_EVENT_DOWN              TYPE_TO_BIT(FLEX_BTN_PRESS_DOWN)
#define BTN_EVENT_CLICK             TYPE_TO_BIT(FLEX_BTN_PRESS_CLICK)
#define BTN_EVENT_DOUBLE_CLICK      TYPE_TO_BIT(FLEX_BTN_PRESS_DOUBLE_CLICK)
#define BTN_EVENT_REPEAT_CLICK      TYPE_TO_BIT(FLEX_BTN_PRESS_REPEAT_CLICK)
#define BTN_EVENT_SHORT             TYPE_TO_BIT(FLEX_BTN_PRESS_SHORT_START)
#define BTN_EVENT_SHORT_UP          TYPE_TO_BIT(FLEX_BTN_PRESS_SHORT_UP)
#define BTN_EVENT_LONG              TYPE_TO_BIT(FLEX_BTN_PRESS_LONG_START)
#define BTN_EVENT_LONG_UP           TYPE_TO_BIT(FLEX_BTN_PRESS_LONG_UP)
#define BTN_EVENT_LONGLONG          TYPE_TO_BIT(FLEX_BTN_PRESS_LONG_HOLD)
#define BTN_EVENT_LONGLONG_UP       TYPE_TO_BIT(FLEX_BTN_PRESS_LONG_HOLD_UP)

/* 抬起事件 */
#define BTN_EVENT_UP                (BTN_EVENT_CLICK | BTN_EVENT_SHORT_UP | BTN_EVENT_LONG_UP | BTN_EVENT_LONGLONG_UP)


typedef struct
{
    U8  GpioId;
    U8  Level;
    U16 Event;
    PF_KeyHandler pfHandler;
} T_KeyInfo, *PT_KeyInfo;


INT Keys_Init(VOID);
VOID Keys_Scan_Handler(VOID);
VOID Keys_Get_Val(U8 *pKeyVal);
VOID Ired_Get_Val(U8 *pIredVal);


#ifdef __cplusplus
}
#endif

#endif  // __USER_KEY_H_

