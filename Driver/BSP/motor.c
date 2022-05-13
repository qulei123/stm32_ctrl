/* 
 * 文件名称：motor.c
 * 摘要：Fashion Star串口舵机驱动库
 * 协议手册：https://wiki.fashionrobo.com/uartbasic/uart-protocol/
 *  
 * 修改历史   版本号   Author  修改内容
 *--------------------------------------------------
 * 2022.01.18   v1      ql     创建文件
 * 2022.04.14   v1.1    ql     舵机多圈模式测试正常
 *--------------------------------------------------
 */ 
 
#include <stdlib.h>
#include <string.h> 
#include "drv_msp.h"
#include "nr_micro_shell.h"
#include "common.h"
#include "motor.h"


/* 发送接收使用同一个buf */
#define RESP_LEN_MIN        5
#define CMD_BUF_LEN         64

static U8 CmdBuf[CMD_BUF_LEN];

static inline U8* Get_Buf(VOID)
{
    return CmdBuf;
}

static FSUS_STATUS FSUS_Resp_Verify(U8 u8CmdId, U8 *pu8Buf, U16 u16Len)
{
    PT_FSUS_CmdPkg ptResp = (PT_FSUS_CmdPkg)pu8Buf;
    U16 u16RespLenExSum;
    U8  u8ChkSum;
    
    if (u16Len <= RESP_LEN_MIN)
    {
        log_e("Resp data invalid\n");
        return FSUS_STATUS_FAIL;
    }

    /* 1. 检查帧头 */
    if (FSUS_PACK_RESPONSE_HEADER != ptResp->u16Hdr)
    {
        log_e("data format err\n");
        return FSUS_STATUS_FAIL;
    }

    /* 2. 判断CmdId */
    if (u8CmdId != ptResp->u8CmdId)
    {
        log_e("data len err\n");
        return FSUS_STATUS_FAIL;
    }

    /* 3. 判断校验位 */
    u16RespLenExSum = sizeof(*ptResp) + ptResp->u8DataLen;
    u8ChkSum = Calc_ChkSum(pu8Buf, u16RespLenExSum);
    if (u8ChkSum != pu8Buf[u16RespLenExSum])
    {
        log_e("ChkSum err\n");
        return FSUS_STATUS_FAIL;
    }

    return FSUS_STATUS_SUCCESS;
}


static U8 *FSUS_Recv_Resp(U8 u8CmdId)
{
    U16 u16Len;
    FSUS_STATUS status;
    U8 *pu8RecvBuf = Get_Buf();
        
    u16Len = Drv_Ctrl1_Recv(pu8RecvBuf, CMD_BUF_LEN);
    if (0 == u16Len)
    {
        log_e("Recv[%d] fail\n", u8CmdId);
        return NULL;
    }

    status = FSUS_Resp_Verify(u8CmdId, pu8RecvBuf, u16Len);
    if (FSUS_STATUS_SUCCESS != status)
    {
        log_e("FSUS_Resp_Verify[%d] fail\n", u8CmdId);
        return NULL;
    }

    return pu8RecvBuf;
}


// 构造发送数据帧
static PT_FSUS_CmdPkg FSUS_Format_Cmd(U8 u8CmdId, U8 *pu8Data, U8 u8DataLen)
{
    U8 *pu8Buf = Get_Buf();
    PT_FSUS_CmdPkg ptCmdBuf;
    U16 u16ChkPos = sizeof(*ptCmdBuf) + u8DataLen;
    
    ptCmdBuf = (PT_FSUS_CmdPkg)pu8Buf;
    ptCmdBuf->u16Hdr = FSUS_PACK_REQUEST_HEADER;
    ptCmdBuf->u8CmdId = u8CmdId;
    ptCmdBuf->u8DataLen = u8DataLen;
    /* 内部函数，不进行越界保护 */
    memcpy(ptCmdBuf->pData, pu8Data, u8DataLen);
    
    pu8Buf[u16ChkPos] = Calc_ChkSum(pu8Buf, u16ChkPos);
    
    return ptCmdBuf;
}

static FSUS_STATUS FSUS_Send_Cmd(U8 u8CmdId, U8 *pu8Data, U8 u8DataLen)
{    
    U16 u16CmdLen, u16SendLen;
    PT_FSUS_CmdPkg ptCmdBuf;
        
    ptCmdBuf = FSUS_Format_Cmd(u8CmdId, pu8Data, u8DataLen);
    u16CmdLen = sizeof(*ptCmdBuf) + ptCmdBuf->u8DataLen + 1;
    
    //log_hex((U8 *)ptCmdBuf, u16CmdLen);
    u16SendLen = Drv_Ctrl1_Send((U8 *)ptCmdBuf, u16CmdLen);
    if (u16SendLen != u16CmdLen)
    {
        log_e("Ctrl_Send[%d] fail\n", u8CmdId);
        return FSUS_STATUS_FAIL;
    }

    return FSUS_STATUS_SUCCESS;
}

