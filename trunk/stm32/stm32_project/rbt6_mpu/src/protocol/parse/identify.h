﻿/*! \file  identify.h
* \b 接口说明：
*       
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

#ifndef __IDENTIFY_H
#define __IDENTIFY_H

/******************************************************************************/
/*                            头文件                                          */
/******************************************************************************/


/******************************************************************************/
/*                           宏定义                                           */
/******************************************************************************/


/******************************************************************************/
/*                          外部接口函数声明                                  */
/******************************************************************************/
int32 identify_protocol_frame(const uint8 *pucFrameBuf, \
                              return_buf_t *pstReturnBuf);


#endif /*!< end __IDENTIFY_H */

/********************************end of file***********************************/

