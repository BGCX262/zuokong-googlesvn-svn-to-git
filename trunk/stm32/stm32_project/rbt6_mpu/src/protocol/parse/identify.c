/*! \file  identify.c
* \b 接口说明：
*
*       1. identify_protocol_frame: 识别协议帧 <br>
*
* \b 功能描述:
*
*       协议帧识别
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-03-14   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include <stdint.h>
#include "deftype.h"
#include "protocol.h"
#include "debug.h"
#include "errtable.h"
#include "can.h"

/******************************************************************************/
/*                              宏定义                                        */
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


/******************************************************************************/
/*                                外部接口                                    */
/******************************************************************************/
/*! \fn       int32 identify_protocol_frame(const uint8 *pucFrameBuf, \
*                                           return_buf_t *pstReturnBuf)
*
*  \brief     识别协议帧
*
*  \param     pucFrameBuf  [in] 指向需识别的协议帧
*
*  \param     pstReturnBuf [in] 帧返回缓冲区
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 identify_protocol_frame(const uint8 *pucFrameBuf, \
                              return_buf_t *pstReturnBuf)
{
    int32 lRet = EXIT_FAILURE;
    uint8 ucIndentifyCode = 0;
    
    do
    {
        /*! 输入参数检查 */
        if(INVALID_POINTER(pucFrameBuf) || INVALID_POINTER(pstReturnBuf))
        {
            debug_msg("E:input param error!", __FILE__, __LINE__);
            break;
        }
        /*! 获取识别码 */
        ucIndentifyCode = pucFrameBuf[FRAME_CONTROL_INDEX] & CONTROL_CODE_IDENT_MASK;
        /*! 识别 */
        switch(ucIndentifyCode)
        {
            /*! 命令 */
            case CONTROL_CODE_IDENT_CMD:
                /*! 发送数据 */
                put_string("s: ");
                lRet = print_protocol_frame(pucFrameBuf);
                lRet = can_send((uint8 *)pucFrameBuf, pucFrameBuf[FRAME_DATALEN_INDEX]+5);
                break;
            /*! 其他值错误 */
            default:
                debug_msg("E:what's this?", __FILE__, __LINE__);
                break;
        }
    }while(0);
    
    return lRet;
}

/********************************end of file***********************************/

