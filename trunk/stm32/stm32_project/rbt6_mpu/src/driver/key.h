﻿/*! \file  key.h
* \b 接口说明：
*       
*
* \b 功能描述:
*
*       扫描按键
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-03-10   创建文件   <br>
*/
#ifndef __KEY_H
#define __KEY_H

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/


/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
/*! 按键定义 */
#define KEY_NONE            (0x00)
#define KEY_S1              (0x01)
#define KEY_S2              (0x02)
/*! 按键判断 */
#define CHECK_KEY_DOWN(keytemp, key)    ((key) == ((keytemp)&(key)))
#define CHECK_KEY_UP(keytemp, key)      ((key) != ((keytemp)&(key)))

/******************************************************************************/
/*                          外部接口函数声明                                  */
/******************************************************************************/
void key_port_init(void);
uint8_t key_read(void);


#endif /*!< end __KEY_H */

/********************************end of file***********************************/
