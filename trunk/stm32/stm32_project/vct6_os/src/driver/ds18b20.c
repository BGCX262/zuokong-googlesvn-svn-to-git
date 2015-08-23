/**
  ******************************************************************************
  * @file    ds18b20.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2011/11/12
  * @brief   �¶ȴ�����(DS18B20->������/DS620-I2C)Դ�ļ�
  ******************************************************************************
  * @attention
  *
  * DS18B20���ö�ʱ��(1us����1��)��ȷ��ʱ,�ڶ��¶�ʱ�����ٽ����.��ʱ�����¼���
  * TIM2ʱ��Ϊ72MHz
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "ds18b20.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DS18B20_DQ_PIN      GPIO_Pin_3
#define DS18B20_DQ_PORT     GPIOE
#define DS18B20_DQ_H()      GPIO_SetBits(DS18B20_DQ_PORT, DS18B20_DQ_PIN)
#define DS18B20_DQ_L()      GPIO_ResetBits(DS18B20_DQ_PORT, DS18B20_DQ_PIN)
#define DS18B20_DQ_RD()     GPIO_ReadInputDataBit(DS18B20_DQ_PORT, DS18B20_DQ_PIN)
/* DS18B20 ROM Commands */
#define DS18B20_SEARCH_ROM          0xf0
#define DS18B20_READ_ROM            0x33
#define DS18B20_MATCH_ROM           0x55
#define DS18B20_SKIP_ROM            0xcc
#define DS18B20_ALARM_SEARCH        0xec
/* DS18B20 Function Commands */
#define DS18B20_CONVERT_T           0x44
#define DS18B20_WRITE_SCRATCHPAD    0x4E
#define DS18B20_READ_SCRATCHPAD     0xBE
#define DS18B20_COPY_SCRATCHPAD     0x48
#define DS18B20_RECALL_E2           0xB8
#define DS18B20_READ_POWER_SUPPLY   0xB4 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/
/**
  * @file   void DS18B20_Delayus(u16 delay_us)
  * @brief  ��ʱ.
  * @param  delay_us [in] ��ʱʱ��(1us~65535us)
  * @retval None
  */
void DS18B20_Delayus(u16 delay_us)
{
    u16 counter = delay_us;

    TIM_Cmd(TIM4, ENABLE);
    TIM_SetCounter(TIM4, counter);
    while(counter > 1)
    {
        counter = TIM_GetCounter(TIM4);
    }
    TIM_Cmd(TIM4, DISABLE);
}

/**
  * @file   s8 DS18B20_Reset(void)
  * @brief  ��λDS18B20����
  * @param  None
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
ErrorStatus DS18B20_Reset(void)
{
    DS18B20_DQ_H();
    DS18B20_Delayus(10);
    DS18B20_DQ_L();
    DS18B20_Delayus(500); /* (501.3us)��λ�����屣��480~640us */
    DS18B20_DQ_H(); /* �ͷ����ߺ�60~75us��Ӧ�� */
    DS18B20_Delayus(68); /* 69.4us */
    if(DS18B20_DQ_RD() == Bit_RESET)
    {
        DS18B20_Delayus(409); /* (410.15us)��ʱ410us�ȴ����� */
        return SUCCESS; /* ��λ�ɹ� */
    }
    else
    {
        return ERROR; /* ������Ӧ�� */
    }       
}

/**
  * @file   void DS18B20_WriteByte(u8 dat)
  * @brief  ������дһ���ֽ�����
  * @param  dat [in] д�������
  * @retval None
  */
void DS18B20_WriteByte(u8 dat)
{
    u8 i = 0, j = 0;
    
    for(i=0; i<8; i++)
    {
        if((dat & 0x01) == 0x01) /* write 1 */
        {
            DS18B20_DQ_H();
            for(j=0; j<10; j++);
            DS18B20_DQ_L(); /* �������߱���6us */
            DS18B20_Delayus(5);
            DS18B20_DQ_H(); /* �ͷ�������ʱ65us */
            DS18B20_Delayus(64); /* 65.1us */
        }
        else /* write 0 */
        {
            DS18B20_DQ_H();
            for(j=0; j<10; j++);
            DS18B20_DQ_L(); /* �������߱���65us */
            DS18B20_Delayus(64); /* 65.1us */
            DS18B20_DQ_H(); /* �ͷ�������ʱ10us */
            DS18B20_Delayus(9);
        }
        dat >>= 1;
    } /* end for(i=0; i<8; i++)... */
}

