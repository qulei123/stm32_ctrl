/* 
 * 文件名称：host_link.h
 * 摘要：
 *  
 * 修改历史        版本号     Author  修改内容
 *--------------------------------------------------
 * 2022.01.13   v1      ql     创建文件
 *--------------------------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include "nr_micro_shell.h"
#include "common.h"
#include "drv_msp.h"
#include "motor.h"
#include "user_keys.h"
#include "host_link.h"

static U8 SendBuf[CMT_BUF_SIZE]; 
static BOOL bFactoryMode;

static PT_CMT_FRAME Format_Cmd(U8 u8CmdId, U8 *pData, U16 u16DataLen)
{
    U8 *pBuf = SendBuf;
    PT_CMT_FRAME ptFrame;
    U16 u16ChkPos = sizeof(*ptFrame) + u16DataLen;

    ptFrame = (PT_CMT_FRAME)pBuf;
    ptFrame->u16Hdr = CMT_CMD_HEAD;
    ptFrame->u8Cmd  = u8CmdId;
    ptFrame->u16DataLen = u16DataLen;
    /* 内部函数，不进行越界保护 */
    memcpy(ptFrame->pData, pData, u16DataLen);
    pBuf[u16ChkPos] = Calc_ChkSum(pBuf, u16ChkPos);

    return ptFrame;
}

static INT Format_Send_Cmd(U8 u8CmdId, U8 *pData, U16 u16DataLen)
{
    U16 u16CmdLen, u16SendLen;
    PT_CMT_FRAME ptCmdBuf;
    
    ptCmdBuf = Format_Cmd(u8CmdId, pData, u16DataLen);
    u16CmdLen = sizeof(*ptCmdBuf) + ptCmdBuf->u16DataLen + CMT_CHECK_BIT_LEN;
    log_hex((U8 *)ptCmdBuf, u16CmdLen);
    u16SendLen = Drv_Commt_Send((U8 *)ptCmdBuf, u16CmdLen);
    if (u16SendLen != u16CmdLen)
    {
        log("Commt Send fail\n");
        return 1;
    }

    return 0;
}


/** 参考《stm32与rk3399通信协议》，基本协议格式如下：
 *  | 帧头 |命令标识| 数据长度      |   数据       |  校验位     |
 *  |0xC0CE|cmd(1B) | len(2B) |data(xB)|chksum(1B)|
 */
static INT Host_Cmd_Verify(U8 *pBuf, U16 u16Len)
{
    PT_CMT_FRAME ptFrm = (PT_CMT_FRAME)pBuf;
    U16 u16LenExSum;
    U8 u8ChkSum;
    
    if ((NULL == pBuf) || (u16Len <= CMT_CMD_MIN_LEN))
    {
        log("do data invalid\n");
        return 1;
    }

    /* 1. 检查帧头 */
    if (CMT_CMD_HEAD != ptFrm->u16Hdr)
    {
        log("data format err\n");
        return 1;
    }

    /* 2. 判断校验位 */
    u16LenExSum = sizeof(*ptFrm) + ptFrm->u16DataLen;
    u8ChkSum = Calc_ChkSum(pBuf, u16LenExSum);
    if (u8ChkSum != pBuf[u16LenExSum])
    {
        log("ChkSum err\n");
        return 1;
    }

    return 0;
}

static INT Do_Ctrl_Motor_Cmd(U8 *pData, U16 u16Len)
{
    FSUS_STATUS status;
    PT_CMT_MOTOR_DATA ptMotor = (PT_CMT_MOTOR_DATA)pData;

    if (u16Len != sizeof(*ptMotor))
    {
        log("set motor cmd len err\n");
        return 1;
    }
    
    status = FSUS_SetServoAngle(ptMotor->Id, ptMotor->s32Angle, ptMotor->u32Interval);
    if (FSUS_STATUS_SUCCESS != status)
    {
        log("Set Servo Angle fail\n");
        return 1;
    }

    return 0;
}

BOOL Is_Factory_Mode(VOID)
{
    return bFactoryMode;
}

static VOID Motor_Query_Angle(S32 *pAngle)
{
    int Id, ret;
    U16 u16Turn;
    S16 s16Angle;
    S32 s32Angle;
    
    for (Id = 1; Id <= 3; Id++)
    {
        if (Id < 3)
        {
            ret = FSUS_QueryServoAngle(Id, &s16Angle);
            if(FSUS_STATUS_SUCCESS != ret)
            {
                s32Angle = 0x7FFFFFFF;
                log_w("get %d ang fail\n");
            }
            else
            {
                s32Angle = s16Angle;
            }
        }
        else
        {
            ret = FSUS_QueryServoMTurnAngle(Id, &s32Angle, &u16Turn);
            if(FSUS_STATUS_SUCCESS != ret)
            {
                s32Angle = 0x7FFFFFFF;
                log_w("get %d ang fail\n");
            }
        }
        *(pAngle + Id - 1)  = s32Angle;
        
        float fAngle = (float)(s32Angle / 10.0);
        log("Id[%d] ang=%f\n", Id, fAngle);
    }
}

