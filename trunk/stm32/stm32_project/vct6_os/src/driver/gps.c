/*******************************************************************************
* File Name          : gps.c
* Description        : GPS(C3-370C)源文件.

* Version            : V1.0
* Date               : 2011/11/06
* Author             : zuokong
* Description        : 创建.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "gps.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART2_DR_Adr       ((u32)(USART2_BASE + 0x04)) // 串口2数据地址
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char gps_buf[GPS_BUFFER_SIZE]; // gps数据缓冲区
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : GPS_Configuration
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPS_Configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;   
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;					
  
    /* GPS使能端 */
    GPIO_InitStructure.GPIO_Pin = GPS_EN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPS_EN_PORT, &GPIO_InitStructure);
    GPS_DISABLE();

    /* 使能串口2时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);

    /* 配置UART2 TX引脚上拉复用输出 -> PA2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置UART2 RX引脚浮空输入 -> PA3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 4800 */
    USART_InitStructure.USART_BaudRate = 4800;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStructure);

    //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 串口接收中断

    /* 使能串口2 DMA */
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);

    /* DMA */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);   // 使能DMA1时钟

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_DR_Adr;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)gps_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = GPS_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);   // 使能DMA1传输完成中断
    DMA_Cmd(DMA1_Channel6, ENABLE);

    /* DMA1_Channel6 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    /* 使能GPS */
    GPS_ENABLE();
} 

/*******************************************************************************
* Function Name  : GPS_Configuration
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
char GPS_GetRMC(GPS_INFO *rmc_info, char *ram_buf) 
{
    int i = 0;
    char *wellhandled_string;

    /* find "$GPRMC" from gps_buf */ 
    if((wellhandled_string = strstr(ram_buf, "$GPRMC")) != NULL) 
    { 
        for (i=0; i<strlen(wellhandled_string); i++) 
        { 
            if (wellhandled_string[i] == '\n') 
            { 
                wellhandled_string[i] = '\0'; //replace ‘\n’ with null 
            }
        }
        
        /* */
        sscanf(wellhandled_string, "$GPRMC,%[^,],%c,%f,%c,%f,%c,%f,%f,%[^,]",
               rmc_info->utc_time, &(rmc_info->status), &(rmc_info->latitude_value),
               &(rmc_info->latitude), &(rmc_info->longtitude_value),&(rmc_info->longtitude),
               &(rmc_info->speed), &(rmc_info->azimuth_angle), rmc_info->utc_data); 
    } 
    else
    {
        return 1;
    }

    return 0;
}


/*******************************************************************************
* Function Name  : GPS_Configuration
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


/**************************END OF FILE*****************************************/

