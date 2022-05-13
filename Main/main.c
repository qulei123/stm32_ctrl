/* 
 * 文件名称：main.c
 * 摘要：
 *  
 * 修改历史        版本号     Author  修改内容
 *--------------------------------------------------
 * 2022.01.13   v1      ql     创建文件
 * 2022.04.12   v2      ql     化简主程序流程
 *--------------------------------------------------
 */
#include "drv_msp.h"
#include "core.h"
#include "user_keys.h"
#include "ds18b20.h"

int main(void)
{    
    Drv_Msp_Init();
    Keys_Init();
    tDs18b20_Drv.Init();
    Core_Polling_Init();
      
    while(1)
    {
        Core_Polling();
    }    
}

