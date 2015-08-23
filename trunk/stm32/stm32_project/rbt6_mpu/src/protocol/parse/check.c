/*! \file  check.c
* \b 接口说明：
*
*       1. check_protocol_frame: 检查协议帧格式 <br>
*
* \b 功能描述:
*
*       帧格式检查
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-03-13   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "deftype.h"
#include "protocolio.h"
#include "protocol.h"
#include "debug.h"
#include "led.h"
#include "identify.h"
#include "errtable.h"
#include "uart.h"

/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
/*! 帧格式检查状态定义 */
typedef enum
{
    FRAME_CHECK_START = 0x00u,
    FRAME_CHECK_CONTROL,
    FRAME_CHECK_DATALEN,
    FRAME_CHECK_DATA,
    FRAME_CHECK_CHECKSUM,
    FRAME_CHECK_STOP,
    FRAME_CHECK_ERROR
} check_state_e;

/******************************************************************************/
/*                              结构定义                                      */
/******************************************************************************/
/*! 帧格式检查结构体 */
typedef struct _check_frame
{
    volatile uint8 ucCheckState; /*!< 帧检查状态 */
    volatile uint8 ucFrameDataLen; /*!< 帧数据区的长度 */
    volatile uint8 ucCheckSum; /*!< 帧数据区的校验和 */
} frame_check_t;

/******************************************************************************/
/*                             变量定义                                       */
/******************************************************************************/
/*! 帧格式检查 */
static frame_check_t s_stCheckFrame = {FRAME_CHECK_START, 0, 0};
/*! 帧缓冲区(缓存) */
static uint8 s_aucFrameBuf[PROTOCOL_FRAME_MAX_SIZE] = {0};
/*! 帧缓冲区下标 */
static uint8 s_ucFrameBufIndex = 0;
/*! 帧返回缓冲区 */
static return_buf_t s_stReturnBuf = {0};

/******************************************************************************/
/*                            内部接口声明                                    */
/******************************************************************************/




/******************************************************************************/
/*                            内部接口实现                                    */
/******************************************************************************/
/*! \fn       int32 check_frame_start(uint8 ucFrameData, uint16 *punErrCode)
*
*  \brief     帧起始码判断
*
*  \param     ucFrameData [in]  帧数据
*
*  \param     punErrCode  [out] 错误码
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 check_frame_start(uint8 ucFrameData, uint16 *punErrCode)
{
    int32 lRet = EXIT_FAILURE;
    
    if(PROTOCOL_FRAME_START_CODE == ucFrameData)
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_CONTROL;
        s_ucFrameBufIndex = 0;
        s_aucFrameBuf[s_ucFrameBufIndex] = ucFrameData;
        s_ucFrameBufIndex++;
        lRet = EXIT_SUCCESS;
    }
    else
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_ERROR;
        *punErrCode = ERR_DB_CMD_FORMAT_START;
    }
    
    return lRet;
}

/*! \fn       int32 check_frame_control(uint8 ucFrameData, uint16 *punErrCode)
*
*  \brief     帧控制码判断
*
*  \param     ucFrameData [in]  帧数据
*
*  \param     punErrCode  [out] 错误码
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 check_frame_control(uint8 ucFrameData, uint16 *punErrCode)
{
    int32 lRet = EXIT_FAILURE;
    
    switch(ucFrameData)
    {
        /*! 控制码 */
        case FRAME_CONTROL_CODE_COMMAND:
            s_stCheckFrame.ucCheckState = FRAME_CHECK_DATALEN;
            s_aucFrameBuf[s_ucFrameBufIndex] = ucFrameData;
            /*! 校验和初值 */
            s_stCheckFrame.ucCheckSum = ucFrameData;
            s_ucFrameBufIndex++;
            lRet = EXIT_SUCCESS;
            break;
        /*! 错误控制码 */
        default:
            s_stCheckFrame.ucCheckState = FRAME_CHECK_ERROR;
            *punErrCode = ERR_DB_CMD_FORMAT_CONTROL;
            break;
    }
    
    return lRet;
}

