
#ifndef __DRV_UART_H_
#define __DRV_UART_H_

#include "drv_msp.h"
#include "pipo.h"

#define TIMEOUT_MAX                 HAL_MAX_DELAY

/* debug ------------------------------------------------------------------*/
int Drv_Dbg_Uart_Init(uint32_t u32Bps);
void Drv_Dbg_Putc(char ch);
void Drv_Dbg_Puts(char *pdata, uint16_t u16Size);
void Dbg_Shell_Handler(void);


/* 通信 ------------------------------------------------------------------*/
#define COMMT_TX_TIMEOUT            50
#define COMMT_RX_TIMEOUT            100

/* pipo声明 */
PIPO_EXTERN(Uart);

int Drv_Commt_Uart_Init(uint32_t u32Bps);
uint16_t Drv_Commt_Send(uint8_t *pdata, uint16_t u16Size);
uint16_t Drv_Commt_Recv(uint8_t *pdata, uint16_t u16Size);


/* 舵机控制 ---------------------------------------------------------------*/
#define CTRL_TX_TIMEOUT            50
#define CTRL_RX_TIMEOUT            100

int Drv_Ctrl1_Uart_Init(uint32_t u32Bps);
uint16_t Drv_Ctrl1_Send(uint8_t *pdata, uint16_t u16Size);
uint16_t Drv_Ctrl1_Recv(uint8_t *pdata, uint16_t u16Size);



#endif  // __DRV_UART_H_
