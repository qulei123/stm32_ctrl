/**  
 *   文件名称：drv_ctrl_uart.c
 *   摘要：中断/DMA接收数据
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.04.01   v1      ql     创建文件
 *   --------------------------------------------------
 */

#include "drv_uart.h"


static UART_HandleTypeDef tCtrl1Uart;


int Drv_Ctrl1_Uart_Init(uint32_t u32Bps)
{    
    UART_HandleTypeDef *ptUart = &tCtrl1Uart;
    
    ptUart->Instance        = CTRL1_UART;
    ptUart->Init.BaudRate   = u32Bps;
    ptUart->Init.WordLength = USART_WORDLENGTH_8B;
    ptUart->Init.StopBits   = USART_STOPBITS_1;
    ptUart->Init.Parity     = USART_PARITY_NONE;
    ptUart->Init.Mode       = USART_MODE_TX_RX;
    ptUart->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    
    if (HAL_UART_Init(ptUart) != HAL_OK)
    {
        log_e("ctrl1 init\n");
        return DRV_ERR;
    }

    return DRV_OK;
}

uint16_t Drv_Ctrl1_Send(uint8_t *pdata, uint16_t u16Size)
{
    UART_HandleTypeDef *ptUart = &tCtrl1Uart;
    HAL_StatusTypeDef State;
    
    State = HAL_UART_Transmit(ptUart, pdata, u16Size, CTRL_TX_TIMEOUT);
    if (HAL_OK != State)
    {
        log_e("ctrl1 send eno[%d]\n", State);
        return 0;
    }

    return u16Size - ptUart->TxXferCount;
}


uint16_t Drv_Ctrl1_Recv(uint8_t *pdata, uint16_t u16Size)
{
    UART_HandleTypeDef *ptUart = &tCtrl1Uart;
    HAL_StatusTypeDef State;
    uint16_t u16RxLen = 0;

#if 0
    /* 阻塞接收u16Size个数据后返回 */
    State = HAL_UART_Receive(ptUart, pdata, u16Size, CTRL_RX_TIMEOUT);
    if (HAL_OK != State)
    {
        log_e("ctrl1 recv eno[%d]\n", State);
        return 0;
    }
    
    u16RxLen =  u16Size - ptUart->RxXferCount;
#else
    /* 阻塞接收小于u16Size个数据后返回, 没有接收到数据时, 阻塞 CTRL_RX_TIMEOUT */
    State = HAL_UARTEx_ReceiveToIdle(ptUart, pdata, u16Size, &u16RxLen, CTRL_RX_TIMEOUT);
    if (HAL_OK != State)
    {
        log_e("ctrl1 recv eno[%d]\n", State);
        return 0;
    }
#endif
    
    return u16RxLen;
}

