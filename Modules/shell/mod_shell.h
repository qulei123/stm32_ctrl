
#ifndef __MOD_SHELL_H__
#define __MOD_SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "config.h"

#if _NR_MICRO_SHELL_ENABLE
#include "nr_micro_shell.h"


void Mod_Shell_Init(void);
int Mod_Shell_Parse(uint8_t *pbuf, uint16_t len);

#endif  // _NR_MICRO_SHELL_ENABLE

#ifdef _NR_MICRO_SHELL_ENABLE
#define log_sh(...)   LogOut(3, LOG_PARAM_DEFAULT, __VA_ARGS__)
#else
#define log_sh(...)
#endif


#ifdef __cplusplus
}
#endif

#endif

