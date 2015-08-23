/*! \file  dac.c
* \b �ӿ�˵����
*
*       1. uart_init: ��ʼ������ <br>
*
*       2. uart_send: ���ڷ��ͺ��� <br>
*
*       3. get_uart_rx_buf: ��ȡ���ڽ��ջ�������ַ <br>
*
*       4. get_uart_rx_buf_write_index: ��ȡ���ڽ��ջ�����д�±� <br>
*
*       5. get_uart_rx_buf_read_index: ��ȡ���ڽ��ջ��������±� <br>
*
*       6. uart_test: ���ڲ���(�����յ�����ԭ�����ͻ�ȥ) <br>
*
* \b ��������:
*
*       ����ɨ��; USART1_TX->DMA1_CH4  USART1_RX->DMA1_CH5
*
* \b ��ʷ��¼:
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2013-03-10   �����ļ�   <br>
*/

/******************************************************************************/
/*                             ͷ�ļ�                                         */
/******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "myGPIOType.h"
#include "deftype.h"
#include "uart.h"


/******************************************************************************/
/*                             �궨��                                         */
/******************************************************************************/
#define USART1_DR_Address    ((u32)0x40013804)

/******************************************************************************/
/*                            ��������                                        */
/******************************************************************************/
/*! ����1���ͻ����� */
static uint8_t s_ucUart1TxBuf[UART1_TX_BUF_SIZE] = {0};
/*! ����1���ͻ�������д���� */
static uint16_t s_unUart1TxWriteIndex = 0, s_unUart1TxReadIndex = 0;
/*! ����1 DMA���ͻ����� */
static uint8_t s_ucUart1TxDmaBuf[UART1_TX_DMA_BUF_SIZE] = {0};
/*! ����1 DMA״̬: 0���� 1æ */
static uint8_t s_ucUart1DmaStatusFlag = 0;

/*! ����1���ջ����� */
static uint8_t s_ucUart1RxBuf[UART1_RX_BUF_SIZE] = {0};
/*! ����1���ջ�������д���� */
static uint16_t s_unUart1RxWriteIndex = 0, s_unUart1RxReadIndex = 0;

/******************************************************************************/
/*                          �ڲ��ӿ�����                                      */
/******************************************************************************/




/******************************************************************************/
/*                          �ڲ��ӿ�ʵ��                                      */
/******************************************************************************/
/*! \fn       void uart_gpio_configuration(void)
*
*  \brief     ����GPIO����
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
static void uart_gpio_configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* ����UART1 TX��������������� -> PA9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* ����UART1 RX���Ÿ������� -> PA10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*! \fn       void uart_dma_configuration(void)
*
*  \brief     ����DMA����
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
static void uart_dma_configuration(void)
{
    DMA_InitTypeDef  DMA_InitStructure;

    /*! ʹ��DMA1ʱ�� */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /* DMA1_CH4����1����ͨ�� */
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&(s_ucUart1TxDmaBuf[0]);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;//UART1_TX_DMA_BUF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    /*! ����жϱ�־λ */
    DMA_ClearITPendingBit(DMA1_IT_GL4);
    /*! ʹ��DMA1��������ж� */
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    /*! DMAͨ������ ENABLE */
    DMA_Cmd(DMA1_Channel4, DISABLE);
}

/*! \fn       int32_t uart_enable_dma_send(uint16_t unSendLen)
*
*  \brief     ʹ��DMA����
*
*  \param     unSendLen [in] DMA������ֽ���
*
*  \exception ��
*
*  \return    EXIT_SUCCESS; EXIT_FAILURE
*/
static int32_t uart_enable_dma_send(uint16_t unSendLen)
{
    int32_t lRet = EXIT_FAILURE;
    uint16_t i = 0;
    
    if((0 < unSendLen) && (UART1_TX_DMA_BUF_SIZE >= unSendLen) \
        && (0 == DMA_GetCurrDataCounter(DMA1_Channel4)))
    {
        /*! �ر�DMAͨ�� */
        DMA_Cmd(DMA1_Channel4, DISABLE);
        /*! ��Ҫ���͵����ݷ���DMA������ */
        for(i=0; i<unSendLen; i++)
        {
            s_ucUart1TxDmaBuf[i] = s_ucUart1TxBuf[s_unUart1TxReadIndex];
            s_unUart1TxReadIndex++;
            if(UART1_TX_BUF_SIZE <= s_unUart1TxReadIndex)
            {
                s_unUart1TxReadIndex = 0;
            }
        }
        /*! ����DMA�������� */
        DMA_SetCurrDataCounter(DMA1_Channel4, unSendLen);
        /*! ��DMAͨ�� */
        DMA_Cmd(DMA1_Channel4, ENABLE);
        lRet = EXIT_SUCCESS;
    }
    
    return lRet;
}

