/*  
 * 文件：drv_timer.c
 * 摘要：定时器
 *  
 * 修改历史    版本号    Author   修改内容
 *--------------------------------------------------
 * 2022.04.27    v1        ql     创建文件
 *--------------------------------------------------
 */

#include "drv_timer.h"   

static TIM_HandleTypeDef  TimInputCaptureHandle;


int Drv_Timer_Lsi_Init(void)
{
    TIM_IC_InitTypeDef timinputconfig = {0};

    TimInputCaptureHandle.Instance = TIM_LSI;

    /* TIMx configuration: Input Capture mode ---------------------
    The LSI clock is connected to TIM5 CH4.
    The Rising edge is used as active edge.
    The TIM5 CCR4 is used to compute the frequency value.
    ------------------------------------------------------------ */
    TimInputCaptureHandle.Init.Prescaler         = 0;
    TimInputCaptureHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimInputCaptureHandle.Init.Period            = 0xFFFF;
    TimInputCaptureHandle.Init.ClockDivision     = 0;
    TimInputCaptureHandle.Init.RepetitionCounter = 0;
    TimInputCaptureHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_IC_Init(&TimInputCaptureHandle) != HAL_OK)
    {
        return DRV_ERR;
    }

    /* Connect internally the  TIM5 CH4 Input Capture to the LSI clock output */
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_TIM5CH4_ENABLE();

    /* Configure the Input Capture of channel 4 */
    timinputconfig.ICPolarity  = TIM_ICPOLARITY_RISING;
    timinputconfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
    timinputconfig.ICPrescaler = TIM_ICPSC_DIV8;            /* 8个事件捕获一次 */
    timinputconfig.ICFilter    = 0;

    if (HAL_TIM_IC_ConfigChannel(&TimInputCaptureHandle, &timinputconfig, TIM_CHANNEL_4) != HAL_OK)
    {
        return DRV_ERR;
    }

    /* Reset the flags */
    TimInputCaptureHandle.Instance->SR = 0;

    /* Start the TIM Input Capture measurement in interrupt mode */
    if (HAL_TIM_IC_Start_IT(&TimInputCaptureHandle, TIM_CHANNEL_4) != HAL_OK)
    {
        return DRV_ERR;
    }

    return DRV_OK;
}


void Drv_Timer_Lsi_Deinit(void)
{
    HAL_TIM_IC_DeInit(&TimInputCaptureHandle);
}


/* 不是程序的主要功能, 没有放到drv_msp.c 中初始化 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    TIM_LSI_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM_LSI_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM_LSI_IRQn);
}

/**
  * @brief  This function handles TIM5 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM_LSI_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimInputCaptureHandle);
}

