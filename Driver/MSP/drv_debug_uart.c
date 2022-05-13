/**  
 *   文件名称：drv_debug_uart.c
 *   摘要：配合log模块使用，或使用printf
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.25   v1      ql     创建文件
 *   --------------------------------------------------
 */
#include "drv_uart.h"
#include "mod_shell.h"

static UART_HandleTypeDef tDbgUart;


int Drv_Dbg_Uart_Init(uint32_t u32Bps)
{
    UART_HandleTypeDef *ptUart = &tDbgUart;
    
    ptUart->Instance         = DEBUG_UART;
    ptUart->Init.BaudRate    = u32Bps;
    ptUart->Init.WordLength  = USART_WORDLENGTH_8B;
    ptUart->Init.StopBits    = USART_STOPBITS_1;
    ptUart->Init.Parity      = USART_PARITY_NONE;
    ptUart->Init.Mode        = USART_MODE_TX_RX;
    ptUart->Init.HwFlowCtl   = UART_HWCONTROL_NONE;
    
    if (HAL_UART_Init(ptUart) != HAL_OK)
    {
        log_e("dbg init\n");
        return DRV_ERR;
    }
    
    //__HAL_UART_ENABLE_IT(ptUart, UART_IT_RXNE);
    SET_BIT(DEBUG_UART->CR1, USART_CR1_RXNEIE);

    return DRV_OK;
}

/* 供log使用，不关注异常 */
void Drv_Dbg_Putc(char ch)
{
    HAL_UART_Transmit(&tDbgUart, (uint8_t*)&ch, 1, 10);
}

void Drv_Dbg_Puts(char *pdata, uint16_t u16Size)
{
    HAL_UART_Transmit(&tDbgUart, (uint8_t *)pdata, u16Size, 10);
}

#if _NR_MICRO_SHELL_ENABLE

uint8_t data = 0;

#include "mod_event.h"
MOD_EVENT_EXTERN(tEventShell);

/* 在中断中，超时接收有问题，SYSTICK无法进入中断 */
void DEBUG_UART_IRQHandler()
{
    if (READ_BIT(DEBUG_UART->SR, USART_SR_RXNE) == USART_SR_RXNE)
    {
        WRITE_REG(DEBUG_UART->SR, ~(USART_SR_RXNE));
        data = (uint8_t)(DEBUG_UART->DR & (uint8_t)0x00FF);
        Mod_Event_Trigger(&tEventShell);
    }
}

void Dbg_Shell_Handler(void)
{
    Mod_Shell_Parse(&data, 1);
}

#endif


#ifdef PRINT
#include <stdio.h>
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&tDbgUart, (uint8_t*)&ch, 1, 10);
    return ch;
}

int fgetc(FILE *f)
{
    uint8_t ch = 0;
    HAL_UART_Receive(&tDbgUart, (uint8_t*)&ch, 1, 10);
    return (int)ch;
}
#endif
