/**
  ******************************************************************************
  * @file    uart.c
  * @author  zuokongxiao
  * @version 
  * @date    2012-10-17
  * @brief   串口驱动函数.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* 串口接收缓冲区 */
u8 UartRxBuf[UART_BUF_SIZE+UART_EX_BUF_SIZE] = {0};
/*  接收缓冲区读取下标 */
u16 UartReadIndex = 0;
/* 接收缓冲区写取下标 */
u16 UartWriteIndex = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/* External functions --------------------------------------------------------*/
/**
  * @file   void uart_configuration(void)
  * @brief  配置窗口
  * @param  None
  * @retval None
  */
void uart_configuration(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    USART_InitTypeDef       USART_InitStructure;

    USART_DeInit(USART1);
    /* 清变量 */
    UartReadIndex = 0;
    UartWriteIndex = 0;
    /* 使能串口1时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
    /* 配置UART1 TX引脚上拉复用输出 -> PA9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* 配置UART1 RX引脚浮空输入 -> PA10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* 串口参数：115200 8 1 N */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    /* 中断设置 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /* 串口接收中断 */
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);
}

/**
  * @file   void uart_send(u8 ucUartNum, const u8 *pucSendBuf, u16 unSendLen)
  * @brief  串口发送
  * @param  ucUartNum  [in] 串口号
  * @param  pucSendBuf [in] 指向发送缓冲区
  * @param  unSendLen  [in] 发送数据长度
  * @retval None
  */
void uart_send(u8 ucUartNum, const u8 *pucSendBuf, u16 unSendLen)
{
    while(unSendLen--)
    {
        if(UART_NUM_1 == ucUartNum)
        {
            USART1->DR = ((u8) (*pucSendBuf) & (u16)0x01FF);
            /* 等待发送寄存器空 */
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        }
        pucSendBuf++;
    }
}

/**
  * @file   void uart_read(void)
  * @brief  读串口(此函数用于串口中断处理函数)
  * @param  None
  * @retval None
  */
void uart_read(void)
{
    if(SET == USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        /* 清串口接收中断标志位 */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        /* 读缓冲区 */
        UartRxBuf[UartWriteIndex] = (u8)USART_ReceiveData(USART1);
        /* 更新写索引 */
        UartWriteIndex++;
        if(UART_BUF_SIZE <= UartWriteIndex)
        {
            UartWriteIndex = 0;
        }
    }
}

/**
  * @file   u8 *retrieve_uart_receive_buf(void)
  * @brief  串口缓冲区
  * @param  None
  * @retval 返回串口缓冲区地址
  */
u8 *retrieve_uart_receive_buf(void)
{
    return &UartRxBuf[0];
}

/**
  * @file   u16 *retrieve_uart_read_index(void)
  * @brief  读串口索引
  * @param  None
  * @retval 返回变量地址
  */
u16 *retrieve_uart_read_index(void)
{
    return &UartReadIndex;
}

/**
  * @file   u16 *retrieve_uart_write_index(void)
  * @brief  写串口索引
  * @param  None
  * @retval 返回变量地址
  */
u16 *retrieve_uart_write_index(void)
{
    return &UartWriteIndex;
}




/**************************end of file*****************************************/

