/**
  ******************************************************************************
  * @file    rtc.h
  * @author  zuokongxiao
  * @version V1.0
  * @date    2011/03/08
  * @brief   RTCÍ·ÎÄ¼þ.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include <time.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RTC_Init(void);
struct tm Time_GetCurTime(void);
void Time_SetCurTime(struct tm tm_time);
void Time_SetAlarmTime(struct tm tm_alarm);


#endif /* __RTC_H */

/**************************end of file*****************************************/

