/*
 * Fashion Star 串口舵机驱动库
 * Version: v1
 * Author: ql
 * Time: 2022-01-18
 */

#ifndef __MOTOR_H
#define __MOTOR_H

#include "deftypes.h"

 
// FSUS通信协议
// 注: FSUS是Fashion Star Uart Servo的缩写

// 串口通讯超时设置
#define FSUS_TIMEOUT_MS                     100
// 舵机用户自定义数据块的大小 单位Byte
#define FSUS_SERVO_BATCH_DATA_SIZE          32
// 返回的响应数据包最长的长度
#define FSUS_PACK_RESPONSE_MAX_SIZE         50
// 在串口舵机的通信系统设计里, 使用的字节序为Little Endian(低字节序/小端格式)
// STM32系统的数值存储模式就是Little Endian
// 所以0x4c12 这个数值, 在实际发送的时候低位会在前面 0x12, 0x4c
#define FSUS_PACK_REQUEST_HEADER            0x4c12
#define FSUS_PACK_RESPONSE_HEADER           0x1c05

// FSUS控制指令数据
// 注: 一下所有的指令都是针对单个舵机的
#define FSUS_CMD_PING                       1 // 舵机通讯检测
#define FSUS_CMD_RESET_USER_DATA            2 // 重置用户数据
#define FSUS_CMD_READ_DATA                  3 // 单个舵机 读取数据库
#define FSUS_CMD_WRITE_DATA                 4 // 单个舵机 写入数据块
#define FSUS_CMD_READ_BATCH_DATA            5 // 单个舵机 批次读取(读取一个舵机所有的数据)
#define FSUS_CMD_WRITE_BATCH_DATA           6 // 单个舵机 批次写入(写入一个舵机所有的数据)
#define FSUS_CMD_SPIN                       7 // 单个舵机 设置轮式模式
#define FSUS_CMD_ROTATE                     8 // 角度控制模式(设置舵机的角度)) 
#define FSUS_CMD_DAMPING                    9 // 阻尼模式
#define FSUS_CMD_READ_ANGLE                 10 // 舵机角度读取

/* 特定指令，仅限于磁编码系列舵机 */
#define FSUS_CMD_MULTI_TURN                 13 // 多圈角度模式
#define FSUS_CMD_READ_MULTI_TURN_ANGLE      16 // 多圈角度模式


// FSUS状态码
#define FSUS_STATUS                         uint8_t
#define FSUS_STATUS_SUCCESS                 0 // 设置/读取成功
#define FSUS_STATUS_FAIL                    1 // 设置/读取失败
#define FSUS_STATUS_TIMEOUT                 2 // 等待超时 
#define FSUS_STATUS_WRONG_RESPONSE_HEADER   3 // 响应头不对
#define FSUS_STATUS_UNKOWN_CMD_ID           4 // 未知的控制指令
#define FSUS_STATUS_SIZE_TOO_BIG            5 // 参数的size大于FSUS_PACK_RESPONSE_MAX_SIZE里面的限制
#define FSUS_STATUS_CHECKSUM_ERROR          6 // 校验和错误
#define FSUS_STATUS_ID_NOT_MATCH            7 // 请求的舵机ID跟反馈回来的舵机ID不匹配


/* 舵机用户自定义参数的数据ID及使用说明 (可度也可写)*/

/* 此项设置同时具备两个功能
 * 在轮式模式与角度控制模式下
 * 1. 舵机指令是否可以中断 interruptable? 
 * 2. 是否产生反馈数据?
 * 0x00(默认)
 *      舵机控制指令执行可以被中断, 新的指令覆盖旧的指令
 *      无反馈数据
 * 0x01
 *      舵机控制指令不可以被中断, 新的指令添加到等候队列里面
 *      等候队列的长度是1, 需要自己在程序里面维护一个队列
 *      当新的控制指令超出了缓冲区的大小之后, 新添加的指令被忽略
 *      指令执行结束之后发送反馈数据
 */
#define FSUS_PARAM_RESPONSE_SWITCH          33
/* 
 * 舵机的ID号, (字节长度 1)
 * 取值范围是 0-254 
 * 255号为广播地址，不能赋值给舵机 广播地址在PING指令中使用。
 */
#define FSUS_PARAM_SERVO_ID                 34
/*
 * 串口通讯的波特率ID  (字节长度 1)
 * 取值范围 [0x01,0x07] , 默认值0x05
 * 0x01-9600,
 * 0x02-19200,
 * 0x03-38400,
 * 0x04-57600,
 * 0x05-115200 (默认波特率),
 * 0x06-250000,
 * 0x07-500000,
 * 波特率设置时即生效
 */
#define FSUS_PARAM_BAUDRATE                 36

/* 舵机保护值相关设置, 超过阈值舵机就进入保护模式 */
/*
 * 舵机堵转保护模式  (字节长度 1)
 * 0x00-模式1 降功率到堵轉功率上限
 * 0x01-模式2 释放舵机锁力
 */
