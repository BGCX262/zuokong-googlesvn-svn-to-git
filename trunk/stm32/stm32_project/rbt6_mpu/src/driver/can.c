/*! \file  can.c
* \b 接口说明：
*
*       1. can_init: 初始化CAN <br>
*
*       2. can_send: CAN发送数据 <br>
*
*       3. get_can_rx_buf: 获取CAN接收缓冲区地址 <br>
*
*       4. get_can_rx_buf_read_index: 获取CAN接收缓冲区的读下标 <br>
*
*       5. get_can_rx_buf_write_index: 获取CAN接收缓冲区的写下标 <br>
*
* \b 功能描述:
*
*       stm32f103 CAN总线驱动
*
* \b 历史记录:
*
*       <作者>          <时间>       <修改记录> <br>
*       zuokongxiao    2013-05-19    创建该文件 <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "deftype.h"
#include "can.h"
#include "debug.h"
#include "led.h"
#include "protocol.h"


/******************************************************************************/
/*                               宏定义                                       */
/******************************************************************************/
/*! 此驱动程序使用CAN的标识符为11位 */
#define CAN_ID_1        0x17A
#define CAN_ID_2        0x275
#define CAN_ID_3        0x36F
#define CAN_ID_4        0x46B

/*! CAN报文缓冲区大小 */
#define CAN_MSG_BUF_SIZE        (CAN1_TX_BUF_SIZE/8)
/*! CAN报文标识符定义 */
/*! 源节点地址mask */
#define SRC_ID_MASK             0x07C00000
/*! 目标节点地址mask */
#define DEST_ID_MASK            0x001F0000
/*! 分段码mask */
#define SEG_CODE_MASK           0x0000FF00
/*! 分段标志mask */
#define SEG_CODE_FLAG_MASK      0x0000C000
/*! 分段号mask */
#define SEG_CODE_NUM_MASK       0x00003F00
/*! 分段标志 */
#define FRAME_NONE_SEG          0x00000000 /*!< 无分段 */
#define FRAME_FIRST_SEG         0x00004000 /*!< 第一个分段 */
#define FRAME_MIDDLE_SEG        0x00008000 /*!< 中间分段 */
#define FRAME_END_SEG           0x0000C000 /*!< 最后分段 */

/******************************************************************************/
/*                              结构定义                                      */
/******************************************************************************/

/******************************************************************************/
/*                             变量定义                                       */
/******************************************************************************/
/*! CAN1发送缓冲区 */
static uint8_t s_ucCan1TxBuf[CAN1_TX_BUF_SIZE] = {0};
/*! CAN1发送缓冲区读写索引 */
static uint16_t s_unCan1TxWriteIndex = 0, s_unCan1TxReadIndex = 0;
/*! CAN报文发送缓冲区 */
static CanTxMsg s_astCanMsgTxBuf[CAN_MSG_BUF_SIZE] = {0};
/*! CAN报文发送读写索引 */
static volatile uint16_t s_unCanMsgTxWrIndex = 0, s_unCanMsgTxRdIndex = 0;

/*! CAN1接收缓冲区 */
static uint8_t s_ucCan1RxBuf[CAN1_RX_BUF_SIZE] = {0};
/*! CAN1接收缓冲区读写索引 */
static uint16_t s_unCan1RxWriteIndex = 0, s_unCan1RxReadIndex = 0;


/*! 帧缓冲区（分段报文重组缓冲区） */
static uint8 s_aucCanSegMsgBuf[CAN_MSG_BUF_SIZE][64] = {0};
/*! 帧缓冲区读写索引 */
static uint16_t s_unCanSegMsgWriteIndex = 0, s_unCanSegMsgReadIndex = 0;
/*! 帧个数 */
static uint8 s_ucCanSegMsgCnt = 0;
/*! 上一次分段号 */
static uint8 s_ucLastSegNum = 0;


/******************************************************************************/
/*                            内部接口声明                                    */
/******************************************************************************/