/*! \fn       int32 check_frame_datalen(uint8 ucFrameData, uint16 *punErrCode)
*
*  \brief     帧数据长度判断
*
*  \param     ucFrameData [in]  帧数据
*
*  \param     punErrCode  [out] 错误码
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 check_frame_datalen(uint8 ucFrameData, uint16 *punErrCode)
{
    int32 lRet = EXIT_FAILURE;
    
    /*! 帧始终有3字节的命令码 */
    if((3 <= ucFrameData) && ((PROTOCOL_FRAME_MAX_SIZE-5) >= ucFrameData))
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_DATA;
        s_aucFrameBuf[s_ucFrameBufIndex] = ucFrameData;
        s_ucFrameBufIndex++;
        /*! 保存长度值 */
        s_stCheckFrame.ucFrameDataLen = ucFrameData;
        /*! 计算校验和 */
        s_stCheckFrame.ucCheckSum += ucFrameData;
        lRet = EXIT_SUCCESS;
    }
    else
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_ERROR;
        *punErrCode = ERR_DB_CMD_FORMAT_DATALEN;
    }
    
    return lRet;
}

/*! \fn       int32 check_frame_data(uint8 ucFrameData, uint16 *punErrCode)
*
*  \brief     帧数据
*
*  \param     ucFrameData [in]  帧数据
*
*  \param     punErrCode  [out] 错误码
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 check_frame_data(uint8 ucFrameData, uint16 *punErrCode)
{
    int32 lRet = EXIT_SUCCESS;
    
    if(0 < s_stCheckFrame.ucFrameDataLen)
    {
        s_stCheckFrame.ucFrameDataLen--;
        /*! 保存数据 */
        s_aucFrameBuf[s_ucFrameBufIndex] = ucFrameData;
        s_ucFrameBufIndex++;
        /*! 计算校验和 */
        s_stCheckFrame.ucCheckSum += ucFrameData;
    }
    /*! 判断数据是否接收完成 */
    if(0 == s_stCheckFrame.ucFrameDataLen)
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_CHECKSUM;
    }
    
    return lRet;
}

/*! \fn       int32 check_frame_checksum(uint8 ucFrameData, uint16 *punErrCode)
*
*  \brief     检查帧校验和
*
*  \param     ucFrameData [in]  帧数据
*
*  \param     punErrCode  [out] 错误码
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 check_frame_checksum(uint8 ucFrameData, uint16 *punErrCode)
{
    int32 lRet = EXIT_FAILURE;
    
    if(s_stCheckFrame.ucCheckSum == ucFrameData)
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_STOP;
        /*! 保存数据 */
        s_aucFrameBuf[s_ucFrameBufIndex] = ucFrameData;
        s_ucFrameBufIndex++;
        lRet = EXIT_SUCCESS;
    }
    else
    {
        /*! 校验和错误 */
        s_stCheckFrame.ucCheckState = FRAME_CHECK_ERROR;
        *punErrCode = ERR_DB_CMD_FORMAT_CHECKSUM;
    }
    
    return lRet;
}

/*! \fn       int32 check_frame_stop(uint8 ucFrameData, uint16 *punErrCode)
*
*  \brief     检查帧停止码
*
*  \param     ucFrameData [in]  帧数据
*
*  \param     punErrCode  [out] 错误码
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 check_frame_stop(uint8 ucFrameData, uint16 *punErrCode)
{
    int32 lRet = EXIT_FAILURE;
    
    if(PROTOCOL_FRAME_STOP_CODE == ucFrameData)
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_START;
        /*! 保存数据 */
        s_aucFrameBuf[s_ucFrameBufIndex] = ucFrameData;
        s_ucFrameBufIndex++;
        lRet = EXIT_SUCCESS;
    }
    else
    {
        /*! 校验和错误 */
        s_stCheckFrame.ucCheckState = FRAME_CHECK_ERROR;
        *punErrCode = ERR_DB_CMD_FORMAT_STOP;
    }
    
    return lRet;
}

/*! \fn       int32 self_check_frame(uint8 ucFrameData)
*
*  \brief     自检
*
*  \param     ucFrameData [in]  帧数据
*
*  \param     punErrCode  [out] 错误码
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 self_check_frame(uint8 ucFrameData)
{
    int32 lRet = EXIT_FAILURE;
    static uint8 s_ucPreFrameData = 0x00;
    
    /*! 检测到0x55 0xAA组合，则认为检测到新的通讯帧 */
    if((PROTOCOL_FRAME_STOP_CODE == s_ucPreFrameData) \
        && (PROTOCOL_FRAME_START_CODE == ucFrameData))
    {
        s_stCheckFrame.ucCheckState = FRAME_CHECK_CONTROL;
        /*! 保存数据 */
        s_ucFrameBufIndex = 0;
        s_aucFrameBuf[s_ucFrameBufIndex] = ucFrameData;
        s_ucFrameBufIndex++;
        lRet = EXIT_SUCCESS;
    }
    else
    {
        s_ucPreFrameData = ucFrameData;
    }
    
    return lRet;
}


