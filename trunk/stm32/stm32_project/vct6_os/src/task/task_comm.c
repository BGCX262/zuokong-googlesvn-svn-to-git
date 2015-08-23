/**
  ******************************************************************************
  * @file    task_comm.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-21
  * @brief   ����ͨ��Э��.
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
  * @brief  ͨ��Э��������� 
  * @param  pdata [in] ��������ʱ���ݹ�����ָ��
  * @retval None
  */
void task_comm(void *pdata)
{
    /* ��ֹ�������澯 */
    pdata = pdata;

    /* ������ѭ�� */
    for(;;)
    {
        /* ������ʱ */
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
