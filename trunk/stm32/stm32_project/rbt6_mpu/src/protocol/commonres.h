/*! \file  commonres.h
* \b 接口说明：
*       
*
* \b 功能描述:
*
*       共用定义
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-03-13   创建文件   <br>
*/

#ifndef __COMMONRES_H
#define __COMMONRES_H

/******************************************************************************/
/*                            头文件                                          */
/******************************************************************************/


/******************************************************************************/
/*                           宏定义                                           */
/******************************************************************************/
/*! 协议帧最大长度*/
#define PROTOCOL_FRAME_MAX_SIZE                 (32u)
/*! ACK帧最大长度 */
#define ACK_FRAME_MAX_SIZE                      (10u)
/*! REP帧最大长度 */
#define REP_FRAME_MAX_SIZE                      (32u)
/*! EVT帧最大长度 */
#define EVT_FRAME_MAX_SIZE                      (10u)
/*! 查询命令返回数据的最大长度 */
#define QUERY_CMD_STATE_DATA_MAX_SIZE           (PROTOCOL_FRAME_MAX_SIZE-8)

/*! 协议帧起始码 */
#define PROTOCOL_FRAME_START_CODE               (0xAA)
/*! 协议帧停止码 */
#define PROTOCOL_FRAME_STOP_CODE                (0x55)

/*! 协议帧格式下标 */
#define FRAME_START_INDEX                       (0u)
#define FRAME_CONTROL_INDEX                     (1u)
#define FRAME_DATALEN_INDEX                     (2u)
#define FRAME_COMMAND_INDEX                     (3u)
#define FRAME_COMMAND_TYPE_INDEX                (3u)
#define FRAME_COMMAND_NUM_INDEX                 (4u)
#define FRAME_COMMAND_CONTENT_INDEX             (5u)
#define FRAME_COMMAND_PARAM_INDEX               (6u)

/*! 控制码编码 */
#define FRAME_CONTROL_CODE_COMMAND              (0x00) /*!< 下传命令帧 */
#define FRAME_CONTROL_CODE_REP                  (0xC0) /*!< 从站响应帧 */
#define FRAME_CONTROL_CODE_ACK                  (0xD4) /*!< 从站应答帧 */
#define FRAME_CONTROL_CODE_EVT                  (0xF8) /*!< 从站异常帧 */

/*! 控制码mask */
#define CONTROL_CODE_TRANSDIR_MASK              (0x80)
#define CONTROL_CODE_RETURNFLAG_MASK            (0x40)
#define CONTROL_CODE_ABNORMALFLAG_MASK          (0x20)
#define CONTROL_CODE_FUNCTION_MASK              (0x1C)
#define CONTROL_CODE_IDENT_MASK                 (0x03)

/*! 控制码功能编码 */
#define CONTROL_CODE_FUNCTION_CMD               (0x00) /*!< 命令 */
#define CONTROL_CODE_FUNCTION_ACK               (0x14) /*!< 应答帧 */
#define CONTROL_CODE_FUNCTION_UPLOAD            (0x18) /*!< 主动上传帧 */

/*! 控制码识别编码 */
#define CONTROL_CODE_IDENT_CMD                  (0x00) /*!< 命令 */

/*! 返回帧状态 */
typedef enum
{
    NONE_RETURN = 0x00, /*! 不返回 */
    NEED_ACK, /*!< 需要返回ACK */
    NEED_REP, /*!< 需要返回REP */
    NEED_EVT, /*!< 需要返回EVT */
} return_frame_state_e;

/******************************************************************************/
/*                            结构体定义                                      */
/******************************************************************************/
/*! 返回帧结构体定义 */
typedef struct _return_frame_buf
{
    uint8 ucFlag; /*!< 返回帧类型 */
    uint8 ucControlCode; /*!< 控制码 */
    uint8 ucCmdType; /*!< 命令类型 */
    uint8 ucCmdNum; /*!< 命令对象 */
    uint8 ucCmdContent; /*!< 命令内容 */
    uint16 unReserve; /*!< 保留(0x0000) */
    uint16 unErrCode; /*!< 错误码 */
    uint8 ucStateLen; /*!< 查询命令的返回数据长度 */
    uint8 *pucStateBuf; /*!< 查询命令的返回数据缓冲区 */
} return_buf_t;

/******************************************************************************/
/*                          外部接口函数声明                                  */
/******************************************************************************/


#endif /*!< end __COMMONRES_H */

/********************************end of file***********************************/

