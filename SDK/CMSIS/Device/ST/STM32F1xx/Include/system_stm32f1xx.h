/**
  ******************************************************************************
  * @file    system_stm32f10x.h
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Header File.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f10x_system
  * @{
  */  
  
/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSTEM_STM32F10X_H
#define __SYSTEM_STM32F10X_H

#ifdef __cplusplus
 extern "C" {
#endif 

/** @addtogroup STM32F10x_System_Includes
  * @{
  */

/**
  * @}
  */


/** @addtogroup STM32F10x_System_Exported_types
  * @{
  */

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */
extern const uint8_t  AHBPrescTable[16U];  /*!< AHB prescalers table values */
extern const uint8_t  APBPrescTable[8U];   /*!< APB prescalers table values */

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Exported_Macros
  * @{
  */

/* 位带操作,实现51类似的GPIO控制功能
 * 具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
 */
/* IO口操作宏定义 */
#define BITBAND(addr, bitnum)   ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2)) 
#define MEM_ADDR(addr)          *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)  MEM_ADDR(BITBAND(addr, bitnum)) 

/* IO口地址映射 */
#define GPIOA_ODR_Addr    (GPIOA_BASE + 12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE + 12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE + 12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE + 12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE + 12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE + 12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE + 12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE + 8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE + 8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE + 8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE + 8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE + 8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE + 8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE + 8) //0x40011E08 
 
/* IO口操作,只对单一的IO口, 确保n的值小于16! */
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr, n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr, n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr, n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr, n)  //输入
/**
  * @}
  */

/** @addtogroup STM32F10x_System_Exported_Functions
  * @{
  */
  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_STM32F10X_H */

/**
  * @}
  */
  
/**
  * @}
  */  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
