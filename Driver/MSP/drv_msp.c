/*  
 * 文件名称：drv_msp.c
 * 摘要：
 *  
 * 修改历史   版本号   Author  修改内容
 *--------------------------------------------------
 * 2022.03.29   v1      ql     创建文件
 *--------------------------------------------------
 */

#include "drv_msp.h"
#include "stm32f1xx_clk.h"
#include "mod_shell.h"

#include "common.h"


/* 所有gpio定义 */
static T_GPIO_InitInfo tGPIO_Info[] = 
{
    /* uart */
    {ID_DBG_TX,  DEBUG_UART_TX_PORT, DEBUG_UART_TX_PIN,    GPIO_MODE_AF_PP,     GPIO_PULLUP,    GPIO_SPEED_FREQ_HIGH},
    {ID_DBG_RX,  DEBUG_UART_RX_PORT, DEBUG_UART_RX_PIN,    GPIO_MODE_AF_INPUT,  GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    {ID_CMT_TX,  COMMT_UART_PORT,    COMMT_UART_TX_PIN,    GPIO_MODE_AF_PP,     GPIO_PULLUP,    GPIO_SPEED_FREQ_HIGH},
    {ID_CMT_RX,  COMMT_UART_PORT,    COMMT_UART_RX_PIN,    GPIO_MODE_AF_INPUT,  GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    {ID_CT1_TX,  CTRL1_UART_PORT,    CTRL1_UART_TX_PIN,    GPIO_MODE_AF_PP,     GPIO_PULLUP,    GPIO_SPEED_FREQ_HIGH},
    {ID_CT1_RX,  CTRL1_UART_PORT,    CTRL1_UART_RX_PIN,    GPIO_MODE_AF_INPUT,  GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    //{ID_CT2_TX,  CTRL1_UART_PORT,    CTRL1_UART_TX_PIN,    GPIO_MODE_AF_PP,     GPIO_PULLUP,    GPIO_SPEED_FREQ_HIGH},
    //{ID_CT2_RX,  CTRL1_UART_PORT,    CTRL1_UART_RX_PIN,    GPIO_MODE_AF_INPUT,  GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* 舵机零位检测: ZPT0,摇头; ZPT1,点头  */
    {ID_ZPT0,    ZPT0_GPIO_PORT,     ZPT0_GPIO_PIN,        GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    {ID_ZPT1,    ZPT1_GPIO_PORT,     ZPT1_GPIO_PIN,        GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* key: 查询方式 */
    {ID_KEY0,    KEY0_GPIO_PORT,     KEY0_GPIO_PIN,        GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    {ID_KEY1,    KEY1_GPIO_PORT,     KEY1_GPIO_PIN,        GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    {ID_KEY2,    KEY2_GPIO_PORT,     KEY2_GPIO_PIN,        GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    {ID_KEY3,    KEY3_GPIO_PORT,     KEY3_GPIO_PIN,        GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* Ired: 查询方式，会产生有4.5s低脉冲 */
    {ID_IR_IN,   IRED_IN_GPIO_PORT,  IRED_IN_GPIO_PIN,     GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    {ID_IR_EN,   IRED_EN_GPIO_PORT,  IRED_EN_GPIO_PIN,     GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* ds18b20 */
    {ID_DS18B20, DS18B20_DQ_PORT,    DS18B20_DQ_PIN,       GPIO_MODE_OUTPUT_OD, GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* adc */
    {ID_BAT_VOL, BATY_VOLT_ADC_PORT, BATY_VOLT_ADC_PIN,    GPIO_MODE_ANALOG,    GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* chargr:ISL6251A */
    {ID_ACP_IN,  CHARGE_DTCT_PORT,   CHARGE_DTCT_PIN,      GPIO_MODE_INPUT,     GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* 舵机供电控制 */
    {ID_MTO_PWR, MOTOR_PWR_PORT,     MOTOR_PWR_PIN,        GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH},
    /* led */
    {ID_LED0,    LED0_GPIO_PORT,     LED0_GPIO_PIN,        GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,    GPIO_SPEED_FREQ_HIGH}
};

/** 用户中断信息
 *  1. tick使用了最高优先级(0, 0)
 *  2. 默认配置: 16个抢占优先级，0个响应优先级
 */
static T_IntInfo tIntInfo[] =
{
    /*  en            IRQn         PreemptPriority   SubPriority */
    {ENABLE,    COMMT_UART_IRQn,         1,              0},
    {ENABLE,    BATY_VOLT_ADC_IRQ,       2,              0},
    {ENABLE,    DEBUG_UART_IRQn,         10,             0},       /* 调试接口中断优先级最低 */
    {ENABLE,    TIM_LSI_IRQn,            0,              0}        /* timer进行LSI时钟校准，wdg使用 */
};

int Drv_Msp_Init(void)
{
    int iRet = 0;

    HAL_Init();
    SystemClock_Config();   /* 72MHz */
    
    /* uart初始化
     *   debug: uart2 -> 打印调试信息 
     *   commt: uart1 -> 中断接收 host cmd, 并处理
     *   ctrl:  uart3 -> 发送控制 cmd
     *          uart5
     */
    iRet |= Drv_Dbg_Uart_Init(115200);
    iRet |= Drv_Commt_Uart_Init(115200);
    iRet |= Drv_Ctrl1_Uart_Init(115200);
    if(iRet != 0)
    {
        log_e("Uart Init\n");
        return DRV_ERR;
    }
    
    Drv_Baty_Volt_Adc_Init();
    Drv_DWT_Deday_Init();
    Drv_Motor_Power(DRV_ENABLE);

#if LSI_CALIB_ENABLE    
    Drv_UpdateLSIFreq();
#endif
    
#if _NR_MICRO_SHELL_ENABLE
    Mod_Shell_Init();
#endif

#if _WDG_ENABLE
    Drv_Iwdg_Init(WDG_TIMEOUT_MS);
#endif

    return DRV_OK;
}


PT_GPIO_InitInfo Drv_Msp_GpioInfo(void)
{
    return &tGPIO_Info[0];
}

static void _Set_Priority(void)
{
    int i;
    
    for (i = 0; i < N_ELEMENTS(tIntInfo); i++)
    {
        HAL_NVIC_SetPriority((IRQn_Type)tIntInfo[i].IrqNum, tIntInfo[i].PreemptPriority, tIntInfo[i].SubPriority);
        HAL_NVIC_EnableIRQ((IRQn_Type)tIntInfo[i].IrqNum);
    }
}

/* 根据实际情况处理 */
static void _Clock_Enable(void)
{
    /* gpio */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    /* 禁止JTAG, PB3、PB4、PA15作为普通IO */
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
    
    /* uart */
    DEBUG_UART_CLK_EN();
    COMMT_UART_CLK_EN();
    CTRL1_UART_CLK_EN();

    /* enable adc clock, configure ADCx clock prescaler */
    BATY_VOLT_ADC_CLK_EN();
    RCC_PeriphCLKInitTypeDef  PeriphClkInit;
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;        /* 分频因子6, adc时钟为72/6M = 12MHz */
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);    
}


void HAL_MspInit(void)
{
    int i;
    
    _Clock_Enable();
    
    for (i = 0; i < N_ELEMENTS(tGPIO_Info); i++)
    {
        HAL_GPIO_Init(tGPIO_Info[i].GPIOx, &tGPIO_Info[i].GPIO_Init);
    }

    _Set_Priority();
}

