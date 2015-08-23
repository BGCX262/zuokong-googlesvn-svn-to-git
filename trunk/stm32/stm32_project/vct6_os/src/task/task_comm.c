/**
  ******************************************************************************
  * @file    task_comm.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-21
  * @brief   解析通信协议.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ucos_ii.h"
#include "stm32f10x_type.h"
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/




/* External functions --------------------------------------------------------*/
/**
  * @file   void task_comm(void *pdata)
  * @brief  通信协议解析任务 
  * @param  pdata [in] 建立任务时传递过来的指针
  * @retval None
  */
void task_comm(void *pdata)
{
    /* 防止编译器告警 */
    pdata = pdata;

    /* 任务主循环 */
    for(;;)
    {
        /* 任务延时 */
        OSTimeDlyHMSM(0, 0, 1, 500);
#if 0
        if(EXIT_FAILURE == parse_protocal())
        {
            DEBUG_MSG("D: parser protocal! %s %d\r\n", __FILE__, __LINE__);
        }
#endif
    } /* end for(;;)... */
}



/**************************end of file*****************************************/

