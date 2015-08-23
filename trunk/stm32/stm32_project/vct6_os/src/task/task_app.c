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
#include "ucos_ii.h"
#include "stm32f10x.h"
#include "task_app.h"
#include "task_lcd.h"
#include "task_temp.h"
#include "task_comm.h"
#include "debug.h"
#include "bsp.h"
#include "uart.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MSG_QUEUE_SIZE          (2u)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* ��ʼ���� */
OS_STK os_stkTaskStartStk[TASK_START_STK_SIZE] = {0};
/* LCD���� */
OS_STK os_stkTaskLcdStk[TASK_LCD_STK_SIZE] = {0};
/* �¶����� */
OS_STK os_stkTaskTempStk[TASK_TEMP_STK_SIZE] = {0};
/* ͨ��Э��������� */
OS_STK os_stkTaskCommStk[TASK_COMM_STK_SIZE] = {0};
/* ��Ϣ���� */
void *MsgQueue[MSG_QUEUE_SIZE];
/* �¼� */
OS_EVENT *pMsg;
/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
/**
  * @file   void create_tasks(void)
  * @brief  ������������.
  * @param  None
  * @retval None
  */
void create_tasks(void)
{
    /* ����LCD���� */
    OSTaskCreateExt(task_lcd,
                    pMsg,//(void *)0,
                    (OS_STK *)&os_stkTaskLcdStk[TASK_LCD_STK_SIZE - 1],
                    TASK_LCD_PRIO,
                    TASK_LCD_ID,
                    (OS_STK *)&os_stkTaskLcdStk[0],
                    TASK_LCD_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    /* ����task_temp */
    OSTaskCreateExt(task_temp,
                    pMsg,//(void *)0,
                    (OS_STK *)&os_stkTaskTempStk[TASK_TEMP_STK_SIZE - 1],
                    TASK_TEMP_PRIO,
                    TASK_TEMP_ID,
                    (OS_STK *)&os_stkTaskTempStk[0],
                    TASK_TEMP_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    /* ����Э��������� */
    OSTaskCreateExt(task_comm,
                    (void *)0,
                    (OS_STK *)&os_stkTaskCommStk[TASK_COMM_STK_SIZE - 1],
                    TASK_COMM_PRIO,
                    TASK_COMM_ID,
                    (OS_STK *)&os_stkTaskCommStk[0],
                    TASK_COMM_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

/**
  * @file   void task_start(void *pdata)
  * @brief  ��ʼ����
  * @param  None
  * @retval None
  */
void task_start(void *pdata)
{
    /* ��ֹ�������澯 */
    pdata = pdata;

    /* ����systick */
    systick_configuration();
    /* ��ʼ��ͳ������ */
    OSStatInit();
    /* ������Ϣ���� */
    pMsg = OSQCreate(MsgQueue, MSG_QUEUE_SIZE);

    /* ��ʼ������ */
    uart_configuration(); /* ���ô���1 */
    /* ��ӡTaskStart */
    DEBUG_MSG("\r\nD: Start uCosII success!\r\n");

    /* ������������ */
    create_tasks();
    /* suspend but not delete */
    OSTaskSuspend(TASK_START_ID);
}







/* External functions --------------------------------------------------------*/
/**
  * @file   void start_ucos(void)
  * @brief  ��������
  * @param  None
  * @retval None
  */
void start_ucos(void)
{
    /* ��ʼ��ucos-ii */
    OSInit();
    /* ����ucos��ʼ���� TaskStart */
    OSTaskCreateExt(task_start, 
                    (void *)0, 
                    (OS_STK *)&os_stkTaskStartStk[TASK_START_STK_SIZE - 1],
                    TASK_START_PRIO,
                    TASK_START_ID,
                    (OS_STK *)&os_stkTaskStartStk[0],
                    TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    
    /* ���������񻷾� */
    OSStart();
}



/**************************end of file*****************************************/
