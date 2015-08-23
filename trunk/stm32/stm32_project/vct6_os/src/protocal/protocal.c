/**
  ******************************************************************************
  * @file    protocal.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-22
  * @brief   Э�����.
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
/* ʹ����չ��״̬�붨�� */
enum use_extend_buf_e
{
    NONUSE_EXTEND_BUF = 0,
    USE_EXTEND_BUF,
};

/* Э������е�״̬�붨�� */
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
/* Э��֡��ʼ�� */
#define PROTOCAL_FRAME_START                        (0x80)
/* Э��֡������ */
#define PROTOCAL_FRAME_END                          (0x55)

/* �����붨�� */
#define PROTOCAL_FRAME_CONTROL_PARAM_NOTIMESEQ      (0x1D)

/* Э������ṹ�� */
struct check_cmd_t
{
    u8 State; /* ״̬ */
    u8 ExBufFlag; /* �Ƿ�����д����չ�� */
    u16 DataLen; /* ���ݳ��� */
    u16 DataCrc; /* CRC */
};
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* �������� */
struct check_cmd_t stCheckCmd = {0};
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/
/**
  * @file   ErrorStatus frame_check_start(u16 *pErr, u8 CheckData)
  * @brief  ���֡��ʼ��
  * @param  pErr      [out] ������
  * @param  CheckData [in]  ����
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
static ErrorStatus frame_check_start(u16 *pErr, u8 CheckData)
{
    /* ��ʼ�� */
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
  * @brief  ���֡������
  * @param  pErr      [out] ������
  * @param  CheckData [in]  ����
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
static ErrorStatus frame_check_control(u16 *pErr, u8 CheckData)
{
    ErrorStatus Ret = SUCCESS;

    switch(CheckData)
    {
        case PROTOCAL_FRAME_CONTROL_PARAM_NOTIMESEQ:
            Ret = SUCCESS;
            break;
        /* �޴˿����� */
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
  * @brief  ����
  * @param  None
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
static ErrorStatus cmd_analysis_process(void)
{
    ErrorStatus Ret = ERROR;
    u8 BufData = 0;
    u8 *pReceiveBuf = NULL;
    u16 ErrorCode = 0, FrameStartIndex = 0;
    u16 *pBufReadIndex = NULL, *pBufWriteIndex = NULL;

    /* ��ȡ������������ */
    pBufReadIndex = retrieve_uart_read_index();
    if(NULL == pBufReadIndex)                        
    {
        return ERROR;
    }
    /* ��ȡ������д���� */
    pBufWriteIndex = retrieve_uart_write_index();
    if(NULL == pBufWriteIndex)
    {
        return ERROR;
    }
    /* �жϻ������Ƿ������� */
    if((*pBufWriteIndex) == (*pBufReadIndex))
    {
        /* ������������ */
        return SUCCESS;
    }
    /* ��ȡ��������ַ */
    pReceiveBuf = retrieve_uart_receive_buf();
    if(NULL == pReceiveBuf)
    {
        return ERROR;
    }

    Ret = SUCCESS;
    /* ���������������� */
    while((*pBufWriteIndex) != (*pBufReadIndex))
    {
        BufData = pReceiveBuf[*pBufReadIndex];
        switch(stCheckCmd.State)
        {
            /* ��ʼ�� */
            case FRAME_CHECK_START:
                Ret = frame_check_start(&ErrorCode, BufData);
                FrameStartIndex = *pBufReadIndex;
                (*pBufReadIndex)++;
                /* �ж��Ƿ������Ƶ���չ�� */
                if(UART_BUF_SIZE == (*pBufReadIndex))
                {
                    (*pBufReadIndex) = 0;
                    stCheckCmd.ExBufFlag = USE_EXTEND_BUF;
                }
                break;
            /* ������ */
            case FRAME_CHECK_CONTROL:
                break;
            /* ���ݳ��ȵ��ֽ� */
            case FRAME_CHECK_DATALEN_L:
                break;
            /* ���ݳ��ȸ��ֽ� */
            case FRAME_CHECK_DATALEN_H:
                break;
            /*  ������ֽ� */
            case FRAME_CHECK_CMD_L:
                break;
            /*  ������ֽ� */
            case FRAME_CHECK_CMD_H:
                break;
            /* CRC���ֽ� */
            case FRAME_CHECK_CRC_L:
                break;
            /* CRC���ֽ� */
            case FRAME_CHECK_CRC_H:
                break;
            /* ������ */
            case FRAME_CHECK_END:
                break;
            /* �������� */
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
  * @brief  Э�����
  * @param  None
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
ErrorStatus protocal_parser(void)
{
    return cmd_analysis_process();
}



/**************************end of file*****************************************/
