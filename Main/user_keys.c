/**  
 *   文件名称：user_keys.c
 *   摘要：配合flexible_button使用
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.29   v1      ql     创建文件
 *   --------------------------------------------------
 */
#include <stdlib.h> 
#include <string.h>
#include "nr_micro_shell.h"
#include "common.h"
#include "drv_msp.h"
#include "charge.h"
#include "motor.h"
#include "host_link.h"
#include "user_keys.h"


void Key0_3_Handler(flex_button_t *ptKey);
void Ired_Detct_Handler(flex_button_t *ptKey);
void Adapter_Detct_Handler(flex_button_t *ptKey);
void Zero_Detct_Handler(flex_button_t *ptKey);


static T_KeyInfo KeysInfoTable[] = 
{
    {ID_KEY0,   ACTIVE_HIGH,  BTN_EVENT_DOWN,         Key0_3_Handler},
    {ID_KEY1,   ACTIVE_HIGH,  BTN_EVENT_DOWN,         Key0_3_Handler},
    {ID_KEY2,   ACTIVE_HIGH,  BTN_EVENT_DOWN,         Key0_3_Handler},
    {ID_KEY3,   ACTIVE_HIGH,  BTN_EVENT_DOWN,         Key0_3_Handler},
    {ID_IR_IN,  ACTIVE_LOW,   BTN_EVENT_SHORT,         Ired_Detct_Handler},         /* 红外检测，会持续产生有4.5s低脉冲 */
    {ID_ACP_IN, ACTIVE_HIGH,  BTN_EVENT_DOWN | BTN_EVENT_UP,            Adapter_Detct_Handler},      /* PB4 -> 默认低电平，适配器接入，会产生高电平 */
    {ID_ZPT0,   ACTIVE_HIGH,  BTN_EVENT_DOWN,          Zero_Detct_Handler},         /* 摇头 */
    {ID_ZPT1,   ACTIVE_LOW,   BTN_EVENT_DOWN,          Zero_Detct_Handler},         /* 点头 */
};

#if 1
enum
{
    USER_KEY_0 = 0,
    USER_KEY_1,
    USER_KEY_2,
    USER_KEY_3,
    USER_IR_IN,
    USER_ACP_IN,
    USER_ZPT0,
    USER_ZPT1,
    USER_KEY_MAX
};
#endif

/* 实际按键4个，其他只是当按键处理 */ 
#define ALL_KEY_NUM     N_ELEMENTS(KeysInfoTable)
#define KEY_NUM         4

static flex_button_t    tUserKeysInfo[ALL_KEY_NUM];
static U8               UserKeysVal[KEY_NUM];
static U8               UserIredVal;
static U8               UserAdapterStatus;

static void Key0_3_Handler(flex_button_t *ptKey)
{
    UserKeysVal[ptKey->id] = 1;
#if REPORT_ENABLE        
    Report_Key_Cmd(ptKey->id, 1);
#endif
    log_i("key id %d\n", ptKey->id);
}

static void Ired_Detct_Handler(flex_button_t *ptKey)
{
    UserIredVal = 1;
    /* 红外检测不用了, 第二版硬件也没有, 暂时不用主动上报 */
#if REPORT_ENABLE        
    //Report_Ired_Cmd(1);
#endif
    //log_i("Ired id %d\n", ptKey->id);
}

/* 适配器检测, 需要检测按下和抬起事件 */
static void Adapter_Detct_Handler(flex_button_t *ptKey)
{
    if (BTN_EVENT_DOWN & TYPE_TO_BIT(ptKey->event))
    {
        UserAdapterStatus = 1;
        OnAdapter_Handler();
    }
    else
    {
        UserAdapterStatus = 0;
        OffAdapter_Handler();
    }
    
#if REPORT_ENABLE        
    Report_Adapter_Cmd(UserAdapterStatus);
#endif

    log_i("ACP id %d, ev %04x\n", ptKey->id, ptKey->event);
}


static void Zero_Detct_Handler(flex_button_t *ptKey)
{
    int id = 1;
    S16 s16Angle;
    
    //log_i("ZPT id %d, ev %04x\n", ptKey->id, ptKey->event);

    /* 在工厂模式模式下处理 */
    if (TRUE != Is_Factory_Mode())
    {
        return;
    }
    
    if (ID_ZPT0 == KeysInfoTable[ptKey->id].GpioId)
    {
        id = 2;
    }

    FSUS_QueryServoAngle(id, &s16Angle);
    Report_Angle_Cmd(id, s16Angle);
    
    log_i("Id[%d] ang=%f\n", id, (float)(s16Angle / 10.0));   
}

static uint8_t Keys_Read(void *arg)
{
    flex_button_t *ptKey = (flex_button_t *)arg; 
    return Drv_Gpio_Read(KeysInfoTable[ptKey->id].GpioId);
}

static void Keys_Callback(void *arg)
{
    flex_button_t *ptKey = (flex_button_t *)arg;
    PT_KeyInfo ptKeyInfo = &KeysInfoTable[ptKey->id];
    
    if (ptKeyInfo->Event & TYPE_TO_BIT(ptKey->event))
    {
        if (ptKeyInfo->pfHandler)
        {
            ptKeyInfo->pfHandler(ptKey);
        }
    }
}


INT Keys_Init(VOID)
{
    INT idx;
    flex_button_t *ptKey = &tUserKeysInfo[0];
    
    for (idx = 0; idx < N_ELEMENTS(tUserKeysInfo); idx++)
    {
        ptKey[idx].id                     = idx;
        ptKey[idx].pressed_logic_level    = KeysInfoTable[idx].Level;
        ptKey[idx].usr_button_read        = Keys_Read;
        ptKey[idx].cb                     = Keys_Callback;
        ptKey[idx].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1000);
        ptKey[idx].long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(2000);
        ptKey[idx].long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(3000);
        
        flex_button_register(&ptKey[idx]);
    }
    
    return 0;
}

VOID Keys_Get_Val(U8 *pKeyVal)
{
    memcpy(pKeyVal, UserKeysVal, sizeof(UserKeysVal));
    memset(UserKeysVal, 0, sizeof(UserKeysVal));
}

VOID Ired_Get_Val(U8 *pIredVal)
{
    *pIredVal = UserIredVal;
    //UserIredVal = 0;
}

VOID Adapter_Get_Status(U8 *pStatus)
{
    *pStatus = UserAdapterStatus;
}


/* 定时器任务20ms, 回调一次 */
VOID Keys_Scan_Handler(VOID)
{
    flex_button_scan();
}

/* shell 调试接口 */
static void Help_Key_Volt(void)
{
    sh_printf("key val get id\n");
    sh_printf("name | key0 key1 key2 key3 IRed adap ZPT0 ZPT1\n");
    sh_printf(" id  |  0    1    2    3     4    5    6    7 \n");
}

void Shell_Key_Cmd(char argc, char *argv)
{
    char *func, *opt;
    int  id;
    
    if (argc < 4)
    {
        Help_Key_Volt();
        return;
    }

    func = &argv[argv[1]];
    opt  = &argv[argv[2]];
    id   = atoi(&argv[argv[3]]);

    if ((!strcmp("val", func)) &&
        (!strcmp("get", opt)))
    {
        int val, status;
        PT_KeyInfo ptKeyInfo = &KeysInfoTable[id];
        
        val = Drv_Gpio_Read(ptKeyInfo->GpioId);
        status = !(val ^ ptKeyInfo->Level);
        
        sh_printf("key[%d], val=%d, status=%d\n", id, val, status);
    }
    else
    {
        Help_Key_Volt();
    }
}

