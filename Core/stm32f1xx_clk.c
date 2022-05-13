/**  
 *   文件名称：stm32f1xx_clk.c
 *   摘要：时钟初始化
 *  
 *   修改历史   版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.29   v1        ql     创建文件
 *   2022.04.27   v2        ql     合入LSI频率校准功能
 *   --------------------------------------------------
 */
#include "drv_timer.h"
#include "stm32f1xx_clk.h"
#include "util_log.h"

/* LSI时钟频率大约40kHz(在30kHz和60kHz之间) */
static uint32_t LsiFreq = 40000;
static __IO uint32_t MeasureDone = 0;
static uint16_t tmpCC4[2] = {0, 0};

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{    
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType    = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState          = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue    = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState          = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState      = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource     = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL        = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while(1);
    }

    /** Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType         = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                         |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource      = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider     = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider    = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider    = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        while(1);
    }
}

/* Get the LSI frequency: default used 40k ###*/
uint32_t Drv_GetLSIFreq(void)
{
    return LsiFreq;
}

/* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
static uint32_t _Calc_LSI_Freq(uint32_t PeriodValue)
{
    uint32_t pclk1 = 0, latency = 0;
    RCC_ClkInitTypeDef clkinit =  {0};
    
    /* Get PCLK1 frequency */
    pclk1 = HAL_RCC_GetPCLK1Freq();
    HAL_RCC_GetClockConfig(&clkinit, &latency);

    /* Get PCLK1 prescaler */
    if ((clkinit.APB1CLKDivider) == RCC_HCLK_DIV1)
    {
        /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
        return ((pclk1 / PeriodValue) * TIM_LSI_ICPSC);         /* 8次捕获一次, timer初始化时配置 */
    }
    else
    {
        /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
        return (((2 * pclk1) / PeriodValue) * TIM_LSI_ICPSC);
    }
}

uint32_t Drv_UpdateLSIFreq(void)
{
    RCC_OscInitTypeDef oscinit = {0};
    uint32_t PeriodValue;

    /* Enable LSI Oscillator */
    oscinit.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    oscinit.LSIState       = RCC_LSI_ON;
    oscinit.PLL.PLLState   = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&oscinit) != HAL_OK)
    {
        log_w("Lsi init fail\n");
        return DRV_ERR;
    }

    /* TIMx configuration: Input Capture mode */
    if (Drv_Timer_Lsi_Init() != DRV_OK)
    {
        log_w("timer Lsi init fail\n");
        return DRV_ERR;
    }
    
    /* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in
       stm32f1xx_it.c file) */
    while (MeasureDone == 0);
    MeasureDone = 0;
    
    /* Compute the period length, 同时考虑了溢出的情况 */
    PeriodValue = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);

    /* Deinitialize the TIMER */
    Drv_Timer_Lsi_Deinit();
    
    LsiFreq = _Calc_LSI_Freq(PeriodValue);
    log("LsiFreq = %d\n", LsiFreq);
    
    return DRV_OK;
}

/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim : TIM IC handle
  * @retval None
  * @note   回调函数与其实现的功能相关, 建议放入功能模块中
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    static int CapNum = 0;
    
    /* Get the Input Capture value */
    tmpCC4[CapNum++] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    if (CapNum >= 2)
    {
        MeasureDone = 1;
        CapNum = 0;
    }
}

