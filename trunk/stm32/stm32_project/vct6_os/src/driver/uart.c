/**
  ******************************************************************************
  * @file    uart.c
  * @author  zuokongxiao
  * @version 
  * @date    2012-10-17
  * @brief   ������������.
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
/* ���ڽ��ջ����� */
u8 UartRxBuf[UART_BUF_SIZE+UART_EX_BUF_SIZE] = {0};
/*  ���ջ�������ȡ�±� */
u16 UartReadIndex = 0;
/* ���ջ�����дȡ�±� */
u16 UartWriteIndex = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/* External functions --------------------------------------------------------*/
/**
  * @file   void uart_configuration(void)
  * @brief  ���ô���
  * @param  None
  * @retval None
  */
void uart_configuration(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    USART_InitTypeDef       USART_InitStructure;

    USART_DeInit(USART1);
    /* ����� */
    UartReadIndex = 0;
    UartWriteIndex = 0;
    /* ʹ�ܴ���1ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
    /* ����UART1 TX��������������� -> PA9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* ����UART1 RX���Ÿ������� -> PA10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* ���ڲ�����115200 8 1 N */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    /* �ж����� */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /* ���ڽ����ж� */
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);
}

/**
  * @file   void uart_send(u8 ucUartNum, const u8 *pucSendBuf, u16 unSendLen)
  * @brief  ���ڷ���
  * @param  ucUartNum  [in] ���ں�
  * @param  pucSendBuf [in] ָ���ͻ�����
  * @param  unSendLen  [in] �������ݳ���
  * @retval None
  */
void uart_send(u8 ucUartNum, const u8 *pucSendBuf, u16 unSendLen)
{
    while(unSendLen--)
    {
        if(UART_NUM_1 == ucUartNum)
        {
            USART1->DR = ((u8) (*pucSendBuf) & (u16)0x01FF);
            /* �ȴ����ͼĴ����� */
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        }
        pucSendBuf++;
    }
}

/**
  * @file   void uart_read(void)
  * @brief  ������(�˺������ڴ����жϴ�������)
  * @param  None
  * @retval None
  */
void uart_read(void)
{
    if(SET == USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        /* �崮�ڽ����жϱ�־λ */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        /* �������� */
        UartRxBuf[UartWriteIndex] = (u8)USART_ReceiveData(USART1);
        /* ����д���� */
        UartWriteIndex++;
        if(UART_BUF_SIZE <= UartWriteIndex)
        {
            UartWriteIndex = 0;
        }
    }
}

/**
  * @file   u8 *retrieve_uart_receive_buf(void)
  * @brief  ���ڻ�����
  * @param  None
  * @retval ���ش��ڻ�������ַ
  */
u8 *retrieve_uart_receive_buf(void)
{
    return &UartRxBuf[0];
}

/**
  * @file   u16 *retrieve_uart_read_index(void)
  * @brief  ����������
  * @param  None
  * @retval ���ر�����ַ
  */
u16 *retrieve_uart_read_index(void)
{
    return &UartReadIndex;
}

/**
  * @file   u16 *retrieve_uart_write_index(void)
  * @brief  д��������
  * @param  None
  * @retval ���ر�����ַ
  */
u16 *retrieve_uart_write_index(void)
{
    return &UartWriteIndex;
}




/**************************end of file*****************************************/
