
#ifndef __DRV_MSP_H_
#define __DRV_MSP_H_

#include "config.h"
#include "stm32f1xx_hal.h"
#include "util_log.h"
#include "drv_adc.h"
#include "drv_uart.h"
#include "drv_delay.h"
#include "drv_gpio.h"
#include "drv_wdg.h"


//#define BOARD_FIRE
enum
{
    ID_DBG_TX  = 0,
    ID_DBG_RX,
    ID_CMT_TX,
    ID_CMT_RX,
    ID_CT1_TX,
    ID_CT1_RX,
    //ID_CT2_TX,
    //ID_CT2_RX,
    ID_ZPT0,
    ID_ZPT1,
    ID_KEY0,
    ID_KEY1,
    ID_KEY2,
    ID_KEY3,
    ID_IR_IN,
    ID_IR_EN,
    ID_DS18B20,
    ID_BAT_VOL,
    ID_ACP_IN,
    ID_MTO_PWR,
    ID_LED0,
    ID_MAX
};

typedef struct
{
    uint8_t     Enable;
    uint8_t     IrqNum;
    uint8_t     PreemptPriority;
    uint8_t     SubPriority;
} T_IntInfo, *PT_IntInfo;

typedef struct
{
    uint32_t            Id;
    GPIO_TypeDef        *GPIOx;     /*!< GPIOx: where x can be (A..G) to select the GPIO peripheral */
    GPIO_InitTypeDef    GPIO_Init;
} T_GPIO_InitInfo, *PT_GPIO_InitInfo;

/* 定义所有GPIO */
#ifdef BOARD_FIRE
/* 使用DB9(J13)作为debug */
#define DEBUG_UART                  USART2
#define DEBUG_UART_TX_PORT          GPIOA
#define DEBUG_UART_TX_PIN           GPIO_PIN_2
#define DEBUG_UART_RX_PORT          GPIOA
#define DEBUG_UART_RX_PIN           GPIO_PIN_3
#define DEBUG_UART_CLK_EN()         __HAL_RCC_USART2_CLK_ENABLE()
#define DEBUG_UART_IRQn             USART2_IRQn
#define DEBUG_UART_IRQHandler       USART2_IRQHandler
#else
#define DEBUG_UART                  UART5
#define DEBUG_UART_TX_PORT          GPIOC
#define DEBUG_UART_TX_PIN           GPIO_PIN_12
#define DEBUG_UART_RX_PORT          GPIOD
#define DEBUG_UART_RX_PIN           GPIO_PIN_2
#define DEBUG_UART_CLK_EN()         __HAL_RCC_UART5_CLK_ENABLE()
#define DEBUG_UART_IRQn             UART5_IRQn
#define DEBUG_UART_IRQHandler       UART5_IRQHandler
#endif

#define COMMT_UART                  USART1
#define COMMT_UART_PORT             GPIOA
#define COMMT_UART_TX_PIN           GPIO_PIN_9
#define COMMT_UART_RX_PIN           GPIO_PIN_10
#define COMMT_UART_CLK_EN()         __HAL_RCC_USART1_CLK_ENABLE()
#define COMMT_UART_IRQn             USART1_IRQn
#define COMMT_UART_IRQHandler       USART1_IRQHandler

#define CTRL1_UART                  USART3
#define CTRL1_UART_PORT             GPIOB
#define CTRL1_UART_TX_PIN           GPIO_PIN_10
#define CTRL1_UART_RX_PIN           GPIO_PIN_11
#define CTRL1_UART_CLK_EN()         __HAL_RCC_USART3_CLK_ENABLE()

#if 0
#define CTRL2_UART                  UART5
#define CTRL2_UART_TX_PORT          GPIOC
#define CTRL2_UART_TX_PIN           GPIO_PIN_12
#define CTRL2_UART_RX_PORT          GPIOD
#define CTRL2_UART_RX_PIN           GPIO_PIN_2
#define CTRL2_UART_CLK_EN()         __HAL_RCC_UART5_CLK_ENABLE()
#endif

#ifdef BOARD_FIRE
/* PA0 -> K1 */
#define KEY0_GPIO_PORT              GPIOA
#define KEY0_GPIO_PIN               GPIO_PIN_0
#define KEY1_GPIO_PORT              GPIOC
#define KEY1_GPIO_PIN               GPIO_PIN_9
#define KEY2_GPIO_PORT              GPIOC
#define KEY2_GPIO_PIN               GPIO_PIN_10
/* PC13 -> K2 */
#define KEY3_GPIO_PORT              GPIOC
#define KEY3_GPIO_PIN               GPIO_PIN_13

/* PB5 -> LED_R(D3) */
#define LED0_GPIO_PORT              GPIOB
#define LED0_GPIO_PIN               GPIO_PIN_5

/* PD6 -> DS18B20(U16) */
#define DS18B20_DQ_PORT             GPIOD
#define DS18B20_DQ_PIN              GPIO_PIN_6

/* 以下的定义只是为了编译通过，开发板没有相关外设 */
/* PA5 -> Infrared detection */
#define IRED_IN_GPIO_PORT           GPIOA
#define IRED_IN_GPIO_PIN            GPIO_PIN_5

/* PA6 -> Ired 使能 */
#define IRED_EN_GPIO_PORT           GPIOA
#define IRED_EN_GPIO_PIN            GPIO_PIN_6

/* 输出唤醒 */
#define WKUP_GPIO_PIN               GPIO_PIN_0
#define WKUP_GPIO_PORT              GPIOA

