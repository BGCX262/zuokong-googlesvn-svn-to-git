/**
  ******************************************************************************
  * @file    ds18b20.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2011/11/12
  * @brief   温度传感器(DS18B20->单总线/DS620-I2C)源文件
  ******************************************************************************
  * @attention
  *
  * DS18B20采用定时器(1us计数1次)精确延时,在读温度时处于临界段中.定时器向下计数
  * TIM2时钟为72MHz
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
  * @brief  延时.
  * @param  delay_us [in] 延时时间(1us~65535us)
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
  * @brief  复位DS18B20总线
  * @param  None
  * @retval ERROR：失败 SUCCESS：成功
  */
ErrorStatus DS18B20_Reset(void)
{
    DS18B20_DQ_H();
    DS18B20_Delayus(10);
    DS18B20_DQ_L();
    DS18B20_Delayus(500); /* (501.3us)复位低脉冲保持480~640us */
    DS18B20_DQ_H(); /* 释放总线后60~75us读应答 */
    DS18B20_Delayus(68); /* 69.4us */
    if(DS18B20_DQ_RD() == Bit_RESET)
    {
        DS18B20_Delayus(409); /* (410.15us)延时410us等待结束 */
        return SUCCESS; /* 复位成功 */
    }
    else
    {
        return ERROR; /* 总线无应答 */
    }       
}

/**
  * @file   void DS18B20_WriteByte(u8 dat)
  * @brief  向总线写一个字节数据
  * @param  dat [in] 写入的数据
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
            DS18B20_DQ_L(); /* 拉低总线保持6us */
            DS18B20_Delayus(5);
            DS18B20_DQ_H(); /* 释放总线延时65us */
            DS18B20_Delayus(64); /* 65.1us */
        }
        else /* write 0 */
        {
            DS18B20_DQ_H();
            for(j=0; j<10; j++);
            DS18B20_DQ_L(); /* 拉低总线保持65us */
            DS18B20_Delayus(64); /* 65.1us */
            DS18B20_DQ_H(); /* 释放总线延时10us */
            DS18B20_Delayus(9);
        }
        dat >>= 1;
    } /* end for(i=0; i<8; i++)... */
}

/**
  * @file   u8 DS18B20_ReadByte(void)
  * @brief  读取一个字节数据.
  * @param  None
  * @retval 返回读取到的数据
  */
u8 DS18B20_ReadByte(void)
{
    u8 i = 0, j = 0, dat = 0;

    for(i=0; i<8; i++)
    {
        dat >>= 1;
        DS18B20_DQ_H();
        for(j=0; j<10; j++);
        DS18B20_DQ_L(); /* 拉低总线保持6us */
        DS18B20_Delayus(5);
        DS18B20_DQ_H(); /* 释放总线延时9us */
        DS18B20_Delayus(8);
        if(DS18B20_DQ_RD() == Bit_RESET)
        {
            dat &= 0x7f;
        }
        else
        {
            dat |= 0x80;
        }
        DS18B20_Delayus(54); /* 读取总线状态延时55us */
    }
    return dat;
}



/* External functions --------------------------------------------------------*/
/**
  * @file   void DS18B20_Configuration(void)
  * @brief  配置DS18B20引脚，初始化所使用的定时器
  * @param  None
  * @retval None
  */
void DS18B20_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    /* DS18B20 DQ pin -> PE3 开漏输出 */
    GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(DS18B20_DQ_PORT, &GPIO_InitStructure);
    DS18B20_DQ_H();
    /* 使能TIM4外设时钟 */
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
  * @brief  写数据到暂存器(调用此函数后需延时至少800ms)
  * @param  temph [in] 0x65
  * @param  templ [in] 0x06
  * @param  bits  [in] 转换精度(位数9/10/11/12)
  * @retval ERROR：失败 SUCCESS：成功
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
  * @brief  复制数据到暂存器(调用此函数后需延时至少800ms)
  * @param  None
  * @retval ERROR：失败 SUCCESS：成功
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
  * @brief  启动1次温度转换(调用此函数后需延时至少800ms)
  * @param  None
  * @retval ERROR：失败 SUCCESS：成功
  */
ErrorStatus DS18B20_StartTempConvert(void)
{
    if(DS18B20_Reset() == ERROR)
    {
        return ERROR; /* 总线不可用 */
    }
    DS18B20_WriteByte(DS18B20_SKIP_ROM); /* 跳过ROM */
    DS18B20_WriteByte(DS18B20_CONVERT_T); /* 温度转换 */
    return SUCCESS;
}

/**
  * @file   ErrorStatus DS18B20_ReadTemperature(float *ptemp)
  * @brief  读温度
  * @param  ptemp [out] 输出转换后的温度值
  * @retval ERROR：失败 SUCCESS：成功
  */
ErrorStatus DS18B20_ReadTemperature(float *ptemp)
{
    u8 byte_l = 0, byte_h = 0;
    s16 temp_buf = 0;
    float temp = 0.0;

    if(DS18B20_Reset() == ERROR)
    {
        return ERROR; /* 总线不可用 */
    }
    DS18B20_WriteByte(DS18B20_SKIP_ROM); /* 跳过ROM */
    DS18B20_WriteByte(DS18B20_READ_SCRATCHPAD); /* 读取温度 */
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

