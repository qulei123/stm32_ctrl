
#ifndef __CORE_H__
#define __CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mod_event.h"
#include "mod_timer.h"
#include "util_log.h"

typedef void (*PF_Polling)(void);


void Core_Polling(void);
void Core_Polling_Init(void);

#ifdef __cplusplus
}
#endif

#endif

