/**  
 *   文件名称：drv_commt_uart.c
 *   摘要：中断/DMA接收数据
 *  
 *   修改历史        版本号     Author  修改内容
 *   --------------------------------------------------
 *   2022.03.28   v1      ql     创建文件
 *   --------------------------------------------------
 */
#include "drv_uart.h"

/* 下步优化：
 * 1. 使用DMA发送接收
 * 2. 使用hal库处理中断比较耗时
 */

#define RX_BUF_SIZE     32


PIPO_INSTANCE(Uart, RX_BUF_SIZE);
static UART_HandleTypeDef tCommtUart;


/* 该函数会在中断回调函数中调用，不要添加打印 */
static int Commt_Recv_IT_Enble(void)
{
    uint8_t *pRxBuf = Pipo_Get_Buf_Inc_Idx(Uart);
    
    /* HAL_UART_Receive_IT / HAL_UARTEx_ReceiveToIdle_IT */
    if(HAL_UARTEx_ReceiveToIdle_IT(&tCommtUart, pRxBuf, RX_BUF_SIZE) != HAL_OK)
    {
        return DRV_ERR;
    }

    return DRV_OK;
}

int Drv_Commt_Uart_Init(uint32_t u32Bps)
{
    UART_HandleTypeDef *ptUart = &tCommtUart;
    
    ptUart->Instance         = COMMT_UART;
    ptUart->Init.BaudRate    = u32Bps;
    ptUart->Init.WordLength  = USART_WORDLENGTH_8B;
    ptUart->Init.StopBits    = USART_STOPBITS_1;
    ptUart->Init.Parity      = USART_PARITY_NONE;
    ptUart->Init.Mode        = USART_MODE_TX_RX;
    ptUart->Init.HwFlowCtl   = UART_HWCONTROL_NONE;
    
    if (HAL_UART_Init(ptUart) != HAL_OK)
    {
        log_e("cmt Init");
        return DRV_ERR;
    }
    
    if(Commt_Recv_IT_Enble() != DRV_OK)
    {
        return DRV_ERR;
    }
    
    return DRV_OK;
}

uint16_t Drv_Commt_Send(uint8_t *pdata, uint16_t u16Size)
{
    UART_HandleTypeDef *ptUart = &tCommtUart;
    HAL_StatusTypeDef State;
    
    State = HAL_UART_Transmit(ptUart, pdata, u16Size, COMMT_TX_TIMEOUT);
    if (HAL_OK != State)
    {
        log_e("cmt send eno[%d]\n", State);
        return 0;
    }
    
    return u16Size - ptUart->TxXferCount;
}

/* 中断接收, 该函数没有使用 */
uint16_t Drv_Commt_Recv(uint8_t *pdata, uint16_t u16Size)
{
    UART_HandleTypeDef *ptUart = &tCommtUart;
    HAL_StatusTypeDef State;
    uint16_t u16RxLen = 0;

#if 0
    /* 阻塞接收u16Size个数据后返回 */
    State = HAL_UART_Receive(ptUart, pdata, u16Size, TIMEOUT_MAX);
    if (HAL_OK != State)
    {
        log_e("cmt recv eno[%d]\n", State);
        return 0;
    }
    
    u16RxLen =  u16Size - ptUart->RxXferCount;
#else
    /* 阻塞接收小于u16Size个数据后返回, 没有接收到数据时, 阻塞 CTRL_RX_TIMEOUT */
    State = HAL_UARTEx_ReceiveToIdle(ptUart, pdata, u16Size, &u16RxLen, COMMT_RX_TIMEOUT);
    if (HAL_OK != State)
    {
        log_e("cmt recv eno[%d]\n", State);
        return 0;
    }
#endif
    
    return u16RxLen;
}

void COMMT_UART_IRQHandler(void)
{
    
    HAL_UART_IRQHandler(&tCommtUart);
    //log("U");
}

#include "mod_event.h"
MOD_EVENT_EXTERN(tEventDoCmd);

/* HAL_UART_RECEPTION_TOIDLE, 触发RxEvent时, 接收数据的长度可变 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    log_hex(huart->pRxBuffPtr - Size, Size);
    Pipo_Set_Len(Uart, Size);
    Mod_Event_Trigger(&tEventDoCmd);
    Commt_Recv_IT_Enble();
}

/* 错误中断怎么处理？ */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    log_e("Int err[0x%08X]\n", huart->ErrorCode);
    Commt_Recv_IT_Enble();
}

#if 0
/* HAL_UART_RECEPTION_STANDARD, 触发RxCplt时, 接收数据的长度固定 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}
#endif