static PT_FSUS_CmdPkg FSUS_TR_Verify(U8 u8CmdId, U8 *pu8Buf, U8 u8Len)
{
    FSUS_STATUS status;
    U8 *pu8RecvBuf;

    status = FSUS_Send_Cmd(u8CmdId, pu8Buf, u8Len);
    if (FSUS_STATUS_SUCCESS != status)
    {
        log_e("FSUS_Send_Cmd[%d] fail\n", u8CmdId);
        return NULL;
    }

    pu8RecvBuf = FSUS_Recv_Resp(u8CmdId);
    if (NULL == pu8RecvBuf)
    {
        log_e("FSUS_RecvResp[%d] fail\n", u8CmdId);
        return NULL;
    }

    return (PT_FSUS_CmdPkg)pu8RecvBuf;
}


/********************************************************************/
/********************** 舵机的各种命令生成 **************************/
/********************************************************************/

/* 舵机通讯检测 */
FSUS_STATUS FSUS_Ping(U8 u8ServoId)
{
    PT_FSUS_CmdPkg ptResp;
    U8 u8CmdId = FSUS_CMD_PING;
    
    ptResp = FSUS_TR_Verify(u8CmdId, &u8ServoId, 1);
    if (NULL == ptResp)
    {
        log("TR_Verify[%d] fail\n", u8CmdId);
        return FSUS_STATUS_FAIL;
    }
    
    if (ptResp->pData[0] != u8ServoId)
    {
        log("Recv Resp id[%d] not match\n", ptResp->pData[0]);
        return FSUS_STATUS_ID_NOT_MATCH;
    }

    return FSUS_STATUS_SUCCESS;
}

/* 重置舵机的用户资料 */
FSUS_STATUS FSUS_ResetUserData(U8 u8ServoId)
{
    PT_FSUS_CmdPkg ptResp;
    U8 u8CmdId = FSUS_CMD_RESET_USER_DATA;
        
    ptResp = FSUS_TR_Verify(u8CmdId, &u8ServoId, 1);
    if (NULL == ptResp)
    {
        log("TR_Verify[%d] fail\n", u8CmdId);
        return FSUS_STATUS_FAIL;
    }

    /* id + status */
    if ((ptResp->pData[0] != u8ServoId) || (ptResp->pData[1] != 1))
    {
        log("ResetUserData fail\n");
        return FSUS_STATUS_FAIL;
    }

    return FSUS_STATUS_SUCCESS;
}

/* 角度模式
 * @angle   : 单位:0.1度
 * @interval: 单位:ms
 * @power   : 单位mW, 若power=0或者大于保护值
 */
FSUS_STATUS FSUS_SetServoAngle(U8 u8ServoId, S16 s16Angle, U16 u16Interval)
{
    T_FSUS_AnglePara tAngle;

    tAngle.u8ServoId   = u8ServoId;
    tAngle.s16Angle    = s16Angle;
    tAngle.u16Interval = u16Interval;
    tAngle.u16Power    = 0;

    return FSUS_Send_Cmd(FSUS_CMD_ROTATE, (U8 *)&tAngle, sizeof(tAngle));
}


/* 查询舵机的角度信息 */
FSUS_STATUS FSUS_QueryServoAngle(U8 u8ServoId, S16 *pAngle)
{
    PT_FSUS_CmdPkg ptResp;
    U8 u8CmdId = FSUS_CMD_READ_ANGLE;
        
    ptResp = FSUS_TR_Verify(u8CmdId, &u8ServoId, 1);
    if (NULL == ptResp)
    {
        log_e("TR_Verify[%d] fail\n", u8CmdId);
        return FSUS_STATUS_FAIL;
    }

    /* id + angle */
    if (ptResp->pData[0] != u8ServoId)
    {
        log_e("QueryServoAngle fail\n");
        return FSUS_STATUS_ID_NOT_MATCH;
    }
    
    // 提取舵机角度
    *pAngle = GET_2B_MEM(ptResp->pData + 1);
    
    return FSUS_STATUS_SUCCESS;
}	


/* 多圈角度模式，仅限于磁编码系列舵机
 * 12 4C 0D 0B 03 94 11 00 00 88 13 00 00 00 00 B9 - 5000ms顺时针转450度
 * 12 4C 07 06 03 00 00 00 00 00 6E                - 停止
 */
