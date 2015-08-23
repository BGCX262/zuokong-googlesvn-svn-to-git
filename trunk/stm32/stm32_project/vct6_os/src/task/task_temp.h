/**
  ******************************************************************************
  * @file    task_temp.h
  * @author  zuokongxiao
  * @version v1.0
  * @date    2012-10-19
  * @brief   ��ȡ�¶�ֵ.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

#ifndef __TASK_TEMP_H
#define __TASK_TEMP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <time.h>

/* Exported types ------------------------------------------------------------*/
struct temp_msg
{
    u8 MsgNum; /* ��Ϣ�� */
    float CurTemp; /* ��ǰ�¶�ֵ */
    //u32 CurTimeCounter; /* ��ǰʱ�� */
    struct tm CurTime;
};
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* ExPorted variables --------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
void task_temp(void *pdata);




#ifdef __cplusplus
}
#endif

#endif /* end __TASK_TEMP_H */

/**************************end of file*****************************************/
