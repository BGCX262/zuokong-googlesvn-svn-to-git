/*! \file  debug.h
* \b 接口说明：
*
*
* \b 功能描述:
*
*       调试接口
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2012-05-16   创建文件   <br>
*/

#ifndef __DEBUG_H
#define __DEBUG_H

/******************************************************************************/
/*                            头文件                                          */
/******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

/******************************************************************************/
/*                           宏定义                                           */
/******************************************************************************/
/*! printf 宏定义 */
/*! 调试信息 "D:....." */
#if DEBUG_MSG_EN > 0
 #define DEBUG_MSG(arg...)      printf(##arg)
#else
 #define DEBUG_MSG(arg...)      ((void)0)
#endif

/*! 错误信息 "E:....." */
#if ERROR_MSG_EN > 0
 #define ERROR_MSG(arg...)      printf(##arg)
#else
 #define ERROR_MSG(arg...)      ((void)0)
#endif

/*! 警告信息 "W:....." */
#if WARNING_MSG_EN > 0
 #define WARNING_MSG(arg...)    printf(##arg)
#else
 #define WARNING_MSG(arg...)    ((void)0)
#endif

/*! 提示信息 "N:....." */
#if NOTE_MSG_EN > 0
 #define NOTE_MSG(arg...)       printf(##arg)
#else
 #define NOTE_MSG(arg...)       ((void)0)
#endif


/******************************************************************************/
/*                          外部接口函数声明                                  */
/******************************************************************************/
int fputc(int ch, FILE *f);


#endif /*!< end __DEBUG_H */

/********************************end of file***********************************/

