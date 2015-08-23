/**
  ******************************************************************************
  * @file    bsp.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-17
  * @brief   初始化程序.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "myGPIOType.h"
#include "misc.h"
#include "bsp.h"
#include "rtc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/




/* Private functions ---------------------------------------------------------*/
/**
  * @file   void rcc_configuration(void)
  * @brief  stm32f10x时钟系统配置.
  * @param  None
  * @retval None
  */
void rcc_configuration(void)
{
    /* Enable GPIOB, GPIOC and AFIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | \
                           RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    /* enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

/**
  * @file   void nvic_configuration(void)
  * @brief  Configures Vector Table base location.
  * @param  None
  * @retval None
  */
void nvic_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 

    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  
 
    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#if 1
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    /* 使能EXTI线3中断 */
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);

    /* Enable the USB interrupt */
    //NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @file   void gpio_configuration(void)
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void gpio_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // PA1上拉输出 beep
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
    BEEP_OFF();

    // PB1浮空输入 -> TP_INT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @file   void beep_configuration(void)
  * @brief  蜂鸣器配置(beep->PA1[TIM2_CH2/TIM5_CH2]).
  * @param  None
  * @retval None
  */
void beep_configuration(void)
{
    /*
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能TIM2外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

    //
    TIM_TimeBaseInitStructure.TIM_Prescaler = 35999;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 100;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    // 配置定时器2输出 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    //TIM_InternalClockConfig(TIM2);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    //TIM_GenerateEvent(TIM2, TIM_EventSource_Update);   // EGR
    TIM_Cmd(TIM2, ENABLE);
    */
}




/* External functions --------------------------------------------------------*/
/**
  * @file   target_init(void)
  * @brief  初始化目标板.
  * @param  None
  * @retval None
  */
void target_init(void)
{
    rcc_configuration(); /*  stm32f10x时钟系统配置 */
    nvic_configuration(); /* NVIC configuration */
    gpio_configuration(); /* 配置端口 */
    beep_configuration(); /* 设置蜂鸣器 */
    RTC_Init();
}

/**
  * @file   void systick_configuration(void)
  * @brief  配置system tick().
  * @param  None
  * @retval None
  */
void systick_configuration(void)
{
    SysTick_Config(9000); /*  1ms 9000 */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); /* 72MHz/8 = 9MHz */
}


/**************************END OF FILE*****************************************/

