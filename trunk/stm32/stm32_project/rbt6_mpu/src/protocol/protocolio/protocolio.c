/*! \file  protocolio.c
* \b 接口说明：
*
*       1. get_receive_buf_read_index: 获得接收缓冲区的读索引 <br>
*
*       2. get_receive_buf_write_index: 获得接收缓冲区的写索引 <br>
*
*       3. get_receive_buf: 获得接收缓冲区 <br>
*
*       4. send_return_data: 发送协议返回数据 <br>
*
* \b 功能描述:
*
*       通信协议接口实现
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-03-13   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include <stdint.h>
#include "deftype.h"
#include "config.h"
#include "uart.h"
#include "can.h"
#include "debug.h"
#include "protocolio.h"

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




/******************************************************************************/
/*                                外部接口                                    */
/******************************************************************************/
/*! \fn       uint16* get_receive_buf_read_index(void)
*
*  \brief     获得接收缓冲区的读索引
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区的读索引
*/
uint16* get_receive_buf_read_index(void)
{
    return get_uart_rx_buf_read_index();
}

/*! \fn       uint16* get_receive_buf_write_index(void)
*
*  \brief     获得接收缓冲区的写索引
*
*  \param     无
*
*  \exception 无
*
*  \return    接收缓冲区的写索引
*/
uint16* get_receive_buf_write_index(void)
{
    return get_uart_rx_buf_write_index();
}

/*! \fn       uint8* get_receive_buf(void)
*
*  \brief     获得接收缓冲区
*
*  \param     无
*
*  \exception 无
*
*  \return    指向接收缓冲区
*/
uint8* get_receive_buf(void)
{
    return get_uart_rx_buf();
}

/*! \fn       int32 send_return_data(uint8 *pucBuffer, uint16 unLength)
*
*  \brief     发送协议返回数据
*
*  \param     pucBuffer [in] 指向需发送数据的缓冲区
*
*  \param     unLength  [in] 发送长度
*
*  \exception 无
*
*  \return    EXIT_FAILURE：失败；EXIT_SUCCESS：成功
*/
int32 send_return_data(uint8 *pucBuffer, uint16 unLength)
{
    int32 lRet = EXIT_FAILURE;
    
    lRet = uart_send(pucBuffer, unLength);
    
    return lRet;
}


/********************************end of file***********************************/

