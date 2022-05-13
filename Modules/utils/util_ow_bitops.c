/*  Copyright (s) 2022 西安天和
 *  All rights reserved
 * 
 * 文件名称：util_ow_bitops.c
 * 摘要：单总线协议时序
 *  
 * 修改历史        版本号     Author  修改内容
 *--------------------------------------------------
 * 2022.03.22   v1      ql     创建文件
 *--------------------------------------------------
 */
#include "util_log.h"
#include "util_ow_bitops.h"

#include "ds18b20.h"

static int _Ow_Check(PT_OneWire_Ops ptOneWire)
{   
    int iCnt = 0;
    
    /* bus上检测低电平 */
    //ptOneWire->Sda_Cfg(SDA_MODE_INPUT);
    while (ptOneWire->Sda_Get() && (iCnt < 220))
    {
        iCnt++;
        ptOneWire->DelayUs(1);
    };
    
    if (iCnt >= 220)
    {
        /* 没有收到低电平应答信号 */
        return 1;
    }

    iCnt = 0;
    /* bus上检测高电平 */
    /* 高脉冲到来, 且存在的时间不能超过240us */
    while ((!ptOneWire->Sda_Get()) && (iCnt < 240))
    {
        iCnt++;
        ptOneWire->DelayUs(1);
    };
    
    if(iCnt >= 240)
    {
        /* 没有收到高电平信号 */
        return 1;
    }
    
    return 0;
}

/**
 * @brief: 单总线复位时序
 * 1. 480us-960us的低电平脉冲
 * 2. 释放总线为高电平,等待15~60us之后
 * 3. 60~240us内等待低电平应答
 */
int Ow_Reset(PT_OneWire_Ops ptOneWire)
{
    //ptOneWire->Sda_Cfg(SDA_MODE_OUTPUT);
    ptOneWire->Sda_Set(0);
    ptOneWire->DelayUs(480);
    ptOneWire->Sda_Set(1);
    ptOneWire->DelayUs(20);

    return _Ow_Check(ptOneWire);
}


/** 
 * @brief:  单总线写一位数据
 * 1. 写周期至少60us, 主机把总线拉低1us表示写周期开始
 * 2. 写0：继续拉低至少60us直至写周期结束, 释放总线为高电平
 * 3. 写1：先拉低至少1us, 释放总线为高电平, 直至写周期结束
 * 4. 从机检测到总线被拉低后等待15us，在15us到60us开始对总线采样。
 */
static void _Ow_Write_Bit(PT_OneWire_Ops ptOneWire, int iBit)
{
    //ptOneWire->Sda_Cfg(SDA_MODE_OUTPUT);
    ptOneWire->Sda_Set(0);                  /* 拉低数据线, 开始写周期 */
    ptOneWire->DelayUs(1);                  /* 保持1us, 小于15us */
    ptOneWire->Sda_Set(!!iBit);             /* 保持15~60us高/低电平 -> 等待从机采样*/
    ptOneWire->DelayUs(30);                 // 取30
    ptOneWire->Sda_Set(1);                  /* 拉高数据线, 准备开始下一个写周期 */
    ptOneWire->DelayUs(1);
}

void Ow_Write_Byte(PT_OneWire_Ops ptOneWire, uint8_t u8Byte)
{
    int i;
    
    for (i = 0; i < 8; i++)
    { 
        /* 按从低位到高位发送 */
        _Ow_Write_Bit(ptOneWire, u8Byte & 0x01);
        u8Byte = u8Byte >> 1;
    }    
}

/**
 * @brief: 单总线读取一位数据
 * 1. 主机把总线拉低至少1us后，释放总线为高电平，让从机把数据传输到总线上
 * 2. 送机送出0/1，把总线拉为低/高电平
 * 3. 主机必须在15us内完成对总线采样。
 */
static int _Ow_Read_Bit(PT_OneWire_Ops ptOneWire)
{
    int iBit;

    //ptOneWire->Sda_Cfg(SDA_MODE_OUTPUT);
    ptOneWire->Sda_Set(0);                      /* 拉低数据线, 启动读周期 */  
    ptOneWire->DelayUs(1);                      /* 保持1us, 释放总线为高电平 */
    ptOneWire->Sda_Set(1);
    //ptOneWire->Sda_Cfg(SDA_MODE_INPUT);       /* 设置成输入, 释放总线, 由外部上拉电阻将总线拉高 */
    ptOneWire->DelayUs(8);                      /* 15us内完成采样高/低电平,  */
    iBit = ptOneWire->Sda_Get();                /* 尽量在15us处读取, 测试配置为8稳定, 具体要根据延时函数调整 */
    ptOneWire->DelayUs(30);                     /* 再等待45us读周期结束 */
    
    return iBit;
}


uint8_t Ow_Read_Byte(PT_OneWire_Ops ptOneWire)
{
    int i, iBit;
    uint8_t u8Byte = 0;
    
    for (i = 0; i < 8; i++) 
    {
        /* 先读出的是最低位 */
        iBit = _Ow_Read_Bit(ptOneWire);
        u8Byte |= (iBit << i);
    }
    
    return u8Byte;
}