/******************************************************************************/
/*                            内部接口实现                                    */
/******************************************************************************/
/*! \fn       static void can_gpio_configuration(void)
*
*  \brief     CAN总线GPIO配置
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
static void can_gpio_configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* 配置CAN TX引脚上拉复用输出 -> PB9(重映射) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* 配置CAN RX引脚上拉输入 -> PB8(重映射) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /*! 端口重映射PA12、PA11->PB9、PB8 */
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}

/*! \fn       static void can_nvic_configuration(void)
*
*  \brief     NVIC配置
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
static void can_nvic_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* CAN1发送中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* CAN1 FIFO0接收中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#if 0
    /* CAN1 FIFO1接收中断 */
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* CAN1错误中断 */
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

/*! \fn       static int32_t send_can_msg(void)
*
*  \brief     发送CAN消息
*
*  \param     无
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功; EXIT_FAILURE：失败
*/
static int32_t send_can_msg(void)
{
    int32_t lRet = EXIT_FAILURE;

    do
    {
        if(s_unCanMsgTxWrIndex == s_unCanMsgTxRdIndex)
        {
            /*! 发送缓冲区无数据可发送 */
            NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
            lRet = EXIT_SUCCESS;
            break;
        }
        /*! can发送消息 */
        if(CAN_TxStatus_NoMailBox == CAN_Transmit(CAN1, &(s_astCanMsgTxBuf[s_unCanMsgTxRdIndex])))
        {
            //DEBUG_MSG("N: Can send mailbox full! %s %d\r\n", __FILE__, __LINE__);
        }
        else
        {
            /*! 发送成功，改变发送缓冲区的读索引 */
            s_unCanMsgTxRdIndex++;
            if(CAN_MSG_BUF_SIZE <= s_unCanMsgTxRdIndex)
            {
                s_unCanMsgTxRdIndex = 0;
            }
        }
        lRet = EXIT_SUCCESS;
    }while(0);

    return lRet;
}

/*! \fn       static void receive_can_msg(void)
*
*  \brief     接收CAN消息
*
*  \param     无
*
*  \exception 此函数只在接受中断中使用
*
*  \return    无
*/
static void receive_can_msg(void)
{
    uint8 i = 0;
    CanRxMsg stCanMsg = {0};
    uint8 ucSrcId = 0, ucDestId = 0, ucSegNum = 0;

    do
    {
        /*! CAN接收消息 */
        CAN_Receive(CAN1, CAN_FIFO0, &stCanMsg);
        LED2 = ~LED2;
        /*! 帧判断 */
        if((CAN_Id_Extended != stCanMsg.IDE) \
            || (CAN_RTR_Data != stCanMsg.RTR) 
            || (1 > stCanMsg.DLC))
        {
            break;
        }
        /*! 标识符 */
        ucSrcId = (uint8)((stCanMsg.ExtId & SRC_ID_MASK)>>22);
        ucDestId = (uint8)((stCanMsg.ExtId & DEST_ID_MASK)>>16);
        ucSegNum = (uint8)((stCanMsg.ExtId & SEG_CODE_NUM_MASK)>>8);
#if 1
        if((1 != ucSrcId) || (0 != ucDestId))
        {
            break;
        }
#endif
        if(8 < stCanMsg.DLC)
        {
            stCanMsg.DLC = 8;
        }
        /*! 数据 */
        if(FRAME_NONE_SEG == (stCanMsg.ExtId & SEG_CODE_FLAG_MASK))
        {
            /*! 无分段，直接将数据复制到帧接收缓冲区 */
            for(i=0; i<stCanMsg.DLC; i++)
            {
                s_aucCanSegMsgBuf[s_unCanSegMsgWriteIndex][i] = stCanMsg.Data[i];
            }
            s_unCanSegMsgWriteIndex++;
            if(CAN_MSG_BUF_SIZE <= s_unCanSegMsgWriteIndex)
            {
                s_unCanSegMsgWriteIndex = 0;
            }
        }
        else if(FRAME_FIRST_SEG == (stCanMsg.ExtId & SEG_CODE_FLAG_MASK))
        {
            if((0 != ucSegNum) || (8 != stCanMsg.DLC))
            {
                break;
            }
            s_ucCanSegMsgCnt = 0;
            for(i=0; i<8; i++)
            {
                s_aucCanSegMsgBuf[s_unCanSegMsgWriteIndex][s_ucCanSegMsgCnt] = stCanMsg.Data[i];
                s_ucCanSegMsgCnt++;
            }
            s_ucLastSegNum = ucSegNum;
        }
        else if(FRAME_MIDDLE_SEG == (stCanMsg.ExtId & SEG_CODE_FLAG_MASK))
        {
            s_ucLastSegNum++;
            if((s_ucLastSegNum != ucSegNum) || (1 > ucSegNum) || (8 != stCanMsg.DLC))
            {
                s_ucLastSegNum = 0xff;
                break;
            }
            for(i=0; i<8; i++)
            {
                s_aucCanSegMsgBuf[s_unCanSegMsgWriteIndex][s_ucCanSegMsgCnt] = stCanMsg.Data[i];
                s_ucCanSegMsgCnt++;
            }
            s_ucLastSegNum = ucSegNum;
        }
        else if(FRAME_END_SEG == (stCanMsg.ExtId & SEG_CODE_FLAG_MASK))
        {
            if((0 != ucSegNum) || (0xff == s_ucLastSegNum))
            {
                break;
            }
            for(i=0; i<stCanMsg.DLC; i++)
            {
                s_aucCanSegMsgBuf[s_unCanSegMsgWriteIndex][s_ucCanSegMsgCnt] = stCanMsg.Data[i];
                s_ucCanSegMsgCnt++;
            }
            s_unCanSegMsgWriteIndex++;
            if(CAN_MSG_BUF_SIZE <= s_unCanSegMsgWriteIndex)
            {
                s_unCanSegMsgWriteIndex = 0;
            }
        }
        else
        {
            break;
        }
    }while(0);
}

