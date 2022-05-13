/* 
 * 文件：drv_delay.c
 * 摘要：使用内核寄存器精确延时
 *  
 * 修改历史    版本号     Author  修改内容
 *--------------------------------------------------
 * 2022.02.24    v1        ql     创建文件
 * 2022.04.29    v2        ql     优化微秒级的延时函数
 *--------------------------------------------------
 */
#include <stdio.h> 
#include "drv_delay.h"   

/*
 *  在Cortex-M里面有一个外设叫DWT(Data Watchpoint and Trace)，
 *  该外设有一个32位的寄存器叫CYCCNT，它是一个向上的计数器，
 *  记录的是内核时钟运行的个数，最长能记录的时间为：
 *  60s=2的32次方/72000000
 *  (假设内核频率为72M，内核跳一次的时间大概为1/72M=13.8ns)
 *  当CYCCNT溢出之后，会清0重新开始向上计数。
 *  使能CYCCNT计数的操作步骤：
 *  1. 先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 *  2. 使能CYCCNT寄存器之前，先清0
 *  3. 使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
 */
 
#if USE_DWT_DELAY


#define  DWT_CR      *(__IO uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC

#define  CYCCNT_MAX         (0xFFFFFFFF)                     
#define  DEM_CR_TRCENA      (1 << 24)
#define  DWT_CR_CYCCNTENA   (1 <<  0)


/**
  * @brief  初始化DWT
  * @param  无
  * @retval 无
  * @note   使用延时函数前，必须调用本函数
  */
void Drv_DWT_Deday_Init(void)
{
    DEM_CR     |= (uint32_t)DEM_CR_TRCENA;          /* 使能DWT外设 */      
    DWT_CYCCNT  = (uint32_t)0u;                     /* DWT CYCCNT寄存器计数清0 */
    DWT_CR     |= (uint32_t)DWT_CR_CYCCNTENA;           /* 使能Cortex-M DWT CYCCNT寄存器 */
}

/**
  * @brief  读取当前系统时钟计数
  * @param  无
  * @retval 即DWT_CYCCNT寄存器的值
  */
#define Get_CYC_CNT()  DWT_CYCCNT

/**
  * @brief  采用CPU的内部计数实现精确延时，32位计数器
  * @param  us : 延迟长度，单位1 us
  * @retval 无
  * @note   使用本函数前必须先调用CPU_TS_TmrInit函数使能计数器，
            或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
            最大延时值为8秒，即8*1000*1000
  */
void Drv_usDeday(uint32_t usDelay)
{
    /*
     * 1us -> 跳出if -> 0.9us
     * 2us -> 跳出if(wait) -> 2.1us
     * > 3us -> (usDelay - 2) -> 3.2us
     */
    if (usDelay > 1)
    {
        uint32_t clkstart = DWT_CYCCNT;
        uint32_t wait = (usDelay - 2) * SysClkFreqPerUs;
        if (wait > 0)
        {
            //while ((DWT_CYCCNT - clkstart) < wait)
            while ((uint32_t)(CYCCNT_MAX - clkstart + DWT_CYCCNT) < wait)
            {
            }
        }
    }
}

#endif

#if 0

/* 使用延时精确延时1us比较困难 */
static float bUsDelayParam = 1.0;

void bHalDelayUs(uint32_t xus)
{
    volatile uint32_t delay = xus * bUsDelayParam;
    while (delay--);
}

static void _bHalUpdateDelayParam()
{
    volatile uint32_t delay  = 1000000;
    volatile uint32_t tick_s = 0, tick_e = 0;
    
    tick_s = HAL_GetTick();
    bHalDelayUs(delay);
    tick_e = HAL_GetTick();
    
    bUsDelayParam = ((1000.0 / (tick_e - tick_s)));
    log("bUsDelayParam = %f\n", bUsDelayParam);
}

#endif



/*********************************************END OF FILE**********************/
