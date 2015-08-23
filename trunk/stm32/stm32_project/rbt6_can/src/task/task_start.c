/*! \file  task_start.c
* \b 接口说明：
*       
*
* \b 功能描述:
*
*       起始任务实现与启动系统
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2012-11-29   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "ucos_ii.h"
#include "os_cpu.h"
#include "task_app.h"
#include "bsp.h"
#include "led.h"
#include "uart.h"
#include "debug.h"


/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
/*! 消息队列长度 */
#define MSG_QUEUE_SIZE          (10u)

/******************************************************************************/
/*                            变量定义                                        */
/******************************************************************************/
/*! 注意：此处定义的数组为8字节对齐 */
/*! 起始任务栈 */
__align(8) OS_STK os_stkAppTaskStartStk[APP_TASK_START_STK_SIZE] = {0};
/*! 按键任务栈 */
__align(8) OS_STK os_stkAppTaskKeyStk[APP_TASK_KEY_STK_SIZE] = {0};
/*! 串口转CAN任务栈 */
__align(8) OS_STK os_stkAppTaskUartToCanStk[APP_TASK_UARTTOCAN_STK_SIZE] = {0};
/* 消息队列数组 */
void *MsgQueue[MSG_QUEUE_SIZE];
/* 事件 */
OS_EVENT *pMsg;

/******************************************************************************/
/*                          内部接口声明                                      */
/******************************************************************************/




/******************************************************************************/
/*                          内部接口实现                                      */
/******************************************************************************/
/*! \fn       void create_tasks(void)
*
*  \brief     创建其他任务
*
*  \param     无
*
*  \exception 创建任务时将消息队列指针传递给其他任务
*
*  \return    无
*/

void create_tasks(void)
{
    /*! 创建转换任务 */
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
    /*! 创建按键任务 */
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
*  \brief     起始任务
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void app_task_start(void *pdata)
{
    /* 防止编译器告警 */
    pdata = pdata;
    /* 配置systick */
    systick_configuration();
    /* 初始化统计任务 */
    OSStatInit();
    /*! 初始化LED */
    led_init();
    /*! 初始化串口 */
    uart_init();
    /* 打印TaskStart */
    DEBUG_MSG("\r\nD: Start uCosII success!\r\n");
    /* 创建消息队列 */
    //pMsg = OSQCreate(MsgQueue, MSG_QUEUE_SIZE);
    //if((OS_EVENT *)0 == pMsg)
    //{
    //    DEBUG_MSG("\r\nE: OSQCreate error!\r\n");
    //    while(1);
    //}
    /* 建立其他任务 */
    create_tasks();
    
    /*! 任务循环 */
    for( ; ; )
    {
        /* 任务延时 */
        OSTimeDlyHMSM(0, 0, 0, 500);
        LED2 = LED1;
        LED1 = ~LED1;
        /* suspend task */
        //OSTaskSuspend(OS_PRIO_SELF);
    }
}

/******************************************************************************/
/*                          外部接口实现                                      */
/******************************************************************************/
/*! \fn       void start_ucos(void)
*
*  \brief     启动系统
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void start_ucos(void)
{
    /* 初始化ucos-ii */
    OSInit();
    /* 创建ucos起始任务 TaskStart */
    OSTaskCreateExt(app_task_start, 
                    (void *)0, 
                    (OS_STK *)&os_stkAppTaskStartStk[APP_TASK_START_STK_SIZE - 1],
                    APP_TASK_START_PRIO,
                    APP_TASK_START_ID,
                    (OS_STK *)&os_stkAppTaskStartStk[0],
                    APP_TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    
    /* 启动多任务环境 */
    OSStart();
}

/********************************end of file***********************************/