/**
  * @file   u8 DS18B20_ReadByte(void)
  * @brief  ��ȡһ���ֽ�����.
  * @param  None
  * @retval ���ض�ȡ��������
  */
u8 DS18B20_ReadByte(void)
{
    u8 i = 0, j = 0, dat = 0;

    for(i=0; i<8; i++)
    {
        dat >>= 1;
        DS18B20_DQ_H();
        for(j=0; j<10; j++);
        DS18B20_DQ_L(); /* �������߱���6us */
        DS18B20_Delayus(5);
        DS18B20_DQ_H(); /* �ͷ�������ʱ9us */
        DS18B20_Delayus(8);
        if(DS18B20_DQ_RD() == Bit_RESET)
        {
            dat &= 0x7f;
        }
        else
        {
            dat |= 0x80;
        }
        DS18B20_Delayus(54); /* ��ȡ����״̬��ʱ55us */
    }
    return dat;
}



/* External functions --------------------------------------------------------*/
/**
  * @file   void DS18B20_Configuration(void)
  * @brief  ����DS18B20���ţ���ʼ����ʹ�õĶ�ʱ��
  * @param  None
  * @retval None
  */
void DS18B20_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    /* DS18B20 DQ pin -> PE3 ��©��� */
    GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(DS18B20_DQ_PORT, &GPIO_InitStructure);
    DS18B20_DQ_H();
    /* ʹ��TIM4����ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInitStructure.TIM_Period = 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
    TIM_Cmd(TIM4, DISABLE);
}

/**
  * @file   ErrorStatus DS18B20_WriteScratchpad(u8 temph, u8 templ, u8 bits)
  * @brief  д���ݵ��ݴ���(���ô˺���������ʱ����800ms)
  * @param  temph [in] 0x65
  * @param  templ [in] 0x06
  * @param  bits  [in] ת������(λ��9/10/11/12)
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
ErrorStatus DS18B20_WriteScratchpad(u8 temph, u8 templ, u8 bits)
{
    if(DS18B20_Reset() == ERROR)
    {
        return ERROR;
    }
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_WRITE_SCRATCHPAD);
    DS18B20_WriteByte(temph);
    DS18B20_WriteByte(templ);
    DS18B20_WriteByte(bits);
    return SUCCESS;
}

/**
  * @file   ErrorStatus DS18B20_CopyScratchpad(void)
  * @brief  �������ݵ��ݴ���(���ô˺���������ʱ����800ms)
  * @param  None
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
ErrorStatus DS18B20_CopyScratchpad(void)
{
    if(DS18B20_Reset() == ERROR)
    {
        return ERROR;
    }
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_COPY_SCRATCHPAD);
    return SUCCESS;
}

/**
  * @file   ErrorStatus DS18B20_StartTempConvert(void)
  * @brief  ����1���¶�ת��(���ô˺���������ʱ����800ms)
  * @param  None
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
ErrorStatus DS18B20_StartTempConvert(void)
{
    if(DS18B20_Reset() == ERROR)
    {
        return ERROR; /* ���߲����� */
    }
    DS18B20_WriteByte(DS18B20_SKIP_ROM); /* ����ROM */
    DS18B20_WriteByte(DS18B20_CONVERT_T); /* �¶�ת�� */
    return SUCCESS;
}

/**
  * @file   ErrorStatus DS18B20_ReadTemperature(float *ptemp)
  * @brief  ���¶�
  * @param  ptemp [out] ���ת������¶�ֵ
  * @retval ERROR��ʧ�� SUCCESS���ɹ�
  */
ErrorStatus DS18B20_ReadTemperature(float *ptemp)
{
    u8 byte_l = 0, byte_h = 0;
    s16 temp_buf = 0;
    float temp = 0.0;

    if(DS18B20_Reset() == ERROR)
    {
        return ERROR; /* ���߲����� */
    }
    DS18B20_WriteByte(DS18B20_SKIP_ROM); /* ����ROM */
    DS18B20_WriteByte(DS18B20_READ_SCRATCHPAD); /* ��ȡ�¶� */
    byte_l = DS18B20_ReadByte(); 
    byte_h = DS18B20_ReadByte();
    temp_buf = byte_h;
    temp_buf <<= 8;
    temp_buf &= 0xff00;
    temp_buf |= byte_l;
    temp = temp_buf;
    *ptemp = temp * 0.0625;

    return SUCCESS;
}

/**************************END OF FILE*****************************************/
