/*! \file  can.c
* \b 接口说明：
*        
*
* \b 功能描述:
*
*       stm32f103 CAN总线驱动
*
* \b 历史记录:
*
*       <作者>          <时间>       <修改记录> <br>
*       zuokongxiao    2013-03-21    创建该文件 <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"


/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/

/******************************************************************************/
/*                              结构定义                                      */
/******************************************************************************/

/******************************************************************************/
/*                             变量定义                                       */
/******************************************************************************/

/******************************************************************************/
/*                            内部接口声明                                    */
/******************************************************************************/




/******************************************************************************/
/*                            内部接口实现                                    */
/******************************************************************************/
/*! \fn       void can_gpio_configuration(void)
*
*  \brief     CAN总线GPIO配置
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
static void can_gpio_configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* 配置CAN TX引脚上拉复用输出 -> PB9(重映射) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* 配置CAN RX引脚上拉输入 -> PB8(重映射) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /*! 端口重映射PA12、PA11->PB9、PB8 */
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}

/*! \fn       void can_nvic_configuration(void)
*
*  \brief     NVIC配置
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
static void can_nvic_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the  Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************/
/*                                外部接口                                    */
/******************************************************************************/
/*! \fn       void can_init(void)
*
*  \brief     初始化CAN1总线
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void can_init(void)
{
    CAN_InitTypeDef CAN_InitStructure;
    
    /*! 使能CAN1时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);
    /*! 配置CAN1端口 */
    can_gpio_configuration();
    /*! 配置NVIC */
    can_nvic_configuration();
    
    /*! */
}



/********************************end of file***********************************/