/*! \fn       void uart_nvic_configuration(void)
*
*  \brief     NVIC����
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
static void uart_nvic_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the DMA1_Channel4_IRQn Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************/
/*                          �ⲿ�ӿ�ʵ��                                      */
/******************************************************************************/
/*! \fn       void uart_init(void)
*
*  \brief     ��ʼ������1
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
void uart_init(void)
{
    USART_InitTypeDef  USART_InitStructure;

    /*! ��ʼ������ */
    s_ucUart1DmaStatusFlag = 0;
    s_unUart1TxWriteIndex = 0;
    s_unUart1TxReadIndex = 0;
    s_unUart1RxWriteIndex = 0;
    s_unUart1RxReadIndex = 0;
    
    /* ʹ�ܴ���1ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
    /*! ����GPIO���� */
    uart_gpio_configuration();
    /*! ����DMA���� */
    uart_dma_configuration();
    /*! NVIC���� */
    uart_nvic_configuration();
    
    /* ���ڲ�����115200 8 1 N */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    /*! ����жϱ�־λ */
    USART_ClearFlag(USART1, USART_FLAG_TXE|USART_FLAG_TC|USART_FLAG_RXNE);
    /*! ʹ�ܴ���DMA���� */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    /* �ж����ã����ڽ����ж�ʹ�� */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);
}

/*! \fn       int32_t uart_send(uint8_t *pucBufPtr, uint16_t unSendLen)
*
*  \brief     ���ڷ���
*
*  \param     pucBufPtr [in] ָ���ͻ�����
*
*  \param     unSendLen [in] �跢�����ݵ��ֽ���
*
*  \exception ��
*
*  \return    EXIT_SUCCESS���ɹ�; EXIT_FAILURE��ʧ��
*/
int32_t uart_send(uint8_t *pucBufPtr, uint16_t unSendLen)
{
    int32_t lRet = EXIT_FAILURE;
    uint16_t i = 0, unDataCnt = 0;
    
    do
    {
        /*! ���������� */
        if(INVALID_POINTER(pucBufPtr) \
            || (1 > unSendLen) || (UART1_TX_BUF_SIZE < unSendLen))
        {
            break;
        }
        /*! �ȴ����ͻ������������跢�͵����� */
        while(1)
        {
            /*! ���㷢�ͻ���������ռ� */
            if(s_unUart1TxWriteIndex >= s_unUart1TxReadIndex)
            {
                i = s_unUart1TxWriteIndex - s_unUart1TxReadIndex;
                i = UART1_TX_BUF_SIZE - i;
            }
            else
            {
                i = s_unUart1TxReadIndex - s_unUart1TxWriteIndex;
            }
            /*! ���ͻ����� */
            if(i >= unSendLen)
            {
                break;
            }
            /*! ���ӳ�ʱ����ֹ��ѭ�� */
        }
        /*! �������ݵ����ͻ����� */
        for(i=0; i<unSendLen; i++)
        {
            s_ucUart1TxBuf[s_unUart1TxWriteIndex] = *pucBufPtr;
            pucBufPtr++;
            s_unUart1TxWriteIndex++;
            if(UART1_TX_BUF_SIZE <= s_unUart1TxWriteIndex)
            {
                s_unUart1TxWriteIndex = 0;
            }
        }
        /*! DMA���У�ʹ��DMA���� */
        if(0 == s_ucUart1DmaStatusFlag)
        {
            if(s_unUart1TxWriteIndex == s_unUart1TxReadIndex)
            {
                /*! ���ͻ����������ݿɷ��� */
                lRet = EXIT_SUCCESS;
                break;
            }
            /*! ����1 DMA���ڴ������� */
            s_ucUart1DmaStatusFlag = 1;
            /*! ���㷢�ͻ����������ݸ�����FIFO 8�ֽڣ� */
            if(s_unUart1TxWriteIndex >= s_unUart1TxReadIndex)
            {
                unDataCnt = s_unUart1TxWriteIndex - s_unUart1TxReadIndex;
            }
            else
            {
                unDataCnt = s_unUart1TxReadIndex - s_unUart1TxWriteIndex;
                unDataCnt = UART1_TX_BUF_SIZE - unDataCnt;
            }
            /*! �����跢�͵��ֽ��� */
            if(UART1_TX_DMA_BUF_SIZE < unDataCnt)
            {
                unDataCnt = UART1_TX_DMA_BUF_SIZE;
            }
            /*! ����DMA���� */
            lRet = uart_enable_dma_send(unDataCnt);
        }
    }while(0);
    
    return lRet;
}

