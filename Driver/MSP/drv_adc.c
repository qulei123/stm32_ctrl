/*  
 * 文件名称：drv_adc.h
 * 摘要：
 *  
 * 修改历史        版本号     Author  修改内容
 *--------------------------------------------------
 * 2022.03.09   v1      ql     创建文件
 *--------------------------------------------------
 */
#include <string.h>
#include "nr_micro_shell.h"
#include "util_log.h"
#include "drv_adc.h"
#include "algo_filter.h"

/*
 * 定义全局变量
 */
#define ADC_LEN 20
static ADC_HandleTypeDef tAdcHdl;
static uint16_t u16AdcVal[ADC_LEN];


void Drv_Baty_Volt_Adc_Init(void)
{
    ADC_HandleTypeDef *ptAdc = &tAdcHdl;
    ADC_ChannelConfTypeDef stCfg;
      
    ptAdc->Instance = BATY_VOLT_ADC;
    
    ptAdc->Init.DataAlign             = ADC_DATAALIGN_RIGHT;             
    ptAdc->Init.ScanConvMode          = ADC_SCAN_DISABLE;        /* 单通道 */
    ptAdc->Init.ContinuousConvMode    = ENABLE;                  /* 连续转化 */
    ptAdc->Init.NbrOfConversion       = 1;                       /* 1个转换在规则序列中 */
    ptAdc->Init.DiscontinuousConvMode = DISABLE;             
    ptAdc->Init.NbrOfDiscConversion   = 0;                     
    ptAdc->Init.ExternalTrigConv      = ADC_SOFTWARE_START;       
    HAL_ADC_Init(ptAdc);                                  

    stCfg.Channel      = BATY_VOLT_ADC_CHL;
    stCfg.Rank         = ADC_REGULAR_RANK_1;
    stCfg.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ;
    HAL_ADC_ConfigChannel(ptAdc, &stCfg);

    HAL_ADC_Start_IT(ptAdc);
}

void BATY_VOLT_ADC_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&tAdcHdl);
    //log("adc, volt\n");   
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    static int i = 0;
    u16AdcVal[i] = HAL_ADC_GetValue(hadc);
    i = (i + 1) % ADC_LEN;
}

/* adc -> volt -> Bat volt
 *   u16batyVolt = (u16mVolt + deta) * factor
 */
uint16_t Drv_Obtain_Baty_Volt(void)
{
    uint16_t u16AverVal, u16mVolt, u16batymVolt;
    u16AverVal = Filter_DeExtremeAver_U16(u16AdcVal, ADC_LEN);
    u16mVolt = CALC_12BITS_TO_VOLT(u16AverVal);
    u16batymVolt = (u16mVolt + BAT_DETA) * BAT_FACTOR;
    //log("adc %d, volt %d, batv %d\n", u16AdcVal, u16mVolt, u16batymVolt);

    return u16batymVolt;
}

/* shell 调试接口 */
static void Help_Baty_Volt(void)
{
    sh_printf("baty volt get\n");
}

void Shell_Baty_Cmd(char argc, char *argv)
{
    char *func, *opt;
    
    if (argc < 3)
    {
        Help_Baty_Volt();
        return;
    }

    func = &argv[argv[1]];
    opt  = &argv[argv[2]];

    if ((!strcmp("volt", func)) &&
        (!strcmp("get", opt)))
    {
        uint16_t u16batymVolt;
        u16batymVolt = Drv_Obtain_Baty_Volt();
        sh_printf("baty=%dmV\n", u16batymVolt);
    }
    else
    {
        Help_Baty_Volt();
    }
}


#if 0
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef          GPIO_InitStruct;
    static DMA_HandleTypeDef  DmaHandle;
    RCC_PeriphCLKInitTypeDef  PeriphClkInit;
    
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    //BATY_VOLT_ADC_GPIO_CLK_ENABLE(); 
    //BATY_VOLT_ADC_CLK_ENABLE();

    /* Configure ADCx clock prescaler */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;        /* 分频因子6, adc时钟为72/6M = 12MHz */
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
    

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* Configure GPIO pin of the selected ADC channel */
    GPIO_InitStruct.Pin  = BATY_VOLT_ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BATY_VOLT_ADC_PORT, &GPIO_InitStruct);
#if 0
    /*##-3- Configure the DMA ##################################################*/
    /* Enable clock of DMA associated to the peripheral */
    ADCx_DMA_CLK_ENABLE();

    /* Configure DMA parameters */
    DmaHandle.Instance = ADCx_DMA;

    DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Transfer from ADC by half-word to match with ADC configuration: ADC resolution 10 or 12 bits */
    DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;   /* Transfer to memory by half-word to match with buffer variable type: half-word */
    DmaHandle.Init.Mode                = DMA_CIRCULAR;              /* DMA in circular mode to match with ADC configuration: DMA continuous requests */
    DmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Deinitialize  & Initialize the DMA for new transfer */
    HAL_DMA_DeInit(&DmaHandle);
    HAL_DMA_Init(&DmaHandle);

    /* Associate the initialized DMA handle to the ADC handle */
    __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

    /*##-4- Configure the NVIC #################################################*/

    /* NVIC configuration for DMA interrupt (transfer completion or error) */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(ADCx_DMA_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(ADCx_DMA_IRQn);
#endif

    /* NVIC configuration for ADC interrupt */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(BATY_VOLT_ADC_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(BATY_VOLT_ADC_IRQ);
}
#endif