/******************************************************************************/
/*                                外部接口                                    */
/******************************************************************************/
/*! \fn       int32 check_protocol_frame(void)
*
*  \brief     协议帧格式检查
*
*  \param     无
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 check_protocol_frame(void)
{
    int32 lRet = EXIT_FAILURE;
    uint8 ucRecData = 0;
    uint8 *pucRecBuf = NULL;
    uint16 *punRecBufWriteIndex = NULL;
    uint16 *punRecBufReadIndex = NULL;
    uint16 unErrCode = NO_ERR;
    
    do
    {
        /*! 获取接收缓冲区的写索引 */
        punRecBufWriteIndex = get_receive_buf_write_index();
        if(INVALID_POINTER(punRecBufWriteIndex))
        {
            debug_msg("E:get write index error!", __FILE__, __LINE__);
            break;
        }
        /*! 获取接收缓冲区的读索引 */
        punRecBufReadIndex = get_receive_buf_read_index();
        if(INVALID_POINTER(punRecBufReadIndex))
        {
            debug_msg("E:get read index error!", __FILE__, __LINE__);
            break;
        }
        /*! 判断接收缓冲区是否有数据 */
        if((*punRecBufWriteIndex) == (*punRecBufReadIndex))
        {
            lRet = EXIT_SUCCESS;
            break;
        }
        /*! 获取接收缓冲区 */
        pucRecBuf = get_receive_buf();
        if(INVALID_POINTER(pucRecBuf))
        {
            debug_msg("E:get recbuf addr error!", __FILE__, __LINE__);
            break;
        }
        /*! 检查缓冲区 */
        lRet = EXIT_SUCCESS;
        while((*punRecBufWriteIndex) != (*punRecBufReadIndex))
        {
            /*! 获取缓冲器的数据 */
            ucRecData = pucRecBuf[(*punRecBufReadIndex)];
            (*punRecBufReadIndex)++;
            if(RECEIVE_BUF_SIZE <= (*punRecBufReadIndex))
            {
                *punRecBufReadIndex = 0;
            }
            /*! 检查获取的数据格式 */
            switch(s_stCheckFrame.ucCheckState)
            {
                /*! 起始码 */
                case FRAME_CHECK_START:
                    check_frame_start(ucRecData, &unErrCode);
                    break;
                /*! 控制码 */
                case FRAME_CHECK_CONTROL:
                    check_frame_control(ucRecData, &unErrCode);
                    break;
                /*! 数据长度 */
                case FRAME_CHECK_DATALEN:
                    check_frame_datalen(ucRecData, &unErrCode);
                    break;
                /*! 数据 */
                case FRAME_CHECK_DATA:
                    check_frame_data(ucRecData, &unErrCode);
                    break;
                /*! 校验和 */
                case FRAME_CHECK_CHECKSUM:
                    check_frame_checksum(ucRecData, &unErrCode);
                    break;
                /*! 结束码*/
                case FRAME_CHECK_STOP:
                    if(EXIT_SUCCESS == check_frame_stop(ucRecData, &unErrCode))
                    {
                        //debug_msg("check frame ok!", __FILE__, __LINE__);
                        /*! 识别协议帧 */
                        lRet = identify_protocol_frame(s_aucFrameBuf, &s_stReturnBuf);
                    }
                    break;
                
                /*! 帧检查错误 */
                case FRAME_CHECK_ERROR:
                    if(EXIT_SUCCESS == self_check_frame(ucRecData))
                    {
                        debug_msg("self-check ok\r\n", NULL, 0);
                    }
                    break;
                /*! 其他值 */
                default:
                    break;
            }/*!< end switch(s_stCheckFrame.ucCheckState)... */
            
            /*! 判断检查帧格式是否有错误 */
            if(NO_ERR != unErrCode)
            {
                /*! 通信故障，返回EVT */
                DEBUG_MSG("E:check frame error(0x%04x)! %s %d\r\n", unErrCode, __FILE__, __LINE__);
                unErrCode = 0;
            }
        } /*!< end while((*punRecBufWriteIndex) != (*punRecBufReadIndex))... */
    }while(0);
    
    return lRet;
}

/********************************end of file***********************************/

