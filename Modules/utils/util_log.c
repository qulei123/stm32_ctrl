/**  Copyright (s) 2022 西安天和
 *   All rights reserved
 * 
 *   文件名称：util_log.c
 *   摘要：使用SysTick计时，单位ms
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.21   v1      ql     创建文件
 *   --------------------------------------------------
 */

#include "util_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#define LOG_BUF_MIN 64

#if (LOG_BUF_SIZE < LOG_BUF_MIN)
#error "error: LOG_BUF_SIZE < 64"
#endif


static uint8_t       gLogBuf[LOG_BUF_SIZE];
static const uint8_t gLogParamTable[3] = {LOG_I_PARAM, LOG_W_PARAM, LOG_E_PARAM};
static const char    gLogPrefix[3]     = {'I', 'W', 'E'};


/**
 * \brief Create and output string
 * \param type log_i log_w log_e log >> 0, 1, 2, 3
 * \param ptr_file file name
 * \param ptr_func function name
 * \param line line number
 * \retval none
 */
void LogOut(uint8_t type, const char *ptr_file, const char *ptr_func, uint32_t line,
             const char *fmt, ...)
{
    uint32_t buf_len  = 0;
    uint32_t param    = 0;
    uint8_t  name_len = 0;
    int      str_len  = 0;
    char *   p_tmp = NULL, *pbuf = (char *)gLogBuf;
    char     line_number[8];
    va_list  ap;

    if (type >= 4)
    {
        return;
    }

    memset(pbuf, 0, LOG_BUF_SIZE);
    param = gLogParamTable[type];

    if (type < 3)
    {
        pbuf[buf_len++] = gLogPrefix[type];
        pbuf[buf_len++] = ':';
        if (param & LOG_PARAM_FILE)
        {
            name_len = strlen(ptr_file);
            p_tmp    = (char *)ptr_file;
            if (name_len > 10)
            {
                /* 截取文件名后10个字符 */
                p_tmp    = (char *)(ptr_file + name_len - 10);
                name_len = 10;
            }
            memcpy(pbuf + buf_len, p_tmp, name_len);
            buf_len += name_len;
            pbuf[buf_len++] = ' ';
        }
        if (param & LOG_PARAM_FUNC)
        {
            name_len = strlen(ptr_func);
            p_tmp    = (char *)ptr_func;
            memcpy(pbuf + buf_len, p_tmp, name_len);
            buf_len += name_len;
            pbuf[buf_len++] = ' ';
        }
        if (param & LOG_PARAM_LINE)
        {
            str_len = sprintf(line_number, "%d", line);
            if (str_len < 0)
            {
                return;
            }
            memcpy(pbuf + buf_len, line_number, str_len);
            buf_len += str_len;
            pbuf[buf_len++] = ' ';
        }
    }
    va_start(ap, fmt);
    str_len = vsnprintf(pbuf + buf_len, LOG_BUF_SIZE - buf_len - 1, fmt, ap);
    va_end(ap);
    if (str_len < 0)
    {
        return;
    }
    
    LogPuts(pbuf);
}


static void LogPutHex (uint8_t hexnum)
{
    uint8_t nibble, i;
    char ch;
    
    LogPuts("0x");
    i = 1;
    do
    {
        nibble = (hexnum >> (4*i)) & 0x0F;
        ch = (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble);
        LogPut(ch);
    } 
    while (i--);
}


void LogHexs(uint8_t *pbuf, uint32_t len) 
{                                       
    uint32_t i;

    for(i = 0; i < len; i++)
    {
        LogPutHex(pbuf[i]); 
        LogPuts(" ");
    }
    LogPuts("\n");
}