/* 主动上报，u16 */
INT Report_Battery_Cmd(U16 u16Battery)
{
    return Format_Send_Cmd(CMT_CMD_RPT_BATTERY, (U8 *)&u16Battery, sizeof(u16Battery));
}

/* 主动上报，按键序号 + 按键状态 */
INT Report_Key_Cmd(U8 u8KeyNo, U8 u8Stat)
{
    U8 Data[2] = {u8KeyNo, u8Stat};

    return Format_Send_Cmd(CMT_CMD_RPT_KEY, Data, sizeof(Data));
}

/* 主动上报，一个字节数据 */
INT Report_Ired_Cmd(U8 u8Stat)
{
    return Format_Send_Cmd(CMT_CMD_RPT_IRDA, &u8Stat, sizeof(u8Stat));
}

/* 上报舵机角度 */
INT Report_Angle_Cmd(U8 u8Id, S32 s32Angle)
{
    T_CMT_ANGLE tAngle = {u8Id, s32Angle};
    return Format_Send_Cmd(CMT_CMD_RPT_ANGLE, (U8 *)&tAngle, sizeof(tAngle));
}


/**
 * |数据类型|故障位|电量|按键|红外|角度|
 * |   1B   |  2B  | 1B | 4B | 1B | 12B|
 */
static INT Report_All_Cmd(VOID)
{
    T_CMT_DATA tData;

    tData.u8Type     = CMT_DT_ALL;
    tData.u16ErrBit  = 0;
    tData.u16Battery = Drv_Obtain_Baty_Volt();
    Keys_Get_Val(tData.au8Key);
    Ired_Get_Val(&tData.u8IrDA);
    Motor_Query_Angle(tData.as32Angle);

    return Format_Send_Cmd(CMT_CMD_RPT_INFO, (U8 *)&tData, sizeof(tData));
}

static INT Report_Battery(VOID)
{
    T_CMT_DATA_BAT tData = {CMT_DT_BATTERY, 0, 0};

    tData.u16Battery = Drv_Obtain_Baty_Volt();
    return Format_Send_Cmd(CMT_CMD_RPT_INFO, (U8 *)&tData, sizeof(tData));
}

static INT Report_Key(VOID)
{
    T_CMT_DATA_KEY tData = {CMT_DT_KEY, 0, 0};
    
    Keys_Get_Val(tData.au8Key);
    return Format_Send_Cmd(CMT_CMD_RPT_INFO, (U8 *)&tData, sizeof(tData));
}

static INT Report_Ired(VOID)
{
    T_CMT_DATA_IRDA tData = {CMT_DT_IRDA, 0, 0};
    
    Ired_Get_Val(&tData.u8IrDA);
    return Format_Send_Cmd(CMT_CMD_RPT_INFO, (U8 *)&tData, sizeof(tData));
}

static INT Report_Angle(VOID)
{
    T_CMT_DATA_ANG tData = {CMT_DT_ANGLE, 0, 0};
    
    Motor_Query_Angle(tData.as32Angle);
    return Format_Send_Cmd(CMT_CMD_RPT_INFO, (U8 *)&tData, sizeof(tData));
}

static T_ReportInfo atReportInfoTable[] = 
{
    {CMT_DT_ALL,     Report_All_Cmd},
    {CMT_DT_BATTERY, Report_Battery},
    {CMT_DT_KEY,     Report_Key},
    {CMT_DT_IRDA,    Report_Ired},
    {CMT_DT_ANGLE,   Report_Angle},
};

static INT Report_Cmd(INT iType)
{
    PT_ReportInfo ptReport = &atReportInfoTable[iType];
    
    if (iType >= N_ELEMENTS(atReportInfoTable))
    {
        log_e("data type[%d] err\n", iType);
        return 1;
    }        
    
    if (NULL == ptReport->pfHandler)
    {
        return 1;
    }
    
    return ptReport->pfHandler();
}

/*
 *  函 数 名：VOID Do_Host_Cmd(CHAR *pDate, U16 u16Size)
 *  输入参数：
 *  输出参数：无
 *  返 回 值：无
 *  函数作用：解析主机下发命令
 */
INT Do_Host_Cmd(U8 *pu8RecvData, U16 u16RecvLen)
{
    PT_CMT_FRAME ptFrm = (PT_CMT_FRAME)pu8RecvData;
    INT iRet;

    iRet = Host_Cmd_Verify(pu8RecvData, u16RecvLen);
    if (0 != iRet)
    {
        log_w("cmd verify err\n");
        return 1;
    }

    /* 3. 处理指令 */
    switch(ptFrm->u8Cmd) 
    {
        case CMT_CMD_POLL_INFO:
            //Report_All_Cmd();
            Report_Cmd(ptFrm->pData[0]);
            break;
        case CMT_CMD_CTRL_MOTOR:
            Do_Ctrl_Motor_Cmd(ptFrm->pData, ptFrm->u16DataLen);
            break;
        case CMT_CMD_CTRL_LED:
            /* led 没有引出 */
            break;
        case CMT_CMD_CTRL_IRDA:
            Drv_Ired_Enable(ptFrm->pData[0]);
            break;
        case CMT_CMD_CTRL_FACTORY:
            bFactoryMode = (BOOL)(ptFrm->pData[0]);
            break;
        case CMT_CMD_CTRL_MTPWR:
            Drv_Motor_Power(ptFrm->pData[0]);
            break;
        default:
            log_e("host cmd[%02X] err\n", ptFrm->u8Cmd);
    }

    return 0;
}

