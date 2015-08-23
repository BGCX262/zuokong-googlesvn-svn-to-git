/*! \file  protocol.c
* \b 接口说明：
*
*       1. protocol_process: 协议处理 <br>
*
*       2. send_return_frame: 发送协议返回帧 <br>
*
* \b 功能描述:
*
*       协议相关
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
#include "commonres.h"
#include "protocol.h"
#include "protocolio.h"
#include "debug.h"
#include "check.h"


/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/

/******************************************************************************/
/*                              结构定义                                      */
/******************************************************************************/

/******************************************************************************/
/*                             变量定义                                       */
/******************************************************************************/

/******************************************************************************/
/*                            内部接口声明                                    */
/******************************************************************************/


/******************************************************************************/
/*                            内部接口实现                                    */
/******************************************************************************/
/*! \fn       static uint8 calc_frame_checksum(uint8 *pucFrame)
*
*  \brief     计算帧校验和
*
*  \param     pucFrame [in] 指向帧缓冲区
*
*  \exception 注意：输入必须是完整的通信帧
*
*  \return    帧校验和
*/
static uint8 calc_frame_checksum(uint8 *paucFrame)
{
    uint8 ucTemp = 0;
    uint8 i = 0, ucCheckSum = 0;
    
    ucCheckSum = 0;
    ucTemp = paucFrame[FRAME_DATALEN_INDEX] + 2;
    ucTemp += FRAME_CONTROL_INDEX;
    for(i=FRAME_CONTROL_INDEX; i<ucTemp; i++)
    {
        ucCheckSum += paucFrame[i];
    }

    return ucCheckSum;
}

/*! \fn       static int32 send_ack_frame(const return_buf_t *pstFrameBuf)
*
*  \brief     发送ACK帧
*
*  \param     pstFrameBuf [in] 指向返回帧缓冲区
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
static int32 send_ack_frame(const return_buf_t *pstFrameBuf)
{
    uint8 aucAckFrame[ACK_FRAME_MAX_SIZE] = {0};

    /*! 帧头 */
    aucAckFrame[FRAME_START_INDEX] = PROTOCOL_FRAME_START_CODE;
    /*! 控制码 */
    aucAckFrame[FRAME_CONTROL_INDEX] = pstFrameBuf->ucControlCode;
    /*! 帧长度(ACK固定长度) */
    aucAckFrame[FRAME_DATALEN_INDEX] = 0x05;
    /*! 命令码 */
    aucAckFrame[FRAME_COMMAND_TYPE_INDEX] = pstFrameBuf->ucCmdType;
    aucAckFrame[FRAME_COMMAND_NUM_INDEX] = pstFrameBuf->ucCmdNum;
    aucAckFrame[FRAME_COMMAND_CONTENT_INDEX] = pstFrameBuf->ucCmdContent;
    /*! 保留 */
    aucAckFrame[FRAME_COMMAND_PARAM_INDEX] = (uint8)((pstFrameBuf->unReserve)>>8);
    aucAckFrame[FRAME_COMMAND_PARAM_INDEX+1] = (uint8)(pstFrameBuf->unReserve);
    /*! 计算校验和 */
    aucAckFrame[FRAME_COMMAND_PARAM_INDEX+2] = calc_frame_checksum(aucAckFrame);
    /*! 停止码 */
    aucAckFrame[FRAME_COMMAND_PARAM_INDEX+3] = PROTOCOL_FRAME_STOP_CODE;
    /*! 发送ACK帧 */
    return send_return_data(aucAckFrame, aucAckFrame[FRAME_DATALEN_INDEX]+5);
}

/*! \fn       static int32 send_rep_frame(const return_buf_t *pstFrameBuf)
*
*  \brief     发送REP帧
*
*  \param     pstFrameBuf [in] 指向返回帧缓冲区
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
static int32 send_rep_frame(const return_buf_t *pstFrameBuf)
{
    uint8 i = 0;
    uint8 aucRepFrame[REP_FRAME_MAX_SIZE] = {0};

    /*! 帧头 */
    aucRepFrame[FRAME_START_INDEX] = PROTOCOL_FRAME_START_CODE;
    /*! 控制码 */
    aucRepFrame[FRAME_CONTROL_INDEX] = pstFrameBuf->ucControlCode;
    /*! 命令码 */
    aucRepFrame[FRAME_COMMAND_TYPE_INDEX] = pstFrameBuf->ucCmdType;
    aucRepFrame[FRAME_COMMAND_NUM_INDEX] = pstFrameBuf->ucCmdNum;
    aucRepFrame[FRAME_COMMAND_CONTENT_INDEX] = pstFrameBuf->ucCmdContent;
    /*! 判断是否为查询命令 */
    if(0 < pstFrameBuf->ucStateLen)
    {
        if((24 < pstFrameBuf->ucStateLen) || INVALID_POINTER(pstFrameBuf->pucStateBuf))
        {
            debug_msg("E:return quire cmd, param null!", __FILE__, __LINE__);
            return EXIT_FAILURE;
        }
        /*! 查询命令的响应帧长度 */
        aucRepFrame[FRAME_DATALEN_INDEX] = 0x03 + pstFrameBuf->ucStateLen;
        /*! 填充查询数据 */
        for(i=0; i<pstFrameBuf->ucStateLen; i++)
        {
            aucRepFrame[FRAME_COMMAND_PARAM_INDEX+i] = (pstFrameBuf->pucStateBuf)[i];
        }
        /*! 计算校验和 */
        aucRepFrame[FRAME_COMMAND_PARAM_INDEX+(pstFrameBuf->ucStateLen)] \
            = calc_frame_checksum(aucRepFrame);
        /*! 停止码 */
        aucRepFrame[FRAME_COMMAND_PARAM_INDEX+(pstFrameBuf->ucStateLen)+1] \
            = PROTOCOL_FRAME_STOP_CODE;
    }
    else
    {
        /*! 非查询命令的响应帧为固定长度 */
        aucRepFrame[FRAME_DATALEN_INDEX] = 0x05;
        /*! 保留 */
        aucRepFrame[FRAME_COMMAND_PARAM_INDEX] = (uint8)((pstFrameBuf->unReserve)>>8);
        aucRepFrame[FRAME_COMMAND_PARAM_INDEX+1] = (uint8)(pstFrameBuf->unReserve);
        /*! 计算校验和 */
        aucRepFrame[FRAME_COMMAND_PARAM_INDEX+2] = calc_frame_checksum(aucRepFrame);
        /*! 停止码 */
        aucRepFrame[FRAME_COMMAND_PARAM_INDEX+3] = PROTOCOL_FRAME_STOP_CODE;
    }
    /*! 发送REP帧 */
    return send_return_data(aucRepFrame, aucRepFrame[FRAME_DATALEN_INDEX]+5);
}