/******************************************************************************/
/*                                外部接口                                    */
/******************************************************************************/
/*! \fn       void can_init(void)
*
*  \brief     初始化CAN1总线
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void can_init(void)
{
    CAN_InitTypeDef CAN_InitStructure = {0};
    CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};

    /*! 使能CAN1时钟(最大36MHz) */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);
    /*! 配置CAN1端口 */
    can_gpio_configuration();
    /*! 配置NVIC */
    can_nvic_configuration();
    
    /* 初始化CAN寄存器 */
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);
    /* CAN cell init */
    /*! 时间触发通信模式 */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    /*! 离线恢复：ENABLE开启自动恢复 */
    CAN_InitStructure.CAN_ABOM = DISABLE;
    /*! 自动唤醒模式 */
    CAN_InitStructure.CAN_AWUM = DISABLE;
    /*! 禁止自动重传模式：ENABLE使能 */
    CAN_InitStructure.CAN_NART = DISABLE;
    /*! 接收FIFO锁定模式 */
    CAN_InitStructure.CAN_RFLM = DISABLE;
    /*! 发送优先级：ENABLE由发送请求次序决定 */
    CAN_InitStructure.CAN_TXFP = ENABLE;
    /*! 模式 */
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_Silent_LoopBack; //CAN_Mode_Normal;
    /* CAN Baudrate = 500KBps = 36MHz/(1*SJW+BS1+BS2) */
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler = 12;
    CAN_Init(CAN1, &CAN_InitStructure);

    /*! 报文滤波器设置 */
