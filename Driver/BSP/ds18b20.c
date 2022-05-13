/*  
 * 文件名称：ds18b20.c
 * 摘要：
 *  
 * 修改历史   版本号   Author  修改内容
 *--------------------------------------------------
 * 2022.02.10   v1      ql     创建文件
 *--------------------------------------------------
 */
#include <string.h>
#include "nr_micro_shell.h"
#include "common.h"
#include "util_ow_bitops.h"
#include "ds18b20.h"

static T_OneWire_Ops tOneWire;

/* 硬件操作
 * 注意：
 * 1. 使用hal配置gpio口时, 耗时13us左右(72M), 对读时序影响比较大。
 *    -- 建议使用直接操作寄存器 
 * 2. stm32, IO输出在开漏模式时, 对输入数据寄存器的读访问可得到IO状态
 *    -- 建议使用该方式, 不用频繁切换IO方向, stm32F1测试正常
 *    -- stm32F1测试, 使用推挽模式也正常
 * 3. 中断会影响onewire的时序
 */
#if 0  
static void Ds18b20_Pin_Config(int iMode)
{
    GPIO_InitTypeDef tGPIO_Init = {0};

    //DS18B20_DQ_CLK_EN();
    
    tGPIO_Init.Pin = DS18B20_DQ_PIN;
    if (SDA_MODE_OUTPUT == iMode)
    {
        tGPIO_Init.Mode  = GPIO_MODE_OUTPUT_OD;         // 开漏输出
        tGPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    }
    else
    {
        tGPIO_Init.Mode  = GPIO_MODE_INPUT;
        tGPIO_Init.Pull  = GPIO_PULLUP;
    }
    HAL_GPIO_Init(DS18B20_DQ_PORT, &tGPIO_Init);
}
#endif

static void Ds18b20_Pin_Set(int iStatus)
{
    HAL_GPIO_WritePin(DS18B20_DQ_PORT, DS18B20_DQ_PIN, (GPIO_PinState)!!iStatus);
}

static int Ds18b20_Pin_Get(void)
{
    return HAL_GPIO_ReadPin(DS18B20_DQ_PORT, DS18B20_DQ_PIN);
}

extern void bHalDelayUs(uint32_t xus);

void Ds18b20_Init(void)
{
    PT_OneWire_Ops ptOneWire = &tOneWire;
    
    /* gpio初始化统一在msp中处理 */
    //Ds18b20_Pin_Config(SDA_MODE_OUTPUT);
    //ptOneWire->Sda_Cfg = Ds18b20_Pin_Config;
    ptOneWire->Sda_Set = Ds18b20_Pin_Set;
    ptOneWire->Sda_Get = Ds18b20_Pin_Get;
    ptOneWire->DelayUs = Drv_usDeday;
    
    return;
}

static int Ds18b20_Start(void)
{
    PT_OneWire_Ops ptOneWire = &tOneWire;

    if (0 == Ow_Reset(ptOneWire))
    {
        Ow_Write_Byte(ptOneWire, 0xCC);          /* 跳过ROM */
        Ow_Write_Byte(ptOneWire, 0x44);          /* 发送温度转换指令 */
        return 0;
    }
    
    return 1;
}

/**
 * @brief : 读取温度
 * @param : none
 * @ret   : 温度值，浮点型
 */
int Ds18b20_Read_Temp(float *pfTemp)
{
    PT_OneWire_Ops ptOneWire = &tOneWire;
    uint8_t u8TempL, u8TempH;
    int16_t s16Temp;
    int iRet;
    
#if 0
    iRet = Ds18b20_Start();
    if (0 != iRet)
    {
        log_e("Ds18b20 start fail");
        return 1;
    }

    HAL_Delay(750);          
#endif

    iRet = Ow_Reset(ptOneWire);
    if (0 != iRet)
    {
        log_e("Ow Reset fail");
        return 1;
    }
    
    Ow_Write_Byte(ptOneWire, 0xCC);          /* 跳过ROM */
    Ow_Write_Byte(ptOneWire, 0xBE);          /* 读温度暂存命令 */
    u8TempL = Ow_Read_Byte(ptOneWire);
    u8TempH = Ow_Read_Byte(ptOneWire);
    s16Temp = (u8TempH << 8) | u8TempL;
    if (s16Temp < 0)
    {
        /* 负温度 */
        *pfTemp = (~s16Temp + 1) * 0.0625;
    }
    else
    {
        *pfTemp = s16Temp * 0.0625;    
    }

    return 0;
}
 
/**
 * @brief : 读唯一序列号(只有一个设备)
 * @param : rom 返回序列号缓存
 * @ret   : none
 */ 
int Ds18b20_Read_Rom(char *pRom)
{
    PT_OneWire_Ops ptOneWire = &tOneWire;
    int i, iRet;

    iRet = Ow_Reset(ptOneWire);
    if (0 != iRet)
    {
        return 1;
    }

    Ow_Write_Byte(ptOneWire, 0x33);
    for (i = 0; i < 8; i++)
    {
        pRom[i] = Ow_Read_Byte(ptOneWire);
    }

    return 0;
}

/* shell 调试接口 */
static void Help_Temp(void)
{
    sh_printf("temp get\n");
}

void Shell_Temp_Cmd(char argc, char *argv)
{
    char *opt;
    
    if (argc < 2)
    {
        Help_Temp();
        return;
    }

    opt = &argv[argv[1]];

    if (!strcmp("get", opt))
    {
        float ftemp;
        
        Ds18b20_Start();
        Ds18b20_Read_Temp(&ftemp);
        sh_printf("temp=%0.2f\n", ftemp);
    }
    else
    {
        Help_Temp();
    }
}


T_DrvTempFunc tDs18b20_Drv = 
{
    Ds18b20_Init,
    Ds18b20_Start, 
    Ds18b20_Read_Temp,
    FALSE,
};

