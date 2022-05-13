#ifndef __UTIL_LOG_H__
#define __UTIL_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include "config.h"
#include "drv_uart.h"

//<h> Utils Configuration
//<o> Debug level
//<0=> off
//<1=> all
//<2=> warn+error
//<3=> error
//#define _DEBUG_ENABLE 1


#define LogPuts(str)   Drv_Dbg_Puts(str, strlen(str))
#define LogPut(char)   Drv_Dbg_Puts(&char, 1)


#define LOG_PARAM_NULL 0X0
#define LOG_PARAM_LINE 0X1
#define LOG_PARAM_FUNC 0X2
#define LOG_PARAM_FILE 0X4

#define LOG_BUF_SIZE 256

#define LOG_I_PARAM (LOG_PARAM_NULL)
#define LOG_W_PARAM (LOG_PARAM_FUNC | LOG_PARAM_LINE)
#define LOG_E_PARAM (LOG_PARAM_FILE | LOG_PARAM_FUNC | LOG_PARAM_LINE)

#define LOG_PARAM_DEFAULT __FILE__, __func__, __LINE__

#if (_DEBUG_ENABLE > 0)
#if (_DEBUG_ENABLE <= 1)
#define log_i(...)      LogOut(0, LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log(...)        LogOut(3, LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_hex         LogHexs
#else
#define log_i(...)
#define log(...)
#define log_hex(...)
#endif

#if (_DEBUG_ENABLE <= 2)
#define log_w(...) LogOut(1, LOG_PARAM_DEFAULT, __VA_ARGS__)
#else
#define log_w(...)
#endif

#if (_DEBUG_ENABLE <= 3)
#define log_e(...) LogOut(2, LOG_PARAM_DEFAULT, __VA_ARGS__)
#else
#define log_e(...)
#endif
#else
#define log_i(...)
#define log_w(...)
#define log_e(...)
#define log(...)
#define log_hex(...)

#endif



void LogOut(uint8_t type, const char *ptr_file, const char *ptr_func, uint32_t line,
             const char *fmt, ...);
void LogHexs(uint8_t *pbuf, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif  // __UTIL_LOG_H__

