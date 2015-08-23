/**
  ******************************************************************************
  * @file    task_app.h
  * @author  zuokongxiao
  * @version 
  * @date    2012-10-16
  * @brief   .
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

#ifndef __TASK_APP_H
#define __TASK_APP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* 任务ID */
#define TASK_START_ID                   2u
#define TASK_COMM_ID                    3u
#define TASK_TEMP_ID                    5u
#define TASK_LCD_ID                     10u
/* 任务优先级 */
#define TASK_START_PRIO                 TASK_START_ID
#define TASK_COMM_PRIO                  TASK_COMM_ID
#define TASK_TEMP_PRIO                  TASK_TEMP_ID
#define TASK_LCD_PRIO                   TASK_LCD_ID
/* 初始化任务堆栈大小 */
#define TASK_START_STK_SIZE             128u
#define TASK_COMM_STK_SIZE              512u
#define TASK_TEMP_STK_SIZE              512u
#define TASK_LCD_STK_SIZE               512u
/* Exported macro ------------------------------------------------------------*/
/* ExPorted variables --------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
void start_ucos(void);


#ifdef __cplusplus
}
#endif

#endif /* end __TASK_APP_H */
/**************************end of file*****************************************/