/*! \fn       static int32 send_evt_frame(const return_buf_t *pstFrameBuf)
*
*  \brief     发送EVT帧
*
*  \param     pstFrameBuf [in] 指向返回帧缓冲区
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
static int32 send_evt_frame(const return_buf_t *pstFrameBuf)
{
    uint8 aucEvtFrame[EVT_FRAME_MAX_SIZE] = {0};

    /*! 帧头 */
    aucEvtFrame[FRAME_START_INDEX] = PROTOCOL_FRAME_START_CODE;
    /*! 控制码 */
    aucEvtFrame[FRAME_CONTROL_INDEX] = pstFrameBuf->ucControlCode;
    /*! 帧长度(EVT固定长度) */
    aucEvtFrame[FRAME_DATALEN_INDEX] = 0x05;
    /*! 命令码 */
    aucEvtFrame[FRAME_COMMAND_TYPE_INDEX] = pstFrameBuf->ucCmdType;
    aucEvtFrame[FRAME_COMMAND_NUM_INDEX] = pstFrameBuf->ucCmdNum;
    aucEvtFrame[FRAME_COMMAND_CONTENT_INDEX] = pstFrameBuf->ucCmdContent;
    /*! 错误码 */
    aucEvtFrame[FRAME_COMMAND_PARAM_INDEX] = (uint8)((pstFrameBuf->unErrCode)>>8);
    aucEvtFrame[FRAME_COMMAND_PARAM_INDEX+1] = (uint8)(pstFrameBuf->unErrCode);
    /*! 计算校验和 */
    aucEvtFrame[FRAME_COMMAND_PARAM_INDEX+2] = calc_frame_checksum(aucEvtFrame);
    /*! 停止码 */
    aucEvtFrame[FRAME_COMMAND_PARAM_INDEX+3] = PROTOCOL_FRAME_STOP_CODE;
    /*! 发送EVT帧 */
    return send_return_data(aucEvtFrame, aucEvtFrame[FRAME_DATALEN_INDEX]+5);
}


/******************************************************************************/
/*                                外部接口                                    */
/******************************************************************************/
/*! \fn       int32 protocol_process(void)
*
*  \brief     协议处理
*
*  \param     无
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 protocol_process(void)
{
    int32 lRet = EXIT_FAILURE;
    
    lRet = check_protocol_frame();

    return lRet;
}

/*! \fn       int32 send_return_frame(return_buf_t *pstFrameBuf)
*
*  \brief     发送返回帧
*
*  \param     pstFrameBuf [in] 指向返回帧缓冲区
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 send_return_frame(const return_buf_t *pstFrameBuf)
{
    int32 lRet = EXIT_FAILURE;
    
    do
    {
        /*! 输入参数检查 */
        if(INVALID_POINTER(pstFrameBuf))
        {
            debug_msg("E:input param error!", __FILE__, __LINE__);
            break;
        }
        /*! 判断返回帧类型 */
        switch(pstFrameBuf->ucFlag)
        {
            /*! 返回ACK */
            case NEED_ACK:
                lRet = send_ack_frame(pstFrameBuf);
                break;
            /*! 返回REP */
            case NEED_REP:
                lRet = send_rep_frame(pstFrameBuf);
                break;
            /*! 返回EVT */
            case NEED_EVT:
                lRet = send_evt_frame(pstFrameBuf);
                break;
            /*! 其他 */
            default:
                debug_msg("E:what's this?", __FILE__, __LINE__);
                break;
        }
    }while(0);
    
    return lRet;
}

/*! \fn       int32 print_protocol_frame(const uint8 *pucFrameBuf)
*
*  \brief     打印协议帧
*
*  \param     pucFrameBuf [in] 指向帧缓冲区
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 print_protocol_frame(const uint8 *pucFrameBuf)
{
    uint8 i = 0, ucFrameLen = 0;
    int32 lRet = EXIT_FAILURE;

    do
    {
        ucFrameLen = pucFrameBuf[FRAME_DATALEN_INDEX] + 5;
        if(32 < ucFrameLen)
        {
            debug_msg("E:input param error!", __FILE__, __LINE__);
            break;
        }
        /*! 打印16进制数据 */
        for(i=0; i<ucFrameLen; i++)
        {
            DEBUG_MSG("%02x ", pucFrameBuf[i]);
        }
        DEBUG_MSG("\r\n");
        lRet = EXIT_SUCCESS;
    }while(0);

    return lRet;
}


/********************************end of file***********************************/