/*! \fn       uint8_t *get_uart_rx_buf(void)
*
*  \brief     ��ȡ���ڽ��ջ�������ַ
*
*  \param     ��
*
*  \exception ��
*
*  \return    ���ջ�������ַ
*/
uint8_t *get_uart_rx_buf(void)
{
    return &s_ucUart1RxBuf[0];
}

/*! \fn       uint16_t *get_uart_rx_buf_read_index(void)
*
*  \brief     ��ȡ���ڽ��ջ������Ķ��±�
*
*  \param     ��
*
*  \exception ��
*
*  \return    ���ջ������Ķ�����
*/
uint16_t *get_uart_rx_buf_read_index(void)
{
    return &s_unUart1RxReadIndex;
}

/*! \fn       uint16_t *get_uart_rx_buf_write_index(void)
*
*  \brief     ��ȡ���ڽ��ջ�������д�±�
*
*  \param     ��
*
*  \exception ��
*
*  \return    ���ջ�������д����
*/
uint16_t *get_uart_rx_buf_write_index(void)
{
    return &s_unUart1RxWriteIndex;
}

/*! \fn       void uart_test(void)
*
*  \brief     ���ڲ���(�����յ�����ԭ�����ͻ�ȥ)
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
void uart_test(void)
{
    /*! �жϴ��ڻ������Ƿ������� */
    while(s_unUart1RxWriteIndex != s_unUart1RxReadIndex)
    {
        uart_send(&(s_ucUart1RxBuf[s_unUart1RxReadIndex]), 1);
        /* ���¶����� */
        s_unUart1RxReadIndex++;
        if(UART1_RX_BUF_SIZE <= s_unUart1RxReadIndex)
        {
            s_unUart1RxReadIndex = 0;
        }
    }
}


/******************************************************************************/
/*                          �жϴ�������                                      */
/******************************************************************************/
/*! \fn       void USART1_IRQHandler(void)
*
*  \brief     ����1�жϴ�������
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
void USART1_IRQHandler(void)
{
    /*! �ж��Ƿ�Ϊ�����ж� */
    if(SET == USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        /* �崮�ڽ����жϱ�־λ */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        /* �������� */
        s_ucUart1RxBuf[s_unUart1RxWriteIndex] = (u8)USART_ReceiveData(USART1);
        /* ����д���� */
        s_unUart1RxWriteIndex++;
        if(UART1_RX_BUF_SIZE <= s_unUart1RxWriteIndex)
        {
            s_unUart1RxWriteIndex = 0;
        }
    }
}

/*! \fn       void DMA1_Channel4_IRQHandler(void)
*
*  \brief     DMA1_CH4�жϴ�������
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
void DMA1_Channel4_IRQHandler(void)
{
    uint16_t unDataCnt = 0;
    
    /*! �ж��ж����� */
    if(SET == DMA_GetITStatus(DMA1_IT_TC4))
    {
        /*! ����жϱ�־λ */
        DMA_ClearITPendingBit(DMA1_IT_GL4);
        /*! �жϷ��ͻ������Ƿ��������跢�� */
        if(s_unUart1TxWriteIndex == s_unUart1TxReadIndex)
        {
            /*! ����1 DMA���� */
            s_ucUart1DmaStatusFlag = 0;
            /*! */
        }
        else
        {
            /*! ����1 DMA���ڴ������� */
            s_ucUart1DmaStatusFlag = 1;
            /*! ���㷢�ͻ����������ݸ�����FIFO 8�ֽڣ� */
            if(s_unUart1TxWriteIndex >= s_unUart1TxReadIndex)
            {
                unDataCnt = s_unUart1TxWriteIndex - s_unUart1TxReadIndex;
            }
            else
            {
                unDataCnt = s_unUart1TxReadIndex - s_unUart1TxWriteIndex;
                unDataCnt = UART1_TX_BUF_SIZE - unDataCnt;
            }
            /*! �����跢�͵��ֽ��� */
            if(UART1_TX_DMA_BUF_SIZE < unDataCnt)
            {
                unDataCnt = UART1_TX_DMA_BUF_SIZE;
            }
            /*! ����DMA���� */
            uart_enable_dma_send(unDataCnt);
        }
    }
    else
    {
        /*! ����жϱ�־λ */
        DMA_ClearITPendingBit(DMA1_IT_GL4);
    }
}


/********************************end of file***********************************/
