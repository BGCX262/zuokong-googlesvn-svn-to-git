/*******************************************************************************
* File Name          : app.c
* Description        : 通过串口测试VCT6(STM32F103VCT6 8MHz, 256K Flash, 48K Ram).

* Version            : V1.0
* Date               : 2011/07/13
* Author             : zuokong.
* Description        : 创建.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "tft.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Priv ate macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* usb report buffer */
/* ucos-ii */ 
OS_STK TaskStartStk[TASK_START_STK_SIZE];       // Start task stack 
OS_STK TaskTouchStk[TASK_TOUCH_STK_SIZE];       // Touch Pannel task stack
OS_STK TaskRTCStk[TASK_RTC_STK_SIZE];           // RTC task stack
OS_EVENT *TimeSecSem;                           // 秒中断信号量
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void TaskStartCreateTasks(void);
void TaskTouch(void *pdata);
void TaskRTC(void *pdata);




/*******************************************************************************
* Function Name  : TaskStartCreateTasks
* Description    : 创建其他任务.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TaskStartCreateTasks(void)
{
    /* create TouchPannel task */
    OSTaskCreateExt(TaskTouch,
                    (void *)0,
                    (OS_STK *)&TaskTouchStk[TASK_TOUCH_STK_SIZE - 1],
                    TASK_TOUCH_PRIO,
                    TASK_TOUCH_ID,
                    (OS_STK *)&TaskTouchStk[0],
                    TASK_TOUCH_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

   /* create RTC task */
    OSTaskCreateExt(TaskRTC,
                    (void *)0,
                    (OS_STK *)&TaskRTCStk[TASK_RTC_STK_SIZE - 1],
                    TASK_RTC_PRIO,
                    TASK_RTC_ID,
                    (OS_STK *)&TaskRTCStk[0],
                    TASK_RTC_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

/*
********************************************************************************
*
*                           uC/OS-II task code
*
********************************************************************************
*/

/*******************************************************************************
* Function Name  : TaskStart
* Description    : UCOS-II.
* Input          : *pdata.
* Output         : None
* Return         : None
*******************************************************************************/
void TaskStart(void *pdata)
{
    //u16 tp_xvalue = 0, tp_yvalue = 0;
    //INT8U  err = 0;

    pdata = pdata;                   // 防止编译器告警
    SysTick_Configuration();         // 初始化系统节拍(1ms)
    OSStatInit();                    // 初始化统计任务
    TaskStartCreateTasks();          // 建立其他任务

    /* 初始化TFT, 设置TFT背光 */
    TFT_Init();
    TFT_SetBlackLight(TFT_BL_PERCENT_90);

    TFT_Clear(Blue);

    TFT_DisplayD36x48(10,  150, '2', White, Blue);
    TFT_DisplayD36x48(46,  150, '1', White, Blue);
    TFT_DisplayD36x48(82,  150, ':', White, Blue);
    TFT_DisplayD36x48(118, 150, '5', White, Blue);
    TFT_DisplayD36x48(154, 150, '8', White, Blue);
    TFT_DisplayString(200, 182, "2011", White, Blue);

    TFT_DisplayString(30, 218, "Wendnesday, Septermber 23", White, Blue);
    TFT_DrawHLine(30, 236, 230, Blue);

    while(1)
    {
        //printf("Start task is running!\r\n");
        /* 任务延时500ms */
        OSTimeDlyHMSM(0, 0, 20, 0);
    }
}

/*******************************************************************************
* Function Name  : TaskRTC
* Description    : RTC task.
* Input          :  - *pdata
* Output         : None.
* Return         : None.
*******************************************************************************/
void TaskRTC(void *pdata)
{
    struct tm current_time;             // 时间结构体
    INT8U  err = 0;

    pdata = pdata;                      // 防止编译器告警
    TimeSecSem = OSSemCreate(0);        // 创建时间信号量
    OSTimeDlyHMSM(0, 0, 2, 0);

#if 0
    current_time.tm_year = 2012;
    current_time.tm_mon = 8;
    current_time.tm_mday = 4;
    current_time.tm_hour = 0;
    current_time.tm_min = 44;
    current_time.tm_sec = 0;
    Time_SetCurTime(current_time);
#endif

    while(1)
    {
        OSSemPend(TimeSecSem, 0, &err);   // 等待信号量
        current_time = Time_GetCurTime(); // 获取时间 
        /* 从串口打印时间 */
        
        printf("%d-%d-%d %d %02d:%02d:%02d\r\n", \
                current_time.tm_year, current_time.tm_mon, current_time.tm_mday, \
                current_time.tm_wday, \
                current_time.tm_hour, current_time.tm_min, current_time.tm_sec);
        
        TFT_DisplayD36x48(10,  150, current_time.tm_hour / 10 + '0', White, Blue);
        TFT_DisplayD36x48(46,  150, current_time.tm_hour % 10 + '0', White, Blue);
        TFT_DisplayD36x48(118, 150, current_time.tm_min / 10 + '0', White, Blue);
        TFT_DisplayD36x48(154, 150, current_time.tm_min % 10 + '0', White, Blue);
    }
}

/*******************************************************************************
* Function Name  : TaskTouch
* Description    : Touch Pannel task.
* Input          : - *pdata.
* Output         : None
* Return         : None
*******************************************************************************/
void TaskTouch(void *pdata)
{
    float temp = 0.0;
    char str_buf[32];

    pdata = pdata;                // 防止编译器告警
    DS18B20_Configuration(12);
    while(1)
    {
        OSTimeDlyHMSM(0, 0, 0, 500);        // 任务延时3s
        
        temp = DS18B20_ReadTemperature();
        strcpy(str_buf, FLOAT2STR(temp, 4));

        TFT_DisplayD36x48(10,  70, str_buf[1], White, Blue);
        TFT_DisplayD36x48(46,  70, str_buf[2], White, Blue);
        TFT_DisplayD36x48(82,  70, '.', White, Blue);
        TFT_DisplayD36x48(118,  70, str_buf[4], White, Blue);
        TFT_DisplayD36x48(154,  70, str_buf[5], White, Blue);
        TFT_DisplayD36x48(190,  70, str_buf[6], White, Blue);

        printf("%s\r\n", str_buf);
          
    } // end while(1)...
}













/*******************************************************************************
*
*      other code
*
*******************************************************************************/

/*******************************************************************************
* Function Name  : SendReport
* Description    : 返回报告.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/


/**************************END OF FILE*****************************************/

