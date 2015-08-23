/*! \file  dac.c
* \b 接口说明：
*
*       1. uart_init: 初始化串口 <br>
*
*       2. uart_send: 串口发送函数 <br>
*
*       3. get_uart_rx_buf: 获取串口接收缓冲区地址 <br>
*
*       4. get_uart_rx_buf_write_index: 获取串口接收缓冲区写下标 <br>
*
*       5. get_uart_rx_buf_read_index: 获取串口接收缓冲区读下标 <br>
*
*       6. uart_test: 串口测试(将接收到数据原样发送回去) <br>
*
* \b 功能描述:
*
*       按键扫描; USART1_TX->DMA1_CH4  USART1_RX->DMA1_CH5
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-03-10   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "myGPIOType.h"
#include "deftype.h"
#include "uart.h"


/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
#define USART1_DR_Address    ((u32)0x40013804)

/******************************************************************************/
/*                            变量定义                                        */
/******************************************************************************/
/*! 串口1发送缓冲区 */
static uint8_t s_ucUart1TxBuf[UART1_TX_BUF_SIZE] = {0};
/*! 串口1发送缓冲区读写索引 */
static uint16_t s_unUart1TxWriteIndex = 0, s_unUart1TxReadIndex = 0;
/*! 串口1 DMA发送缓冲区 */
static uint8_t s_ucUart1TxDmaBuf[UART1_TX_DMA_BUF_SIZE] = {0};
/*! 串口1 DMA状态: 0空闲 1忙 */
static uint8_t s_ucUart1DmaStatusFlag = 0;

/*! 串口1接收缓冲区 */
static uint8_t s_ucUart1RxBuf[UART1_RX_BUF_SIZE] = {0};
/*! 串口1接收缓冲区读写索引 */
static uint16_t s_unUart1RxWriteIndex = 0, s_unUart1RxReadIndex = 0;

/******************************************************************************/
/*                          内部接口声明                                      */
/******************************************************************************/




/******************************************************************************/
/*                          内部接口实现                                      */
/******************************************************************************/
/*! \fn       void uart_gpio_configuration(void)
*
*  \brief     串口GPIO配置
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
static void uart_gpio_configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* 配置UART1 TX引脚上拉复用输出 -> PA9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* 配置UART1 RX引脚浮空输入 -> PA10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*! \fn       void uart_dma_configuration(void)
*
*  \brief     串口DMA配置
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
static void uart_dma_configuration(void)
{
    DMA_InitTypeDef  DMA_InitStructure;

    /*! 使能DMA1时钟 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /* DMA1_CH4串口1发送通道 */
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
    /*! 清除中断标志位 */
    DMA_ClearITPendingBit(DMA1_IT_GL4);
    /*! 使能DMA1传输完成中断 */
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    /*! DMA通道控制 ENABLE */
    DMA_Cmd(DMA1_Channel4, DISABLE);
}

/*! \fn       int32_t uart_enable_dma_send(uint16_t unSendLen)
*
*  \brief     使能DMA传输
*
*  \param     unSendLen [in] DMA传输的字节数
*
*  \exception 无
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
        /*! 关闭DMA通道 */
        DMA_Cmd(DMA1_Channel4, DISABLE);
        /*! 将要发送到数据放入DMA缓冲区 */
        for(i=0; i<unSendLen; i++)
        {
            s_ucUart1TxDmaBuf[i] = s_ucUart1TxBuf[s_unUart1TxReadIndex];
            s_unUart1TxReadIndex++;
            if(UART1_TX_BUF_SIZE <= s_unUart1TxReadIndex)
            {
                s_unUart1TxReadIndex = 0;
            }
        }
        /*! 设置DMA传输数量 */
        DMA_SetCurrDataCounter(DMA1_Channel4, unSendLen);
        /*! 打开DMA通道 */
        DMA_Cmd(DMA1_Channel4, ENABLE);
        lRet = EXIT_SUCCESS;
    }
    
    return lRet;
}

