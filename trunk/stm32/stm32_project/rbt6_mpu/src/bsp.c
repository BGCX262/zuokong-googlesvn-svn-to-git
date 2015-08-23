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
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "bsp.h"

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
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
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
    //NVIC_InitTypeDef NVIC_InitStructure; 

    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  
 
    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USART1 Interrupt */
    //NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);

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

