/**
  ******************************************************************************
  * @file    task_temp.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-16
  * @brief   获取温度值.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_type.h"
#include "task_temp.h"
#include "task_app.h"
#include "ucos_ii.h"
#include "ds18b20.h"
#include "debug.h"
#include "rtc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* 温度消息结构体 */
struct temp_msg stTempMsg;
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/




/* External functions --------------------------------------------------------*/
/**
  * @file   void task_temp(void *pdata)
  * @brief  获取温度 
  * @param  pdata [in] 消息队列指针
  * @retval None
  */
void task_temp(void *pdata)
{
    ErrorStatus ErrorFlag = ERROR;
    float CurTemp = 0.0;
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;   /* Storage for CPU status register */
#endif

    /* 初始化DS18B20 */
    do{
        /* 临界段 */
        OS_ENTER_CRITICAL();
        /* 初始化DS18B20端口 */
        DS18B20_Configuration();
        if(ERROR == DS18B20_WriteScratchpad(0x65, 0x06, 
                            DS18B20_RESOLUTION_BITS_12))
        {
            break;
        }
        OS_EXIT_CRITICAL();
        /*! 延时800ms */
        OSTimeDlyHMSM(0, 0, 0, 800);
        OS_ENTER_CRITICAL();
        if(ERROR == DS18B20_CopyScratchpad())
        {
            break;
        }
        /*! 延时800ms */
        OSTimeDlyHMSM(0, 0, 0, 800);
        ErrorFlag = SUCCESS;
    } while(0);
    OS_EXIT_CRITICAL();

    /* 操作DS1820成功, 任务主循环 */
    while(SUCCESS == ErrorFlag)
    {
        /* 任务延时 */
        OSTimeDlyHMSM(0, 0, 1, 0);
        /* 启动温度转换 */
        OS_ENTER_CRITICAL();
        if(ERROR == DS18B20_StartTempConvert())
        {
            ErrorFlag = ERROR;
            OS_EXIT_CRITICAL();
            break;
        }
        OS_EXIT_CRITICAL();
        /* 转换延时800ms */
        OSTimeDlyHMSM(0, 0, 0, 800);
        /* 读取温度值 */
        OS_ENTER_CRITICAL();
        if(ERROR == DS18B20_ReadTemperature(&CurTemp))
        {
            ErrorFlag = ERROR;
            OS_EXIT_CRITICAL();
            break;
        }
        OS_EXIT_CRITICAL();

        /* 发送消息：当前温度+当前时间 */
        DEBUG_MSG("D: Temp=%02.4f\r\n", CurTemp);
        stTempMsg.MsgNum = 1;
        stTempMsg.CurTemp = CurTemp;
        stTempMsg.CurTime = Time_GetCurTime(); 
        /* 发送消息 */
        if(OS_NO_ERR != OSQPost(pdata, &stTempMsg))
        {
            ERROR_MSG("E: Send message to queue! %s %d\r\n", \
                __FILE__, __LINE__);
        }
#if 0
        else
        {
            DEBUG_MSG("D: Send message to queue success! %s %d\r\n", \
                __FILE__, __LINE__);
        }
#endif
    }

    /* 初始化或读取DS18B20错误 */
    ERROR_MSG("E: Init or Read DS18B20!\r\n");
    /* 错误：挂起此任务 */
    //OSTaskSuspend(TASK_TEMP_PRIO);

    for(;;)
    {
        /* 任务延时 */
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}



/**************************end of file*****************************************/

