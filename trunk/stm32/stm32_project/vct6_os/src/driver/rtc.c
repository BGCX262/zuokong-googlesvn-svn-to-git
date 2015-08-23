/**
  ******************************************************************************
  * @file    rtc.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-19
  * @brief   RTC.(ע�⣺VBATһ���ӵ�ػ�ӵ��ݵ�VDD)
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
/* ����RTC */
#define DEBUG_RTC_EN            0u
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/
/**
  * @file   RTC_Configuration
  * @brief  ����rtc����
  * @param  None
  * @retval None
  */
static void RTC_Configuration(void)
{   
    BKP_DeInit(); /* ��λBackup�Ĵ��� */
    RCC_LSEConfig(RCC_LSE_ON); /* ʹ��LSE */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); /* �ȴ�LSE���� */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); /* ѡ��LSEΪRTCʱ��Դ */
    RCC_RTCCLKCmd(ENABLE); /* ����RTCʱ�� */
    RTC_WaitForSynchro(); /* �ȴ�RTC�Ĵ���ͬ�� */
    RTC_WaitForLastTask(); /* �ȴ��ϴ�д������� */
    RTC_SetPrescaler(32767); /* (32.768 KHz)/(32767+1) */
    RTC_WaitForLastTask(); /* �ȴ��ϴ�д������� */
}




/* External functions --------------------------------------------------------*/
/**
  * @file   void RTC_Init(void)
  * @brief  ��ʼ��RTC.
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
        RTC_Configuration(); /* ����RTC */
        BKP_WriteBackupRegister(BKP_DR1, 0xAA55); /* д���ݵ����ݼĴ���1 */
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
        RTC_WaitForSynchro(); /* �ȴ�RTC�Ĵ���ͬ�� */
    }
    /* ʹ���ж� */
    RTC_ITConfig(RTC_IT_SEC | RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask(); /* �ȴ��ϴ�д������� */
    RCC_ClearFlag();
}

/**
  * @file   struct tm Time_GetCurTime(void)
  * @brief  ��ȡ��ǰʱ��.
  * @param  None
  * @retval ���ص�ǰʱ��(struct tm �ṹ��)(���忴time.h��)
  */
struct tm Time_GetCurTime(void)
{ 
    time_t current;
    struct tm *tm_time;

    current = (time_t)RTC_GetCounter(); /* ��RTC�����Ĵ��� */
    tm_time = localtime(&current); /* ʱ���ʽת�� */
    tm_time->tm_year += 1900; /* 1900�� + tm_year */
    tm_time->tm_mon += 1;

    return *tm_time;
}

/**
  * @file   void Time_SetCurTime(struct tm tm_time)
  * @brief  ���õ�ǰʱ��
  * @param  tm_time [in] �趨��ʱ��
  * @retval None
  */
void Time_SetCurTime(struct tm tm_time)
{
    time_t current;

    tm_time.tm_year -= 1900; /* year-1900 */
    tm_time.tm_mon -= 1;
    current = mktime(&tm_time); /* ���ֽ�ʱ��ת��Ϊ����ʱ�� */
    RTC_WaitForLastTask(); /* �ȴ��ϴ�д������� */
    RTC_SetCounter((u32)current); /* дRTC�����Ĵ��� */
    RTC_WaitForLastTask(); /* �ȴ��ϴ�д������� */
}

/**
  * @file   void Time_SetAlarmTime(struct tm tm_alarm)
  * @brief  ��������ʱ��(ʱ�䵽�����ж�)
  * @param  tm_alarm [in] ����ʱ��
  * @retval None
  */
void Time_SetAlarmTime(struct tm tm_alarm)
{
    time_t alarm;

    tm_alarm.tm_year -= 1900; /* year-1900 */
    tm_alarm.tm_mon -= 1; 
    alarm = mktime(&tm_alarm); /* ���ֽ�ʱ��ת��Ϊ����ʱ�� */
    RTC_WaitForLastTask(); /* �ȴ��ϴ�д������� */
    RTC_SetAlarm((u32)alarm); /* дRTC���ӼĴ��� */
    RTC_WaitForLastTask(); /* �ȴ��ϴ�д������� */
}


/**************************END OF FILE*****************************************/