/*! \fn       void uart_nvic_configuration(void)
*
*  \brief     NVIC配置
*
*  \param     无
*
*  \exception 无
*
*  \return    无
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
/*                          外部接口实现                                      */
/******************************************************************************/
/*! \fn       void uart_init(void)
*
*  \brief     初始化串口1
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void uart_init(void)
{
    USART_InitTypeDef  USART_InitStructure;

    /*! 初始化变量 */
    s_ucUart1DmaStatusFlag = 0;
    s_unUart1TxWriteIndex = 0;
    s_unUart1TxReadIndex = 0;
    s_unUart1RxWriteIndex = 0;
    s_unUart1RxReadIndex = 0;
    
    /* 使能串口1时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
    /*! 串口GPIO配置 */
    uart_gpio_configuration();
    /*! 串口DMA配置 */
    uart_dma_configuration();
    /*! NVIC配置 */
    uart_nvic_configuration();
    
    /* 串口参数：115200 8 1 N */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    /*! 清除中断标志位 */
    USART_ClearFlag(USART1, USART_FLAG_TXE|USART_FLAG_TC|USART_FLAG_RXNE);
    /*! 使能串口DMA功能 */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    /* 中断设置：串口接收中断使能 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);
}

/*! \fn       int32_t uart_send(uint8_t *pucBufPtr, uint16_t unSendLen)
*
*  \brief     串口发送
*
*  \param     pucBufPtr [in] 指向发送缓冲区
*
*  \param     unSendLen [in] 需发送数据的字节数
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功; EXIT_FAILURE：失败
*/
int32_t uart_send(uint8_t *pucBufPtr, uint16_t unSendLen)
{
    int32_t lRet = EXIT_FAILURE;
    uint16_t i = 0, unDataCnt = 0;
    
    do
    {
        /*! 输入参数检查 */
        if(INVALID_POINTER(pucBufPtr) \
            || (1 > unSendLen) || (UART1_TX_BUF_SIZE < unSendLen))
        {
            break;
        }
        /*! 等待发送缓冲区能容纳需发送的数据 */
        while(1)
        {
            /*! 计算发送缓冲区空余空间 */
            if(s_unUart1TxWriteIndex >= s_unUart1TxReadIndex)
            {
                i = s_unUart1TxWriteIndex - s_unUart1TxReadIndex;
                i = UART1_TX_BUF_SIZE - i;
            }
            else
            {
                i = s_unUart1TxReadIndex - s_unUart1TxWriteIndex;
            }
            /*! 发送缓冲区 */
            if(i >= unSendLen)
            {
                break;
            }
            /*! 添加超时，防止死循环 */
        }
        /*! 复制数据到发送缓冲区 */
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
        /*! DMA空闲，使能DMA传输 */
        if(0 == s_ucUart1DmaStatusFlag)
        {
            if(s_unUart1TxWriteIndex == s_unUart1TxReadIndex)
            {
                /*! 发送缓冲区无数据可发送 */
                lRet = EXIT_SUCCESS;
                break;
            }
            /*! 串口1 DMA正在传输数据 */
            s_ucUart1DmaStatusFlag = 1;
            /*! 计算发送缓冲区的数据个数（FIFO 8字节） */
            if(s_unUart1TxWriteIndex >= s_unUart1TxReadIndex)
            {
                unDataCnt = s_unUart1TxWriteIndex - s_unUart1TxReadIndex;
            }
            else
            {
                unDataCnt = s_unUart1TxReadIndex - s_unUart1TxWriteIndex;
                unDataCnt = UART1_TX_BUF_SIZE - unDataCnt;
            }
            /*! 计算需发送的字节数 */
            if(UART1_TX_DMA_BUF_SIZE < unDataCnt)
            {
                unDataCnt = UART1_TX_DMA_BUF_SIZE;
            }
            /*! 启用DMA传输 */
            lRet = uart_enable_dma_send(unDataCnt);
        }
    }while(0);
    
    return lRet;
}

