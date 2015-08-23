/*! \file  task_app.h
* \b 接口说明：
*       
*
* \b 功能描述:
*
*       任务相关
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2012-11-29   创建文件   <br>
*/
#ifndef __TASK_APP_H
#define __TASK_APP_H

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/


/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
/* 任务ID */
#define APP_TASK_KEY_ID                     5u
#define APP_TASK_START_ID                   6u
#define APP_TASK_UARTTOCAN_ID               7u
/* 任务优先级 */
#define APP_TASK_KEY_PRIO                   APP_TASK_KEY_ID
#define APP_TASK_START_PRIO                 APP_TASK_START_ID
#define APP_TASK_UARTTOCAN_PRIO             APP_TASK_UARTTOCAN_ID
/* 初始化任务堆栈大小 */
#define APP_TASK_KEY_STK_SIZE               256u
#define APP_TASK_START_STK_SIZE             256u
#define APP_TASK_UARTTOCAN_STK_SIZE         256u

/******************************************************************************/
/*                          外部接口函数声明                                  */
/******************************************************************************/
void app_task_uarttocan(void *pdata);
void app_task_key(void *pdata);


#endif /*!< end __TASK_APP_H_ */

/********************************end of file***********************************/

