/*! \file  debug.h
* \b �ӿ�˵����
*
*
* \b ��������:
*
*       ���Խӿ�
*
* \b ��ʷ��¼:
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2012-05-16   �����ļ�   <br>
*/

#ifndef __DEBUG_H
#define __DEBUG_H

/******************************************************************************/
/*                            ͷ�ļ�                                          */
/******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

/******************************************************************************/
/*                           �궨��                                           */
/******************************************************************************/
/*! printf �궨�� */
/*! ������Ϣ "D:....." */
#if DEBUG_MSG_EN > 0
 #define DEBUG_MSG(arg...)      printf(##arg)
#else
 #define DEBUG_MSG(arg...)      ((void)0)
#endif

/*! ������Ϣ "E:....." */
#if ERROR_MSG_EN > 0
 #define ERROR_MSG(arg...)      printf(##arg)
#else
 #define ERROR_MSG(arg...)      ((void)0)
#endif

/*! ������Ϣ "W:....." */
#if WARNING_MSG_EN > 0
 #define WARNING_MSG(arg...)    printf(##arg)
#else
 #define WARNING_MSG(arg...)    ((void)0)
#endif

/*! ��ʾ��Ϣ "N:....." */
#if NOTE_MSG_EN > 0
 #define NOTE_MSG(arg...)       printf(##arg)
#else
 #define NOTE_MSG(arg...)       ((void)0)
#endif


/******************************************************************************/
/*                          �ⲿ�ӿں�������                                  */
/******************************************************************************/
int fputc(int ch, FILE *f);


#endif /*!< end __DEBUG_H */

/********************************end of file***********************************/
