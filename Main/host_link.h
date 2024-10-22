 
#ifndef __HOST_LINK_H
#define __HOST_LINK_H

#include "deftypes.h"

/** 参考《stm32与rk3399通信协议》，基本协议格式如下：
 *  | 帧头 |命令标识| 数据长度      |   数据       |  校验位     |
 *  |0xC0CE|cmd(1B) | len(2B) |data(xB)|chksum(1B)|
 */

typedef INT (*PF_Report)(VOID);

typedef struct
{
    U8  Type;
    PF_Report pfHandler;
} T_ReportInfo, *PT_ReportInfo;

#pragma pack(1)
typedef struct CMT_FRAME
{
    U16 u16Hdr;
    U8  u8Cmd;
    U16 u16DataLen;
    U8  pData[0];
}T_CMT_FRAME, *PT_CMT_FRAME;


/*
 * |故障位|电量|按键|红外|角度|
 * |  2B  | 1B | 4B | 1B |12B |
 */
typedef struct CMT_DATA
{
    U8      u8Type;
    U16     u16ErrBit;
    U16     u16BatyVolt;
    U8      u8VoltLevel;
    U8      au8Key[4];
    U8      u8IrDA;
    U8      u8Adapter;
    S32     as32Angle[3];
}T_CMT_DATA, *PT_CMT_DATA;

typedef struct CMT_DATA_BAT
{
    U8      u8Type;
    U16     u16ErrBit;
    U16     u16BatyVolt;
    U8      u8VoltLevel;
}T_CMT_DATA_BAT, *PT_CMT_DATA_BAT;

typedef struct CMT_DATA_KEY
{
    U8      u8Type;
    U16     u16ErrBit;
    U8      au8Key[4];
}T_CMT_DATA_KEY, *PT_CMT_DATA_KEY;

typedef struct CMT_DATA_IRDA
{
    U8      u8Type;
    U16     u16ErrBit;
    U8      u8IrDA;
}T_CMT_DATA_IRDA, *PT_CMT_DATA_IRDA;

typedef struct CMT_DATA_ANG
{
    U8      u8Type;
    U16     u16ErrBit;
    S32     as32Angle[3];
}T_CMT_DATA_ANG, *PT_CMT_DATA_ANG;

typedef struct CMT_DATA_ADP
{
    U8      u8Type;
    U16     u16ErrBit;
    U8      u8Adapter;
}T_CMT_DATA_ADP, *PT_CMT_DATA_ADP;


/**| id |角度|时间|
 * | 1B | 4B | 4B |
 */
typedef struct CMT_MOTOR_DATA
{
    U8      Id;
    S32     s32Angle;
    U32     u32Interval;
}T_CMT_MOTOR_DATA, *PT_CMT_MOTOR_DATA;

typedef struct CMT_ANGLE
{
    U8      Id;
    S32     s32Angle;
}T_CMT_ANGLE, *PT_CMT_ANGLE;

typedef struct CMT_BAT
{
    U16     u16BatyVolt;
    U8      u8VoltLevel;
}T_CMT_BAT, *PT_CMT_BAT;

#pragma pack()


#define CMT_BUF_SIZE          32
#define CMT_CMD_MIN_LEN       6         /* 命令不包括数据的长度 */
#define CMT_CHECK_BIT_LEN     1         /* 校验位的长度 */


/* 通信协议宏定义 */
#define CMT_CMD_HEAD          0xCEC0

// host -> stm32
#define CMT_CMD_POLL_INFO     0xA0
#define CMT_CMD_CTRL_MOTOR    0xA1
#define CMT_CMD_CTRL_LED      0xA2
#define CMT_CMD_CTRL_IRDA     0xA3
#define CMT_CMD_CTRL_FACTORY  0xA4
#define CMT_CMD_CTRL_MTPWR    0xA5


// host <- stm32
#define CMT_CMD_RPT_INFO      0x80
#define CMT_CMD_RPT_BATTERY   0x81
#define CMT_CMD_RPT_KEY       0x82
#define CMT_CMD_RPT_IRDA      0x83
#define CMT_CMD_RPT_ANGLE     0x84
#define CMT_CMD_RPT_ADAPTER   0x85


/* 数据类型 */
#define CMT_DT_ALL            0
#define CMT_DT_BATTERY        1
#define CMT_DT_KEY            2
#define CMT_DT_IRDA           3
#define CMT_DT_ANGLE          4
#define CMT_DT_ADAPTER        5


INT Do_Host_Cmd(U8 *pRecvData, U16 u16RecvLen);
INT Report_Ired_Cmd(U8 u8Stat);
INT Report_Key_Cmd(U8 u8KeyNo, U8 u8Stat);
INT Report_Battery_Cmd(U16 u16BatyVolt, U8 u8VoltLevel);
INT Report_Angle_Cmd(U8 u8Id, S32 s32Angle);
INT Report_Adapter_Cmd(U8 u8Stat);
VOID DoCmd_Event_Handler(VOID);
BOOL Is_Factory_Mode(VOID);


#endif  //__HOST_LINK_H

