/*! \file  can.c
* \b �ӿ�˵����
*        
*
* \b ��������:
*
*       stm32f103 CAN��������
*
* \b ��ʷ��¼:
*
*       <����>          <ʱ��>       <�޸ļ�¼> <br>
*       zuokongxiao    2013-03-21    �������ļ� <br>
*/

/******************************************************************************/
/*                             ͷ�ļ�                                         */
/******************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"


/******************************************************************************/
/*                             �궨��                                         */
/******************************************************************************/

/******************************************************************************/
/*                              �ṹ����                                      */
/******************************************************************************/

/******************************************************************************/
/*                             ��������                                       */
/******************************************************************************/

/******************************************************************************/
/*                            �ڲ��ӿ�����                                    */
/******************************************************************************/




/******************************************************************************/
/*                            �ڲ��ӿ�ʵ��                                    */
/******************************************************************************/
/*! \fn       void can_gpio_configuration(void)
*
*  \brief     CAN����GPIO����
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
static void can_gpio_configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* ����CAN TX��������������� -> PB9(��ӳ��) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* ����CAN RX������������ -> PB8(��ӳ��) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /*! �˿���ӳ��PA12��PA11->PB9��PB8 */
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}

/*! \fn       void can_nvic_configuration(void)
*
*  \brief     NVIC����
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
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
/*                                �ⲿ�ӿ�                                    */
/******************************************************************************/
/*! \fn       void can_init(void)
*
*  \brief     ��ʼ��CAN1����
*
*  \param     ��
*
*  \exception ��
*
*  \return    ��
*/
void can_init(void)
{
    CAN_InitTypeDef CAN_InitStructure;
    
    /*! ʹ��CAN1ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);
    /*! ����CAN1�˿� */
    can_gpio_configuration();
    /*! ����NVIC */
    can_nvic_configuration();
    
    /*! */
}



/********************************end of file***********************************/
