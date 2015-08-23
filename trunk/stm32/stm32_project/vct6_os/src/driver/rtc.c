/**
  ******************************************************************************
  * @file    rtc.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-19
  * @brief   RTC.(注意：VBAT一定接电池或接电容到VDD)
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_bkp.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "debug.h"
#include "rtc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* 调试RTC */
#define DEBUG_RTC_EN            0u
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/
/**
  * @file   RTC_Configuration
  * @brief  配置rtc外设
  * @param  None
  * @retval None
  */
static void RTC_Configuration(void)
{   
    BKP_DeInit(); /* 复位Backup寄存器 */
    RCC_LSEConfig(RCC_LSE_ON); /* 使能LSE */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); /* 等待LSE就绪 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); /* 选择LSE为RTC时钟源 */
    RCC_RTCCLKCmd(ENABLE); /* 开启RTC时钟 */
    RTC_WaitForSynchro(); /* 等待RTC寄存器同步 */
    RTC_WaitForLastTask(); /* 等待上次写操作完成 */
    RTC_SetPrescaler(32767); /* (32.768 KHz)/(32767+1) */
    RTC_WaitForLastTask(); /* 等待上次写操作完成 */
}




/* External functions --------------------------------------------------------*/
/**
  * @file   void RTC_Init(void)
  * @brief  初始化RTC.
  * @param  None
  * @retval None
  */
void RTC_Init(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    if(BKP_ReadBackupRegister(BKP_DR1) != 0xAA55)
    { 
        RTC_Configuration(); /* 配置RTC */
        BKP_WriteBackupRegister(BKP_DR1, 0xAA55); /* 写数据到备份寄存器1 */
#if DEBUG_RTC_EN > 0
        DEBUG_MSG("D: Configure RTC!\r\n");
#endif
    }
    else
    {
#if DEBUG_RTC_EN > 0
        if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            DEBUG_MSG("D: Power On Reset occurred!\r\n");
        }
        else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            DEBUG_MSG("D: External Reset occurred!\r\n");
        }
        DEBUG_MSG("D: No need to configure RTC!\r\n");
#endif
        RTC_WaitForSynchro(); /* 等待RTC寄存器同步 */
    }
    /* 使能中断 */
    RTC_ITConfig(RTC_IT_SEC | RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask(); /* 等待上次写操作完成 */
    RCC_ClearFlag();
}

/**
  * @file   struct tm Time_GetCurTime(void)
  * @brief  获取当前时间.
  * @param  None
  * @retval 返回当前时间(struct tm 结构体)(具体看time.h库)
  */
struct tm Time_GetCurTime(void)
{ 
    time_t current;
    struct tm *tm_time;

    current = (time_t)RTC_GetCounter(); /* 读RTC计数寄存器 */
    tm_time = localtime(&current); /* 时间格式转换 */
    tm_time->tm_year += 1900; /* 1900年 + tm_year */
    tm_time->tm_mon += 1;

    return *tm_time;
}

/**
  * @file   void Time_SetCurTime(struct tm tm_time)
  * @brief  设置当前时间
  * @param  tm_time [in] 设定的时间
  * @retval None
  */
void Time_SetCurTime(struct tm tm_time)
{
    time_t current;

    tm_time.tm_year -= 1900; /* year-1900 */
    tm_time.tm_mon -= 1;
    current = mktime(&tm_time); /* 将分解时间转换为日历时间 */
    RTC_WaitForLastTask(); /* 等待上次写操作完成 */
    RTC_SetCounter((u32)current); /* 写RTC计数寄存器 */
    RTC_WaitForLastTask(); /* 等待上次写操作完成 */
}

/**
  * @file   void Time_SetAlarmTime(struct tm tm_alarm)
  * @brief  设置闹钟时间(时间到产生中断)
  * @param  tm_alarm [in] 闹钟时间
  * @retval None
  */
void Time_SetAlarmTime(struct tm tm_alarm)
{
    time_t alarm;

    tm_alarm.tm_year -= 1900; /* year-1900 */
    tm_alarm.tm_mon -= 1; 
    alarm = mktime(&tm_alarm); /* 将分解时间转换为日历时间 */
    RTC_WaitForLastTask(); /* 等待上次写操作完成 */
    RTC_SetAlarm((u32)alarm); /* 写RTC闹钟寄存器 */
    RTC_WaitForLastTask(); /* 等待上次写操作完成 */
}


/**************************END OF FILE*****************************************/