/*! \fn       uint8_t *get_uart_rx_buf(void)
*
*  \brief     获取串口接收缓冲区地址
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区地址
*/
uint8_t *get_uart_rx_buf(void)
{
    return &s_ucUart1RxBuf[0];
}

/*! \fn       uint16_t *get_uart_rx_buf_read_index(void)
*
*  \brief     获取串口接收缓冲区的读下标
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区的读索引
*/
uint16_t *get_uart_rx_buf_read_index(void)
{
    return &s_unUart1RxReadIndex;
}

/*! \fn       uint16_t *get_uart_rx_buf_write_index(void)
*
*  \brief     获取串口接收缓冲区的写下标
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区的写索引
*/
uint16_t *get_uart_rx_buf_write_index(void)
{
    return &s_unUart1RxWriteIndex;
}

/*! \fn       void uart_test(void)
*
*  \brief     串口测试(将接收到数据原样发送回去)
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void uart_test(void)
{
    /*! 判断串口缓冲区是否有数据 */
    while(s_unUart1RxWriteIndex != s_unUart1RxReadIndex)
    {
        uart_send(&(s_ucUart1RxBuf[s_unUart1RxReadIndex]), 1);
        /* 更新读索引 */
        s_unUart1RxReadIndex++;
        if(UART1_RX_BUF_SIZE <= s_unUart1RxReadIndex)
        {
            s_unUart1RxReadIndex = 0;
        }
    }
}


/******************************************************************************/
/*                          中断处理函数                                      */
/******************************************************************************/
/*! \fn       void USART1_IRQHandler(void)
*
*  \brief     串口1中断处理函数
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void USART1_IRQHandler(void)
{
    /*! 判断是否为接收中断 */
    if(SET == USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        /* 清串口接收中断标志位 */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        /* 读缓冲区 */
        s_ucUart1RxBuf[s_unUart1RxWriteIndex] = (u8)USART_ReceiveData(USART1);
        /* 更新写索引 */
        s_unUart1RxWriteIndex++;
        if(UART1_RX_BUF_SIZE <= s_unUart1RxWriteIndex)
        {
            s_unUart1RxWriteIndex = 0;
        }
    }
}

/*! \fn       void DMA1_Channel4_IRQHandler(void)
*
*  \brief     DMA1_CH4中断处理函数
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void DMA1_Channel4_IRQHandler(void)
{
    uint16_t unDataCnt = 0;
    
    /*! 判断中断类型 */
    if(SET == DMA_GetITStatus(DMA1_IT_TC4))
    {
        /*! 清除中断标志位 */
        DMA_ClearITPendingBit(DMA1_IT_GL4);
        /*! 判断发送缓冲区是否有数据需发送 */
        if(s_unUart1TxWriteIndex == s_unUart1TxReadIndex)
        {
            /*! 串口1 DMA空闲 */
            s_ucUart1DmaStatusFlag = 0;
            /*! */
        }
        else
        {
            /*! 串口1 DMA正在传输数据 */
            s_ucUart1DmaStatusFlag = 1;
            /*! 计算发送缓冲区的数据个数（FIFO 8字节） */
            if(s_unUart1TxWriteIndex >= s_unUart1TxReadIndex)
            {
                unDataCnt = s_unUart1TxWriteIndex - s_unUart1TxReadIndex;
            }
            else
            {
                unDataCnt = s_unUart1TxReadIndex - s_unUart1TxWriteIndex;
                unDataCnt = UART1_TX_BUF_SIZE - unDataCnt;
            }
            /*! 计算需发送的字节数 */
            if(UART1_TX_DMA_BUF_SIZE < unDataCnt)
            {
                unDataCnt = UART1_TX_DMA_BUF_SIZE;
            }
            /*! 启用DMA传输 */
            uart_enable_dma_send(unDataCnt);
        }
    }
    else
    {
        /*! 清除中断标志位 */
        DMA_ClearITPendingBit(DMA1_IT_GL4);
    }
}


/********************************end of file***********************************/