#if 0
    /*! 过滤器组 */
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    /*!屏蔽位模式和标识符列表模式 */
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
    /*! 过滤器组位宽：16位时可以设置4个ID */
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
    /*! ID */
    CAN_FilterInitStructure.CAN_FilterIdHigh = (CAN_ID_1<<5) & 0xFFE0; //FR2L
    CAN_FilterInitStructure.CAN_FilterIdLow = (CAN_ID_2<<5) & 0xFFE0; //FR1L
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (CAN_ID_3<<5) & 0xFFE0; //FR2H
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = (CAN_ID_4<<5) & 0xFFE0; //FR1H
    /*! FIFO */
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
#else
    /*! 过滤器组 */
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    /*!屏蔽位模式和标识符列表模式 */
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    /*! 过滤器组位宽 */
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    /*! ID */
    CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)((0x00400000<<3)>>16);
    CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)(0x00400000<<3);
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//0x0000;//(uint16_t)((0xFFFF00FF<<3)>>16);
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;//0x0000;//(uint16_t)(0xFFFF00FF<<3);
    /*! FIFO */
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
#endif

    /*! CAN1中断设置 */
    /*! CAN_IT_TME：发送邮箱空中断使能 */
    /*! CAN_IT_FMP0：FIFO0消息挂号中断使能 */
    /*! CAN_IT_FF0：FIFO0满中断使能 */
    /*! CAN_IT_FOV0：FIFO0溢出中断使能 */
    /*! CAN_IT_EWG：错误警告中断使能（>=96） */
    /*! CAN_IT_EPV：错误被动中断使能(>127) */
    /*! CAN_IT_ERR：错误中断使能 */
    CAN_ITConfig(CAN1, CAN_IT_TME|CAN_IT_FMP0, ENABLE);
}

/*! \fn       int32_t can_send(uint8_t *pucSendBuf, uint16_t unSendLen)
*
*  \brief     CAN发送(输入必须为完整的通信帧)
*
*  \param     pucSendBuf [in] 指向发送缓冲区
*
*  \param     unSendLen  [in] 需发送数据的字节数
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功; EXIT_FAILURE：失败
*/
int32_t can_send(uint8_t *pucSendBuf, uint16_t unSendLen)
{
    int32 lRet = EXIT_FAILURE;
    uint8 ucCnt = 0;
    uint16 i = 0, unDataCnt = 0, unTemp = 0;
    
    do
    {
        /*! 输入参数检查 */
        if(INVALID_POINTER(pucSendBuf) \
           || (1 > unSendLen) || (CAN_MSG_BUF_SIZE < unSendLen))
        {
            break;
        }
        unTemp = unSendLen / 8;
        if(0x0000 != (unSendLen&0x0007))
        {
            unTemp = unTemp + 1;
        }
        while(1)
        {
            /*! 计算报文发送缓冲区空余空间 */
            if(s_unCanMsgTxWrIndex >= s_unCanMsgTxRdIndex)
            {
                unDataCnt = s_unCanMsgTxWrIndex - s_unCanMsgTxRdIndex;
                unDataCnt = CAN_MSG_BUF_SIZE - unDataCnt;
            }
            else
            {
                unDataCnt = s_unCanMsgTxRdIndex - s_unCanMsgTxWrIndex;
            }
            /*! 发送缓冲区 */
            if(unDataCnt >= unTemp)
            {
                break;
            }
            /*! 添加超时，防止死循环 */
        }
        /*! 无分段 */
        if(1 == unTemp)
        {
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].ExtId = 0x00010000;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].IDE = CAN_Id_Extended;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].RTR = CAN_RTR_Data;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].DLC = (uint8)unSendLen;
            for(i=0; i<(uint8)unSendLen; i++)
            {
                (s_astCanMsgTxBuf[s_unCanMsgTxWrIndex]).Data[i] = *pucSendBuf;
                pucSendBuf++;
            }
            s_unCanMsgTxWrIndex++;
            if(CAN_MSG_BUF_SIZE <= s_unCanMsgTxWrIndex)
            {
                s_unCanMsgTxWrIndex = 0;
            }
        }
        else /*!< 帧分为多个报文段 */
        {
            /*! 第一个分段 */
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].ExtId = 0x00014000;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].IDE = CAN_Id_Extended;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].RTR = CAN_RTR_Data;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].DLC = 8;
            for(i=0; i<8; i++)
            {
                (s_astCanMsgTxBuf[s_unCanMsgTxWrIndex]).Data[i] = *pucSendBuf;
                pucSendBuf++;
            }
            s_unCanMsgTxWrIndex++;
            if(CAN_MSG_BUF_SIZE <= s_unCanMsgTxWrIndex)
            {
                s_unCanMsgTxWrIndex = 0;
            }
            unTemp--;
            ucCnt = 1;
            /*! 中间段 */
            while(1 != unTemp)
            {
                s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].ExtId = 0x00018000 + (ucCnt<<8);
                s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].IDE = CAN_Id_Extended;
                s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].RTR = CAN_RTR_Data;
                s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].DLC = 8;
                for(i=0; i<8; i++)
                {
                    (s_astCanMsgTxBuf[s_unCanMsgTxWrIndex]).Data[i] = *pucSendBuf;
                    pucSendBuf++;
                }
                s_unCanMsgTxWrIndex++;
                if(CAN_MSG_BUF_SIZE <= s_unCanMsgTxWrIndex)
                {
                    s_unCanMsgTxWrIndex = 0;
                }
                unTemp--;
                ucCnt++;
            }
            /*! 结束段 */
            if(0x0000 == (unSendLen&0x0007))
            {
                unTemp = 8;
            }
            else
            {
                unTemp = unSendLen & 0x0007;
            }
            ucCnt = 0;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].ExtId = 0x0001C000 + (ucCnt<<8);;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].IDE = CAN_Id_Extended;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].RTR = CAN_RTR_Data;
            s_astCanMsgTxBuf[s_unCanMsgTxWrIndex].DLC = unTemp;
            for(i=0; i<unTemp; i++)
            {
                (s_astCanMsgTxBuf[s_unCanMsgTxWrIndex]).Data[i] = *pucSendBuf;
                pucSendBuf++;
            }
            s_unCanMsgTxWrIndex++;
            if(CAN_MSG_BUF_SIZE <= s_unCanMsgTxWrIndex)
            {
                s_unCanMsgTxWrIndex = 0;
            }
        }
        /*! 发送消息 */
        lRet = send_can_msg();
        NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
    }while(0);
    
    return lRet;
}

