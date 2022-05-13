/*  
 * 文件：drv_wdg.c
 * 摘要：看门狗
 *  
 * 修改历史    版本号    Author   修改内容
 *--------------------------------------------------
 * 2022.04.26    v1        ql     创建文件
 *--------------------------------------------------
 */
#include "stm32f1xx_clk.h"
#include "drv_wdg.h"   


static IWDG_HandleTypeDef IwdgHandle;


/* Set counter reload value to obtain 250ms IWDG TimeOut.
   IWDG counter clock Frequency = LsiFreq / 32
   Counter Reload Value = 250ms / IWDG counter clock period
                        = 0.25s / (32/LsiFreq)
                        = LsiFreq / (32 * 4)
                        = LsiFreq / 128 */
static uint32_t _CalcReload(uint32_t timeout, uint32_t Prescaler)
{
    //(timeout / 1000) / (Prescaler / Drv_GetLSIFreq());
    return (Drv_GetLSIFreq() * timeout / Prescaler / 1000);
}


/**
  * @brief  独立看门狗初始化
  * @param  timeout, 超时时间ms
  * @retval 无
  * @note   范围0.8ms ~ 3276.8ms, 超过这个范围需要修改分频系数
  */
int Drv_Iwdg_Init(uint32_t timeout)
{
    IwdgHandle.Instance = IWDG;
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
    IwdgHandle.Init.Reload    = _CalcReload(timeout, 32);
    
    if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
    {
        log_e("iwdg init\n");
        return DRV_ERR;
    }
    
    return DRV_OK;
}


void Drv_Iwdg_Feed(void)
{
    HAL_IWDG_Refresh(&IwdgHandle);
}