/* 注册命令处理事件 */
VOID DoCmd_Event_Handler(VOID)
{
    U32 u32Len;
    U8 *pBuf;
    
    u32Len = Pipo_Get_Len(Uart);
    pBuf   = Pipo_Get_Buf(Uart);
    Do_Host_Cmd(pBuf, u32Len);
    Pipo_Set_Len(Uart, 0);   
}

static PT_CMT_FRAME Build_Poll_Cmd(VOID)
{
    U8 Data;
    return Format_Cmd(CMT_CMD_POLL_INFO, (U8 *)&Data, sizeof(Data));
}

static PT_CMT_FRAME Build_Motor_Cmd(U8 u8ServoId, S32 s32Angle, U32 u32Interval)
{
    T_CMT_MOTOR_DATA tMotor;

    tMotor.Id          = u8ServoId;
    tMotor.s32Angle    = s32Angle;
    tMotor.u32Interval = u32Interval;
    return Format_Cmd(CMT_CMD_CTRL_MOTOR, (U8 *)&tMotor, sizeof(tMotor));
}

static PT_CMT_FRAME Build_Ired_Cmd(U8 Status)
{
    return Format_Cmd(CMT_CMD_CTRL_IRDA, &Status, sizeof(Status));
}

static PT_CMT_FRAME Build_Factory_Cmd(U8 Status)
{
    return Format_Cmd(CMT_CMD_CTRL_FACTORY, &Status, sizeof(Status));
}

static PT_CMT_FRAME Build_Motor_Power_Cmd(U8 Status)
{
    return Format_Cmd(CMT_CMD_CTRL_MTPWR, &Status, sizeof(Status));
}


/* shell 调试接口 */
static void Help_Link(void)
{
    sh_printf("link down info\n");
    sh_printf("link down motor 1 500 500\n");
    sh_printf("link down mtpwr 1\n");
    sh_printf("link down ired 1\n");
    sh_printf("link down fty 1\n");
}

void Shell_Link_Cmd(char argc, char *argv)
{
    char *func, *opt;
    PT_CMT_FRAME ptCmdBuf;
    U16 u16CmdLen;
    
    if (argc < 3)
    {
        Help_Link();
        return;
    }

    func = &argv[argv[1]];
    opt  = &argv[argv[2]];
    
    if (!strcmp("down", func))
    {
        if (!strcmp("info", opt))
        {
            ptCmdBuf = Build_Poll_Cmd();
        }
        else if (!strcmp("motor", opt))
        {
            INT Id          = atoi(&argv[argv[3]]);
            S32 s32Angle    = atoi(&argv[argv[4]]);
            U32 u32Interval = atoi(&argv[argv[5]]);
            
            ptCmdBuf = Build_Motor_Cmd(Id, s32Angle, u32Interval);
        }
        else if (!strcmp("ired", opt))
        {
            INT status = atoi(&argv[argv[3]]);
            ptCmdBuf = Build_Ired_Cmd(status);
        }
        else if (!strcmp("fty", opt))
        {
            INT status = atoi(&argv[argv[3]]);
            ptCmdBuf = Build_Factory_Cmd(status);
        }
        else if (!strcmp("mtpwr", opt))
        {
            INT status = atoi(&argv[argv[3]]);
            ptCmdBuf = Build_Motor_Power_Cmd(status);
        }
        else
        {
            Help_Link();
            return;
        }
        
        u16CmdLen = sizeof(*ptCmdBuf) + ptCmdBuf->u16DataLen + CMT_CHECK_BIT_LEN;
        /* 生成host指令 */
        log_hex((U8 *)ptCmdBuf, u16CmdLen);
        Do_Host_Cmd((U8 *)ptCmdBuf, u16CmdLen);
    }
    else
    {
        Help_Link();
        return;
    }
}

/** 协议测试:
 *  1.状态查询: C0 CE A0 01 00 00 2F
 *    -- 返回： C0 CE 80 16 00 00 00 00 06 1F 00 00 00 00 00 F4 FE FF FF C5 03 00 00 02 00 00 00 02
 *  2.舵机控制: C0 CE A1 05 00 02 84 03 F4 01 B2  ->  设置2号舵机在500ms内, 旋转到90度
 *   ctrl_uart: 12 4C 08 07 02 84 03 F4 01 00 00 EB
 *  3.使能红外检测：C0 CE A3 01 00 01 33
 *  4.上报红外检测：C0 CE 83 01 00 01 13
 *  5.上报按键状态：C0 CE 82 02 00 01 01 14 -> 1号按键按下
 *  6.使能工厂模式：C0 CE A4 01 00 01 34
 *  7.舵机供电：C0 CE A5 01 00 01 35
 */
 
