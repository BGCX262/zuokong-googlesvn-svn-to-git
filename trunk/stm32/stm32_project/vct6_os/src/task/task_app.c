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
/* 起始任务 */
OS_STK os_stkTaskStartStk[TASK_START_STK_SIZE] = {0};
/* LCD任务 */
OS_STK os_stkTaskLcdStk[TASK_LCD_STK_SIZE] = {0};
/* 温度任务 */
OS_STK os_stkTaskTempStk[TASK_TEMP_STK_SIZE] = {0};
/* 通信协议解析任务 */
OS_STK os_stkTaskCommStk[TASK_COMM_STK_SIZE] = {0};
/* 消息数组 */
void *MsgQueue[MSG_QUEUE_SIZE];
/* 事件 */
OS_EVENT *pMsg;
/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
/**
  * @file   void create_tasks(void)
  * @brief  创建其他任务.
  * @param  None
  * @retval None
  */
void create_tasks(void)
{
    /* 创建LCD任务 */
    OSTaskCreateExt(task_lcd,
                    pMsg,//(void *)0,
                    (OS_STK *)&os_stkTaskLcdStk[TASK_LCD_STK_SIZE - 1],
                    TASK_LCD_PRIO,
                    TASK_LCD_ID,
                    (OS_STK *)&os_stkTaskLcdStk[0],
                    TASK_LCD_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    /* 创建task_temp */
    OSTaskCreateExt(task_temp,
                    pMsg,//(void *)0,
                    (OS_STK *)&os_stkTaskTempStk[TASK_TEMP_STK_SIZE - 1],
                    TASK_TEMP_PRIO,
                    TASK_TEMP_ID,
                    (OS_STK *)&os_stkTaskTempStk[0],
                    TASK_TEMP_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    /* 创建协议解析任务 */
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
  * @brief  起始任务
  * @param  None
  * @retval None
  */
void task_start(void *pdata)
{
    /* 防止编译器告警 */
    pdata = pdata;

    /* 配置systick */
    systick_configuration();
    /* 初始化统计任务 */
    OSStatInit();
    /* 创建消息队列 */
    pMsg = OSQCreate(MsgQueue, MSG_QUEUE_SIZE);

    /* 初始化串口 */
    uart_configuration(); /* 配置串口1 */
    /* 打印TaskStart */
    DEBUG_MSG("\r\nD: Start uCosII success!\r\n");

    /* 建立其他任务 */
    create_tasks();
    /* suspend but not delete */
    OSTaskSuspend(TASK_START_ID);
}







/* External functions --------------------------------------------------------*/
/**
  * @file   void start_ucos(void)
  * @brief  启动任务
  * @param  None
  * @retval None
  */
void start_ucos(void)
{
    /* 初始化ucos-ii */
    OSInit();
    /* 创建ucos起始任务 TaskStart */
    OSTaskCreateExt(task_start, 
                    (void *)0, 
                    (OS_STK *)&os_stkTaskStartStk[TASK_START_STK_SIZE - 1],
                    TASK_START_PRIO,
                    TASK_START_ID,
                    (OS_STK *)&os_stkTaskStartStk[0],
                    TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    
    /* 启动多任务环境 */
    OSStart();
}



/**************************end of file*****************************************/