/* charge */
#define CHARGE_EN_PORT              GPIOB
#define CHARGE_EN_PIN               GPIO_PIN_5
#define CHARGE_MODE_PORT            GPIOB
#define CHARGE_MODE_PIN             GPIO_PIN_6
/** 适配器接入检测
 * PB3 -> 默认高电平，适配器接入，会产生一个60ms左右的低脉冲
 * PB4 -> 默认低电平，适配器接入，会产生高电平
 */
#define CHARGE_DTCT_PORT            GPIOB
#define CHARGE_DTCT_PIN             GPIO_PIN_4  /* 输入 */
#define CHARGE_CTRL_PORT            GPIOB
#define CHARGE_CTRL_PIN             GPIO_PIN_8

/* 舵机零位检测 */
#define ZPT0_GPIO_PORT              GPIOB
#define ZPT0_GPIO_PIN               GPIO_PIN_15
#define ZPT1_GPIO_PORT              GPIOB
#define ZPT1_GPIO_PIN               GPIO_PIN_14
#define ZPT2_GPIO_PORT              GPIOB
#define ZPT2_GPIO_PIN               GPIO_PIN_13

/* ADC 电压采集 */
#define BATY_VOLT_ADC               ADC1
#define BATY_VOLT_ADC_PORT          GPIOC
#define BATY_VOLT_ADC_PIN           GPIO_PIN_1
#define BATY_VOLT_ADC_CHL           ADC_CHANNEL_11
#define BATY_VOLT_ADC_CLK_EN()      __HAL_RCC_ADC1_CLK_ENABLE()
#define BATY_VOLT_ADC_IRQ           ADC1_IRQn
#define BATY_VOLT_ADC_IRQHandler    ADC1_IRQHandler

#else

/* key */
#define KEY0_GPIO_PORT              GPIOC
#define KEY0_GPIO_PIN               GPIO_PIN_8
#define KEY1_GPIO_PORT              GPIOC
#define KEY1_GPIO_PIN               GPIO_PIN_9
#define KEY2_GPIO_PORT              GPIOC
#define KEY2_GPIO_PIN               GPIO_PIN_10
#define KEY3_GPIO_PORT              GPIOC
#define KEY3_GPIO_PIN               GPIO_PIN_11

/* PA5 -> Infrared detection */
#define IRED_IN_GPIO_PORT           GPIOA
#define IRED_IN_GPIO_PIN            GPIO_PIN_5

/* PA6 -> Ired 使能 */
#define IRED_EN_GPIO_PORT           GPIOA
#define IRED_EN_GPIO_PIN            GPIO_PIN_6

/* 输出唤醒 */
#define WKUP_GPIO_PIN               GPIO_PIN_0
#define WKUP_GPIO_PORT              GPIOA

/* PA1 -> LED1 */
#define LED0_GPIO_PORT              GPIOA
#define LED0_GPIO_PIN               GPIO_PIN_1

/* PA12 -> DS18B20Z(U15) */
#define DS18B20_DQ_PORT             GPIOA
#define DS18B20_DQ_PIN              GPIO_PIN_12

/* charge */
#define CHARGE_EN_PORT              GPIOB
#define CHARGE_EN_PIN               GPIO_PIN_5
#define CHARGE_MODE_PORT            GPIOB
#define CHARGE_MODE_PIN             GPIO_PIN_6
/** 适配器接入检测
 * PB3 -> 默认高电平，适配器接入，会产生一个60ms左右的低脉冲
 * PB4 -> 默认低电平，适配器接入，会产生高电平
 */
#define CHARGE_DTCT_PORT            GPIOB
#define CHARGE_DTCT_PIN             GPIO_PIN_4  /* 输入 */
#define CHARGE_CTRL_PORT            GPIOB
#define CHARGE_CTRL_PIN             GPIO_PIN_8

/* 舵机零位检测 */
#define MOTOR_PWR_PORT              GPIOB
#define MOTOR_PWR_PIN               GPIO_PIN_7
#define ZPT0_GPIO_PORT              GPIOB
#define ZPT0_GPIO_PIN               GPIO_PIN_15
#define ZPT1_GPIO_PORT              GPIOB
#define ZPT1_GPIO_PIN               GPIO_PIN_14
#define ZPT2_GPIO_PORT              GPIOB
#define ZPT2_GPIO_PIN               GPIO_PIN_13

/* ADC 电压采集 */
#define BATY_VOLT_ADC               ADC1
#define BATY_VOLT_ADC_PORT          GPIOC
#define BATY_VOLT_ADC_PIN           GPIO_PIN_5
#define BATY_VOLT_ADC_CHL           ADC_CHANNEL_15
#define BATY_VOLT_ADC_CLK_EN()      __HAL_RCC_ADC1_CLK_ENABLE()
#define BATY_VOLT_ADC_IRQ           ADC1_IRQn
#define BATY_VOLT_ADC_IRQHandler    ADC1_IRQHandler

#endif

/* Definition of TIM instance */
#define TIM_LSI                     TIM5
#define TIM_LSI_CLK_ENABLE()        __HAL_RCC_TIM5_CLK_ENABLE()
#define TIM_LSI_IRQn                TIM5_IRQn
#define TIM_LSI_IRQHandler          TIM5_IRQHandler
#define TIM_LSI_ICPSC               8                       /* 8个事件捕获一次 */


#define DRV_OK      0
#define DRV_ERR     1


PT_GPIO_InitInfo Drv_Msp_GpioInfo(void);
int Drv_Msp_Init(void);


#endif  // __DRV_MSP_H_