/*! \fn       uint8_t *get_can_rx_buf(void)
*
*  \brief     获取CAN接收缓冲区地址
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区地址
*/
uint8_t *get_can_rx_buf(void)
{
    return &s_ucCan1RxBuf[0];
}

/*! \fn       uint16_t *get_can_rx_buf_read_index(void)
*
*  \brief     获取CAN接收缓冲区的读下标
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区的读索引
*/
uint16_t *get_can_rx_buf_read_index(void)
{
    return &s_unCan1RxReadIndex;
}

/*! \fn       uint16_t *get_can_rx_buf_write_index(void)
*
*  \brief     获取CAN接收缓冲区的写下标
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区的写索引
*/
uint16_t *get_can_rx_buf_write_index(void)
{
    return &s_unCan1RxWriteIndex;
}

/*! \fn       int32 print_can_frame(void)
*
*  \brief     打印CAN帧
*
*  \param     无
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 print_can_frame(void)
{
    int32 lRet = EXIT_SUCCESS;

    if(s_unCanSegMsgReadIndex != s_unCanSegMsgWriteIndex)
    {
        put_string(" ");
        lRet = print_protocol_frame(&(s_aucCanSegMsgBuf[s_unCanSegMsgReadIndex][0]));
        s_unCanSegMsgReadIndex++;
        if(CAN_MSG_BUF_SIZE <= s_unCanSegMsgReadIndex)
        {
            s_unCanSegMsgReadIndex = 0;
        }
    }

    return lRet;
}


/******************************************************************************/
/*                          中断处理函数                                      */
/******************************************************************************/
/*! \fn       void USB_HP_CAN1_TX_IRQHandler(void)
*
*  \brief     CAN1发送中断处理函数
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void USB_HP_CAN1_TX_IRQHandler(void)
{
    /*! 发送缓冲区剩余数据 */
    send_can_msg();
    LED1 = ~LED1;
}

/*! \fn       void USB_LP_CAN1_RX0_IRQHandler(void)
*
*  \brief     CAN1 FIFO0接收中断处理函数
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    receive_can_msg();
}

/*! \fn       void CAN1_RX1_IRQHandler(void)
*
*  \brief     CAN1 FIFO1接收中断处理函数
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void CAN1_RX1_IRQHandler(void)
{
}

/*! \fn       void CAN1_SCE_IRQHandler(void)
*
*  \brief     CAN1错误中断处理函数
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void CAN1_SCE_IRQHandler(void)
{
}


/********************************end of file***********************************/

