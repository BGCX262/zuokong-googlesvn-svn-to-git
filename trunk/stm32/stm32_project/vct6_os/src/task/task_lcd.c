/**
  ******************************************************************************
  * @file    task_app.c
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ucos_ii.h"
#include "stm32f10x_type.h"
#include "task_temp.h"
#include "tft.h"
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/




/* External functions --------------------------------------------------------*/
/**
  * @file   void task_lcd(void *pdata)
  * @brief  lcd���� 
  * @param  pdata [in] ��������ʱ���ݹ�����ָ��
  * @retval None
  */
void task_lcd(void *pdata)
{
    u8 err = 0, TempStr[8] = {0};
    struct temp_msg *pTempMsg;

    /* ��ʼ��TFT */
    TFT_Init();
    /* ����TFT���� */
    TFT_SetBlackLight(TFT_BL_PERCENT_90);
    /* ���� */
    TFT_Clear(Blue);
    TFT_DisplayD36x48(10,  150, '2', White, Blue);
    TFT_DisplayD36x48(46,  150, '1', White, Blue);
    TFT_DisplayD36x48(82,  150, ':', White, Blue);
    TFT_DisplayD36x48(118, 150, '5', White, Blue);
    TFT_DisplayD36x48(154, 150, '8', White, Blue);
    TFT_DisplayString(200, 182, "2011", White, Blue);
    TFT_DisplayString(30, 218, "Wendnesday, Septermber 23", White, Blue);
    TFT_DrawHLine(30, 236, 230, Blue);

    /* ������ѭ�� */
    for(;;)
    {
        /* ������ʱ */
        OSTimeDlyHMSM(0, 0, 0, 500);
        /* �Ӷ����л�ȡ��Ϣ */
        pTempMsg = OSQAccept(pdata, &err);
        if((NULL == pTempMsg) || (OS_ERR_NONE != err))
        {
            continue;
        }
        /* ��Ϣ�� */
        if(1 == pTempMsg->MsgNum)
        {
            /* ��ʽ���¶�ֵ */
            sprintf((char *)TempStr, "%08.4f", pTempMsg->CurTemp);
            //DEBUG_MSG("%s\r\n", TempStr);
            /* ��ʾ�¶� */
            TFT_DisplayD36x48(10,  70, TempStr[1], White, Blue);
            TFT_DisplayD36x48(46,  70, TempStr[2], White, Blue);
            TFT_DisplayD36x48(82,  70, TempStr[3], White, Blue);
            TFT_DisplayD36x48(118, 70, TempStr[4], White, Blue);
            TFT_DisplayD36x48(154, 70, TempStr[5], White, Blue);
            TFT_DisplayD36x48(190, 70, TempStr[6], White, Blue);
            /* ��ʾʱ�� */
            TFT_DisplayD36x48(10,  150, pTempMsg->CurTime.tm_hour / 10 + '0', White, Blue);
            TFT_DisplayD36x48(46,  150, pTempMsg->CurTime.tm_hour % 10 + '0', White, Blue);
            TFT_DisplayD36x48(118, 150, pTempMsg->CurTime.tm_min / 10 + '0', White, Blue);
            TFT_DisplayD36x48(154, 150, pTempMsg->CurTime.tm_min % 10 + '0', White, Blue);
        }
        else if(2 == pTempMsg->MsgNum)
        {
            DEBUG_MSG("2\r\n");
        }
    } /* end for(;;)... */
}



/**************************end of file*****************************************/