FSUS_STATUS FSUS_SetServoMTurnAngle(U8 u8ServoId, S32 s32Angle, U32 u32Interval)
{
    T_FSUS_MTurnPara tMTurn;

    tMTurn.u8ServoId   = u8ServoId;
    tMTurn.s32Angle    = s32Angle;
    tMTurn.u32Interval = u32Interval;
    tMTurn.u16Power    = 0;

    return FSUS_Send_Cmd(FSUS_CMD_MULTI_TURN, (U8 *)&tMTurn, sizeof(tMTurn));
}

FSUS_STATUS FSUS_QueryServoMTurnAngle(U8 u8ServoId, S32 *pAngle, U16 *pTurn)
{
    PT_FSUS_CmdPkg ptResp;
    U8 u8CmdId = FSUS_CMD_READ_MULTI_TURN_ANGLE;
        
    ptResp = FSUS_TR_Verify(u8CmdId, &u8ServoId, 1);
    if (NULL == ptResp)
    {
        log("TR_Verify[%d] fail\n", u8CmdId);
        return FSUS_STATUS_FAIL;
    }

    /* id + angle + trun*/
    if (ptResp->pData[0] != u8ServoId)
    {
        log("QueryServoAngle fail\n");
        return FSUS_STATUS_ID_NOT_MATCH;
    }
    
    /* 提取舵机角度和圈数 */
    *pAngle = GET_4B_MEM(ptResp->pData + 1);
    *pTurn  = GET_2B_MEM(ptResp->pData + 5);
     
    return FSUS_STATUS_SUCCESS;
}   

static PT_FSUS_CmdPkg FSUS_ReadData(uint8_t u8ServoId,  uint8_t u8DataId)
{
    PT_FSUS_CmdPkg ptResp;
    uint8_t data[2] = {u8ServoId, u8DataId};
   
    ptResp =  FSUS_TR_Verify(FSUS_CMD_READ_DATA, data, sizeof(data));
    if (NULL == ptResp)
    {
        return NULL;
    }

    /* Id + DataId + n Data */
    if ( (u8ServoId != ptResp->pData[0]) ||
        (u8DataId != ptResp->pData[1]))
    {
        log("FSUS_ReadData fail %d\n", FSUS_STATUS_ID_NOT_MATCH);
        return NULL;
    }

    return ptResp;
}


FSUS_STATUS FSUS_QueryServoId(uint8_t u8ServoId, uint8_t *Id)
{
    PT_FSUS_CmdPkg ptResp;

    ptResp = FSUS_ReadData(u8ServoId, FSUS_PARAM_SERVO_ID);
    if (NULL == ptResp)
    {
        return FSUS_STATUS_FAIL;
    }

    *Id = ptResp->pData[2];

    return FSUS_STATUS_SUCCESS;
}

FSUS_STATUS FSUS_SetServoId(uint8_t u8ServoId, uint8_t NewId)
{
    uint8_t data[3];

    data[0] = u8ServoId;
    data[1] = FSUS_PARAM_SERVO_ID;
    data[2] = NewId;
    
    return FSUS_Send_Cmd(FSUS_CMD_WRITE_DATA, data, sizeof(data));
}


FSUS_STATUS FSUS_SetRespSwitch(uint8_t u8ServoId, uint8_t Val)
{
    uint8_t data[3];

    data[0] = u8ServoId;
    data[1] = FSUS_PARAM_RESPONSE_SWITCH;
    data[2] = Val;
    
    return FSUS_Send_Cmd(FSUS_CMD_WRITE_DATA, data, sizeof(data));
}


/** 轮式控制模式
 *  speed单位 °/s
 */
static FSUS_STATUS FSUS_WheelMove(uint8_t servoId, uint8_t method, uint16_t speed, uint16_t value)
{
    T_FSUS_WheelPara tWheel;

    tWheel.u8ServoId = servoId;
    tWheel.u8Method  = method;
    tWheel.u16Speed  = speed;
    tWheel.u16Value  = value;

    return FSUS_Send_Cmd(FSUS_CMD_SPIN, (U8 *)&tWheel, sizeof(tWheel));
}


/* 轮式模式, 舵机停止转动 */
FSUS_STATUS FSUS_WheelStop(uint8_t servoId)
{
    return FSUS_WheelMove(servoId, FSUS_SPIN_METHOD_STOP, 0, 0);
}

/* 轮式模式 不停的旋转 */
FSUS_STATUS FSUS_WheelKeepMove(uint8_t servoId, uint8_t is_cw, uint16_t speed)
{
    uint8_t method = FSUS_SPIN_METHOD_START;
    
    if (is_cw)
    {
        /* 顺时针旋转 */
        method = method | 0x80;
    }
    return FSUS_WheelMove(servoId, method, speed, 0);
}

