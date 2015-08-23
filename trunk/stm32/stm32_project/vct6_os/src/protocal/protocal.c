/**
  ******************************************************************************
  * @file    protocal.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-22
  * @brief   协议解析.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x_type.h"
#include "debug.h"
#include "uart.h"

/* Private typedef -----------------------------------------------------------*/
/* 使用扩展区状态码定义 */
enum use_extend_buf_e
{
    NONUSE_EXTEND_BUF = 0,
    USE_EXTEND_BUF,
};

/* 协议解析中的状态码定义 */
enum check_state_e
{
    FRAME_CHECK_START = 0,
    FRAME_CHECK_CONTROL,
    FRAME_CHECK_DATALEN_L,
    FRAME_CHECK_DATALEN_H,
    FRAME_CHECK_CMD_L,
    FRAME_CHECK_CMD_H,
    FRAME_CHECK_DATA,
    FRAME_CHECK_CRC_L,
    FRAME_CHECK_CRC_H,
    FRAME_CHECK_END,
};
/* Private define ------------------------------------------------------------*/
/* 协议帧起始码 */
#define PROTOCAL_FRAME_START                        (0x80)
/* 协议帧结束码 */
#define PROTOCAL_FRAME_END                          (0x55)

/* 控制码定义 */
#define PROTOCAL_FRAME_CONTROL_PARAM_NOTIMESEQ      (0x1D)

/* 协议解析结构体 */
struct check_cmd_t
{
    u8 State; /* 状态 */
    u8 ExBufFlag; /* 是否将数据写入扩展区 */
    u16 DataLen; /* 数据长度 */
    u16 DataCrc; /* CRC */
};
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* 解析命令 */
struct check_cmd_t stCheckCmd = {0};
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/
/**
  * @file   ErrorStatus frame_check_start(u16 *pErr, u8 CheckData)
  * @brief  检查帧起始码
  * @param  pErr      [out] 错误码
  * @param  CheckData [in]  数据
  * @retval ERROR：失败 SUCCESS：成功
  */
static ErrorStatus frame_check_start(u16 *pErr, u8 CheckData)
{
    /* 初始化 */
    stCheckCmd.ExBufFlag = NONUSE_EXTEND_BUF;
    stCheckCmd.DataLen = 0;
    stCheckCmd.DataCrc = 0;

    if(PROTOCAL_FRAME_START == CheckData)
    {
        stCheckCmd.State = FRAME_CHECK_CONTROL;
        return SUCCESS;
    }
    else
    {
        //stCheckCmd.State = FRAME_CHECK_START;
        *pErr = 0x01;
        DEBUG_MSG("E: Frame Start!\r\n");
        return ERROR;
    }
}

/**
  * @file   ErrorStatus frame_check_control(u16 *pErr, u8 CheckData)
  * @brief  检查帧控制码
  * @param  pErr      [out] 错误码
  * @param  CheckData [in]  数据
  * @retval ERROR：失败 SUCCESS：成功
  */
static ErrorStatus frame_check_control(u16 *pErr, u8 CheckData)
{
    ErrorStatus Ret = SUCCESS;

    switch(CheckData)
    {
        case PROTOCAL_FRAME_CONTROL_PARAM_NOTIMESEQ:
            Ret = SUCCESS;
            break;
        /* 无此控制码 */
        default:
            *pErr = 0x01;
            DEBUG_MSG("E: Frame Control!\r\n");
            Ret = ERROR;
            break;
    }

    return Ret;
}

/**
  * @file   ErrorStatus cmd_analysis_process(void)
  * @brief  解析
  * @param  None
  * @retval ERROR：失败 SUCCESS：成功
  */
static ErrorStatus cmd_analysis_process(void)
{
    ErrorStatus Ret = ERROR;
    u8 BufData = 0;
    u8 *pReceiveBuf = NULL;
    u16 ErrorCode = 0, FrameStartIndex = 0;
    u16 *pBufReadIndex = NULL, *pBufWriteIndex = NULL;

    /* 获取缓冲区读索引 */
    pBufReadIndex = retrieve_uart_read_index();
    if(NULL == pBufReadIndex)                        
    {
        return ERROR;
    }
    /* 获取缓冲区写索引 */
    pBufWriteIndex = retrieve_uart_write_index();
    if(NULL == pBufWriteIndex)
    {
        return ERROR;
    }
    /* 判断缓冲区是否有数据 */
    if((*pBufWriteIndex) == (*pBufReadIndex))
    {
        /* 缓冲区无数据 */
        return SUCCESS;
    }
    /* 获取缓冲区地址 */
    pReceiveBuf = retrieve_uart_receive_buf();
    if(NULL == pReceiveBuf)
    {
        return ERROR;
    }

    Ret = SUCCESS;
    /* 解析缓冲区的数据 */
    while((*pBufWriteIndex) != (*pBufReadIndex))
    {
        BufData = pReceiveBuf[*pBufReadIndex];
        switch(stCheckCmd.State)
        {
            /* 起始码 */
            case FRAME_CHECK_START:
                Ret = frame_check_start(&ErrorCode, BufData);
                FrameStartIndex = *pBufReadIndex;
                (*pBufReadIndex)++;
                /* 判断是否将数据移到扩展区 */
                if(UART_BUF_SIZE == (*pBufReadIndex))
                {
                    (*pBufReadIndex) = 0;
                    stCheckCmd.ExBufFlag = USE_EXTEND_BUF;
                }
                break;
            /* 控制码 */
            case FRAME_CHECK_CONTROL:
                break;
            /* 数据长度低字节 */
            case FRAME_CHECK_DATALEN_L:
                break;
            /* 数据长度高字节 */
            case FRAME_CHECK_DATALEN_H:
                break;
            /*  命令低字节 */
            case FRAME_CHECK_CMD_L:
                break;
            /*  命令高字节 */
            case FRAME_CHECK_CMD_H:
                break;
            /* CRC低字节 */
            case FRAME_CHECK_CRC_L:
                break;
            /* CRC高字节 */
            case FRAME_CHECK_CRC_H:
                break;
            /* 结束码 */
            case FRAME_CHECK_END:
                break;
            /* 其他错误 */
            default:
                Ret = ERROR;
                break;
        } /* end switch... */
    } /* end while((*pBufWriteIndex) != (*pBufReadIndex))... */

    return Ret;
}




/* External functions --------------------------------------------------------*/
/**
  * @file   ErrorStatus protocal_parser(void)
  * @brief  协议分析
  * @param  None
  * @retval ERROR：失败 SUCCESS：成功
  */
ErrorStatus protocal_parser(void)
{
    return cmd_analysis_process();
}



/**************************end of file*****************************************/