#define FSUS_PARAM_STALL_PROTECT            37
/* 舵机堵转功率上限, (单位mW) (字节长度 2) */
#define FSUS_PARAM_STALL_POWER_LIMIT        38
/* 舵机电压下限 (单位mV) (字节长度 2) */
#define FSUS_PARAM_OVER_VOLT_LOW            39
/* 舵机电压上限 (单位mV) (字节长度 2) */
#define FSUS_PARAM_OVER_VOLT_HIGH           40
/* 舵机温度上限 (单位 摄氏度) (字节长度 2) */
#define FSUS_PARAM_OVER_TEMPERATURE         41
/* 舵机功率上限 (单位mW) (字节长度 2) */
#define FSUS_PARAM_OVER_POWER               42
/* 舵机电流上限 (单位mA) (字节长度 2) */
#define FSUS_PARAM_OVER_CURRENT             43
/*
 * 舵机启动加速度处理开关 (字节长度 1)
 * 0x00 不启动加速度处理 (无效设置)
 * 0x01 启用加速度处理(默认值)
 *      舵机梯形速度控制,根据时间t推算加速度a
 *      行程前1/4 加速
 *      行程中间1/2保持匀速
 *      行程后1/4 
 */
#define FSUS_PARAM_ACCEL_SWITCH             44
/*
 * 舵机上电锁力开关 (字节长度 1)
 * 0x00 上电舵机释放锁力(默认值)
 * 0x11 上电时刹车
 */
#define FSUS_PARAM_POWER_ON_LOCK_SWITCH     46
/*
 * [轮式模式] 轮式模式刹车开关 (字节长度 1)
 * 0x00 停止时舵机释放锁力(默认)
 * 0x01 停止时刹车
 */
#define FSUS_PARAM_WHEEL_MODE_BRAKE_SWITCH  47
/*
 * [角度模式] 角度限制开关 (字节长度 1)
 * 0x00 关闭角度限制
 * 0x01 开启角度限制
 * 注: 只有角度限制模式开启之后, 角度上限下限才有效
 */
#define FSUS_PARAM_ANGLE_LIMIT_SWITCH       48
/*
 * [角度模式] 舵机上电首次角度设置缓慢执行 (字节长度 1)
 * 0x00 关闭
 * 0x01 开启
 * 开启后更安全
 * 缓慢旋转的时间周期即为下方的”舵机上电启动时间“
 */
#define FSUS_PARAM_SOFT_START_SWITCH        49
/*
 * [角度模式] 舵机上电启动时间 (单位ms)(字节长度 2)
 * 默认值: 0x0bb8
 */
#define FSUS_PARAM_SOFT_START_TIME          50
/*
 * [角度模式] 舵机角度上限 (单位0.1度)(字节长度 2)
 */
#define FSUS_PARAM_ANGLE_LIMIT_HIGH         51
/*
 * [角度模式] 舵机角度下限 (单位0.1度)(字节长度 2)
 */
#define FSUS_PARAM_ANGLE_LIMIT_LOW          52
/*
 * [角度模式] 舵机中位角度偏移 (单位0.1度)(字节长度 2)
 */
#define FSUS_PARAM_ANGLE_MID_OFFSET         53

// 轮式模式参数定义
// Bit 0 是否运行
#define FSUS_SPIN_METHOD_STOP               0x00 // 轮子停止旋转
#define FSUS_SPIN_METHOD_START              0x01 // 轮子旋转
#define FSUS_SPIN_METHOD_CIRCLE             0x02 // 轮子定圈
#define FSUS_SPIN_METHOD_TIME               0x03 // 轮子定时
#define FSUS_SPIN_METHOD_CCW                0x00 // 轮子逆时针旋转
#define FSUS_SPIN_METHOD_CW                 0x80 // 轮子顺时针旋转

// 串口舵机用户自定义设置 
// 是否开启响应模式
#define FSUS_IS_RESPONSE_ON                 0


// 请求数据帧的结构体
#pragma pack(1)
typedef struct FSUS_CmdPkg{
    U16 u16Hdr;             // 请求头
    U8  u8CmdId;            // 指令ID号
    U8  u8DataLen;          // 包的长度
    U8  pData[0];           // 包的内容
}T_FSUS_CmdPkg, *PT_FSUS_CmdPkg;

typedef struct FSUS_AnglePara{
    U8  u8ServoId;          // 舵机ID
    S16 s16Angle;           // 舵机角度
    U16 u16Interval;        // 到达目标角度的时间
    U16 u16Power;           // 执行功率
}T_FSUS_AnglePara, *PT_FSUS_AnglePara;

/* 磁编码舵机；多圈模式 */
typedef struct FSUS_MTurnPara{
    U8  u8ServoId;          // 舵机ID
    S32 s32Angle;           // 舵机角度
    U32 u32Interval;        // 到达目标角度的时间
    U16 u16Power;           // 执行功率
}T_FSUS_MTurnPara, *PT_FSUS_MTurnPara;

/* 轮式模式 */
typedef struct FSUS_WheelPara{
    U8  u8ServoId;          // 舵机ID
    U8  u8Method;           // 执行方式&旋转方向
    U16 u16Speed;           // 舵机角速度(单位: °/s)
    U16 u16Value;
}T_FSUS_WheelPara, *PT_FSUS_WheelPara;

#pragma pack()


/* API */

// 注: 如果没有舵机响应这个Ping指令的话, 就会超时
FSUS_STATUS FSUS_Ping(U8 u8ServoId);
FSUS_STATUS FSUS_ResetUserData(U8 u8ServoId);
FSUS_STATUS FSUS_SetServoAngle(U8 u8ServoId, S16 s16Angle, U16 u16Interval);
FSUS_STATUS FSUS_QueryServoAngle(U8 u8ServoId, S16 *pAngle);
FSUS_STATUS FSUS_SetServoMTurnAngle(U8 u8ServoId, S32 s32Angle, U32 u32Interval);
FSUS_STATUS FSUS_QueryServoMTurnAngle(U8 u8ServoId, S32 *pAngle, U16 *pTurn);


#endif //__MOTOR_H

