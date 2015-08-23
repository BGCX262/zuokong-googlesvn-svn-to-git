/*! \file  task_app.h
* \b �ӿ�˵����
*       
*
* \b ��������:
*
*       �������
*
* \b ��ʷ��¼:
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2012-11-29   �����ļ�   <br>
*/
#ifndef __TASK_APP_H
#define __TASK_APP_H

/******************************************************************************/
/*                             ͷ�ļ�                                         */
/******************************************************************************/


/******************************************************************************/
/*                             �궨��                                         */
/******************************************************************************/
/* ����ID */
#define APP_TASK_KEY_ID                     9u
#define APP_TASK_START_ID                   6u
#define APP_TASK_UARTTOCAN_ID               7u
/* �������ȼ� */
#define APP_TASK_KEY_PRIO                   APP_TASK_KEY_ID
#define APP_TASK_START_PRIO                 APP_TASK_START_ID
#define APP_TASK_UARTTOCAN_PRIO             APP_TASK_UARTTOCAN_ID
/* ��ʼ�������ջ��С */
#define APP_TASK_KEY_STK_SIZE               128u //256u
#define APP_TASK_START_STK_SIZE             256u
#define APP_TASK_UARTTOCAN_STK_SIZE         256u

/******************************************************************************/
/*                          �ⲿ�ӿں�������                                  */
/******************************************************************************/
void app_task_uarttocan(void *pdata);


#endif /*!< end __TASK_APP_H_ */

/********************************end of file***********************************/
