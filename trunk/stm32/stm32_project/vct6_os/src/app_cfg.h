/*******************************************************************************
* File Name          : app_cfg.h
* Description        : 头文件.

* Version            : V1.0
* Date               : 2011/04/03
* Author             : zuokong
* Description        : 创建.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_CFG_H
#define __APP_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
// RTC_EN
#define _RTC_EN                     1 // 1: enable rtc; 0: disable rtc.

/* ucos-ii相关 ---------------------------------------------------------------*/
// 任务ID 
#define TASK_START_ID               2
#define TASK_TOUCH_ID               3
#define TASK_RTC_ID                 4                 
// 任务优先级
#define TASK_START_PRIO             TASK_START_ID 
#define TASK_TOUCH_PRIO             TASK_TOUCH_ID   
#define TASK_RTC_PRIO               TASK_RTC_ID
// 初始化任务堆栈大小
#define TASK_START_STK_SIZE         1024               
#define TASK_TOUCH_STK_SIZE         1024
#define TASK_RTC_STK_SIZE           4096

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern OS_STK TaskStartStk[TASK_START_STK_SIZE];
extern OS_EVENT *TimeSecSem;
/* Exported functions ------------------------------------------------------- */
void TaskStart(void *pdata);


#endif /* __APP_CFG_H */

/**************************END OF FILE*****************************************/

