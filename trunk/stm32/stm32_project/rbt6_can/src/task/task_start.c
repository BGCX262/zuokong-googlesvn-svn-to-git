/*! \file  task_start.c
* \b �ӿ�˵����
*       
*
* \b ��������:
*
*       ��ʼ����ʵ��������ϵͳ
*
* \b ��ʷ��¼:
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2012-11-29   �����ļ�   <br>
*/

/******************************************************************************/
/*                             ͷ�ļ�                                         */
/******************************************************************************/
#include "ucos_ii.h"
#include "os_cpu.h"
#include "task_app.h"
#include "bsp.h"
#include "led.h"
#include "uart.h"
#include "debug.h"


/******************************************************************************/
/*                             �궨��                                         */
/******************************************************************************/
/*! ��Ϣ���г��� */
#define MSG_QUEUE_SIZE          (10u)

/******************************************************************************/
/*                            ��������                                        */
/******************************************************************************/
/*! ע�⣺�˴����������Ϊ8�ֽڶ��� */
/*! ��ʼ����ջ */
__align(8) OS_STK os_stkAppTaskStartStk[APP_TASK_START_STK_SIZE] = {0};
/*! ��������ջ */
__align(8) OS_STK os_stkAppTaskKeyStk[APP_TASK_KEY_STK_SIZE] = {0};
/*! ����תCAN����ջ */
__align(8) OS_STK os_stkAppTaskUartToCanStk[APP_TASK_UARTTOCAN_STK_SIZE] = {0};
/* ��Ϣ�������� */
void *MsgQueue[MSG_QUEUE_SIZE];
/* �¼� */
OS_EVENT *pMsg;

/******************************************************************************/
/*                          �ڲ��ӿ�����                                      */
/******************************************************************************/




/******************************************************************************/
/*                          �ڲ��ӿ�ʵ��                                      */
/******************************************************************************/
/*! \fn       void create_tasks(void)
*
*  \brief     ������������
*
*  \param     ��
*
*  \exception ��������ʱ����Ϣ����ָ�봫�ݸ���������
*
*  \return    ��
*/

void create_tasks(void)
{
    /*! ����ת������ */
    OSTaskCreateExt(app_task_uarttocan,
                    (void *)0,
                    (OS_STK *)&os_stkAppTaskUartToCanStk[APP_TASK_UARTTOCAN_STK_SIZE - 1],
                    APP_TASK_UARTTOCAN_PRIO,
                    APP_TASK_UARTTOCAN_ID,
                    (OS_STK *)&os_stkAppTaskUartToCanStk[0],
                    APP_TASK_UARTTOCAN_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#if 0
    /*! ������������ */
    OSTaskCreateExt(app_task_key,
                    pMsg, //(void *)0,
                    (OS_STK *)&os_stkAppTaskKeyStk[APP_TASK_KEY_STK_SIZE - 1],
                    APP_TASK_KEY_PRIO,
                    APP_TASK_KEY_ID,
                    (OS_STK *)&os_stkAppTaskKeyStk[0],
                    APP_TASK_KEY_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif
}

/*! \fn       void app_task_start(void *pdata)
*
*  \brief     ��ʼ����
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
void app_task_start(void *pdata)
{
    /* ��ֹ�������澯 */
    pdata = pdata;
    /* ����systick */
    systick_configuration();
    /* ��ʼ��ͳ������ */
    OSStatInit();
    /*! ��ʼ��LED */
    led_init();
    /*! ��ʼ������ */
    uart_init();
    /* ��ӡTaskStart */
    DEBUG_MSG("\r\nD: Start uCosII success!\r\n");
    /* ������Ϣ���� */
    //pMsg = OSQCreate(MsgQueue, MSG_QUEUE_SIZE);
    //if((OS_EVENT *)0 == pMsg)
    //{
    //    DEBUG_MSG("\r\nE: OSQCreate error!\r\n");
    //    while(1);
    //}
    /* ������������ */
    create_tasks();
    
    /*! ����ѭ�� */
    for( ; ; )
    {
        /* ������ʱ */
        OSTimeDlyHMSM(0, 0, 0, 500);
        LED2 = LED1;
        LED1 = ~LED1;
        /* suspend task */
        //OSTaskSuspend(OS_PRIO_SELF);
    }
}

/******************************************************************************/
/*                          �ⲿ�ӿ�ʵ��                                      */
/******************************************************************************/
/*! \fn       void start_ucos(void)
*
*  \brief     ����ϵͳ
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
void start_ucos(void)
{
    /* ��ʼ��ucos-ii */
    OSInit();
    /* ����ucos��ʼ���� TaskStart */
    OSTaskCreateExt(app_task_start, 
                    (void *)0, 
                    (OS_STK *)&os_stkAppTaskStartStk[APP_TASK_START_STK_SIZE - 1],
                    APP_TASK_START_PRIO,
                    APP_TASK_START_ID,
                    (OS_STK *)&os_stkAppTaskStartStk[0],
                    APP_TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    
    /* ���������񻷾� */
    OSStart();
}

/********************************end of file***********************************/