/* 轮式模式 按照特定的速度旋转特定的时间 */
FSUS_STATUS FSUS_WheelMoveTime(uint8_t servoId, uint8_t is_cw, uint16_t speed, uint16_t nTime)
{
    uint8_t method = FSUS_SPIN_METHOD_TIME;
    
    if (is_cw)
    {
        /* 顺时针旋转 */
        method = method | 0x80;
    }
    return FSUS_WheelMove(servoId, method, speed, nTime);
}

/* 轮式模式 旋转特定的圈数 */
FSUS_STATUS FSUS_WheelMoveNCircle(uint8_t servoId, uint8_t is_cw, uint16_t speed, uint16_t nCircle)
{
    uint8_t method = FSUS_SPIN_METHOD_CIRCLE;
    
    if (is_cw)
    {
        /* 顺时针旋转 */
        method = method | 0x80;
    }
    return FSUS_WheelMove(servoId, method, speed, nCircle);
}


/** shell 测试
 *  motor opt func id ang(0.1) time(ms)
 *  motor set ang  1  900      500
 *  motor get ang  1
 */
static void help_motor(void)
{
    /* 角度模式 */
    sh_printf("motor func opt  id ang(0.1) time(ms)\n");
    sh_printf("motor ang  set  1   900      500\n");
    sh_printf("motor ang  get  1\n");
    /* 设置读取参数 */
    sh_printf("motor func opt  id newid\n");
    sh_printf("motor id   set  1   2\n");
    sh_printf("motor id   get  x\n");
    sh_printf("motor resp set  1 on/off\n");
    /* 多圈角度模式 */
    sh_printf("motor mcc  set  1   900      500\n");
    sh_printf("motor mcc  get  1\n");
    /* 停止 */
    sh_printf("motor whl  stop 1\n");
}

void shell_motor_cmd(char argc, char *argv)
{
    char *func, *opt;
    int  id;
    
    if (argc < 4)
    {
        help_motor();
        return;
    }

    func = &argv[argv[1]];
    opt  = &argv[argv[2]];
    id    = atoi(&argv[argv[3]]);

    if (!strcmp("ang", func))
    {
        if (!strcmp("set", opt))
        {
            int  angle = atoi(&argv[argv[4]]);
            int  time  = atoi(&argv[argv[5]]);
            
            FSUS_SetServoAngle(id, angle, time);
        }
        else
        {
            /* 默认get */
            int16_t s16Angle;
            float fAngle;
            
            FSUS_QueryServoAngle(id, &s16Angle);
            fAngle = (float)(s16Angle / 10.0);
            sh_printf("Id[%d] ang=%f\n", id, fAngle);
        }    
    }
    else if (!strcmp("mcc", func))
    {
        if (!strcmp("set", opt))
        {
            int  angle = atoi(&argv[argv[4]]);
            int  time  = atoi(&argv[argv[5]]);
            
            FSUS_SetServoMTurnAngle(id, angle, time);
        }
        else
        {
            /* 默认get */
            int32_t s32Angle;
            float   fAngle;
            uint16_t turn;

            FSUS_QueryServoMTurnAngle(id, &s32Angle, &turn);
            fAngle = (float)(s32Angle / 10.0);
            sh_printf("Id[%d] ang=%f, trun=%d\n", id, fAngle, turn);
        }    
    }
    else if (!strcmp("id", func))
    {
        if (!strcmp("set", opt))
        {
            int NewId = atoi(&argv[argv[4]]);
            FSUS_SetServoId(id, NewId);
        }
        else
        {
            /* 默认get */
            int idx;
            for (idx = 0; idx < 255; idx++)
            {
                if(!FSUS_Ping(idx))
                {
                    sh_printf("motor Id[%d]\n", idx);
                    break;
                }
            }
        }
    }
    else if (!strcmp("resp", func))
    {
        /* 默认set */
        char *en = &argv[argv[4]];
        int sw = 1;
        
        if (!strcmp("off", en))
        {
            sw = 0;
        }
        FSUS_SetRespSwitch(id, sw);
    }
    else if (!strcmp("whl", func))
    {        
        if (!strcmp("stop", opt))
        {
            FSUS_WheelStop(id);
        }
    }
    else if (!strcmp("pwr", func))
    {
        int sw = 1;
        
        if (!strcmp("off", opt))
        {
            sw = 0;
            sh_printf("motor off\n");
        }
        Drv_Motor_Power(sw);
    }
    else
    {
        help_motor();
    }
}


/** 舵机零位校准流程
 *  1. 手动转动电机到左右/上下的极限位置，分别发送命令获取当前位置的角度
 *  2. 发送命令自动让舵机从左->右(上->下)，零位检测触发，读取角度(自动上报角度)
 *                       从右->左(下->上)，零位检测触发，读取角度(自动上报角度)
 *  3. 根据这两个角度，计算舵机零位
 */

