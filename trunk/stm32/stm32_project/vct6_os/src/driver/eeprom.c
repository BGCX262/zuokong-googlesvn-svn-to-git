/*******************************************************************************
* File Name          : eeprom.c
* Description        : 24C16源文件.

* Version            : V1.0
* Date               : 2011/11/10
* Author             : zuokong
* Description        : 创建.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* 24c16 写保护端 */
#define EEPROM_WP               PB5
#define EEPROM_WP_PORT          GPIOB
#define EEPROM_WP_PIN           GPIO_Pin_5
#define EEPROM_WP_ENABLE()      EEPROM_WP = 1 // 使能写保护 
#define EEPROM_WP_DISABLE()     EEPROM_WP = 0 // 正常读写
/* I2C */
#define I2C_Speed               400000  // I2C速度400K
#define I2C1_SLAVE_ADDRESS7     0x50    // 从机地址
#define EEPROM_ADDR             0xa0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : EEPROM_Configuration
* Description    : EEPROM配置.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEPROM_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;    
  
	/* 使能与 I2C1 有关的时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    /* 24c16 wp pin -> PB5 */
    GPIO_InitStructure.GPIO_Pin = EEPROM_WP_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(EEPROM_WP_PORT, &GPIO_InitStructure);
    EEPROM_WP_DISABLE(); 
    
    /* PB6 -> I2C1_SCL  PB7 -> I2C1_SDA */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	 // 开漏输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);					

    /* I2C 配置 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
    I2C_Init(I2C1, &I2C_InitStructure);

	/* 使能 I2C1 */
    I2C_Cmd(I2C1, ENABLE);
} 

/*******************************************************************************
* Function Name  : 
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
ErrorStatus EEPROM_WriteByte(u16 addr, u8 dat)
{
    u8 eeprom_adr = 0x00;
    u32 I2C_TimeOut = 0x3FFFF;

    eeprom_adr = (u8)(addr>>7);
    eeprom_adr &= 0x0e;
    eeprom_adr |= EEPROM_ADDR;
     
    /*----- Transmission Phase -----*/
    /* Send I2C1 START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
  
    /* Test on I2C1 EV5 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    { 
        if((I2C_TimeOut--) < 1)
        {
            return ERROR;
        }
    }

    /* Send STLM75 slave address for write */
    I2C_Send7bitAddress(I2C1, eeprom_adr, I2C_Direction_Transmitter);
  
    /* Test on I2C1 EV6 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return ERROR;
        }   
    }

    /* Send the specified register data pointer */
    I2C_SendData(I2C1, (u8)addr);
  
    /* Test on I2C1 EV8 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return ERROR;
        }  
    }

    /* Send I2C1 data */
    I2C_SendData(I2C1, dat);
  
    I2C_TimeOut = 0x3FFFF;
    /* Test on I2C1 EV8 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
    {  
        if((I2C_TimeOut--) < 1)
        {
            return ERROR;
        }                                                               
    }

    /* Send I2C1 STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

    return SUCCESS;
}

/*******************************************************************************
* Function Name  : 
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 EERPROM_ReadByte(u16 addr)
{
    u8 eeprom_adr = 0x00;
    u32 I2C_TimeOut = 0x3FFFF;

    eeprom_adr = (u8)(addr>>7);
    eeprom_adr &= 0x0e;
    eeprom_adr |= EEPROM_ADDR;

    /* Enable I2C1 acknowledgement if it is already disabled by other function */
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    /*----- Transmission Phase -----*/
    /* Send I2C1 START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
  
    /* Test on I2C1 EV5 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return 0;
        }
    }
  
    /* Send STLM75 slave address for write */
    I2C_Send7bitAddress(I2C1, eeprom_adr, I2C_Direction_Transmitter);
  
    /* Test on I2C1 EV6 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return 0;
        }
    }

    /* Send the specified register data pointer */
    I2C_SendData(I2C1, (u8)addr);
  
    /* Test on I2C1 EV8 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return 0;
        }
    }

    /*----- Reception Phase -----*/
    /* Send Re-STRAT condition */
    I2C_GenerateSTART(I2C1, ENABLE);
  
    /* Test on EV5 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return 0;
        }
    }
  
    /* Send STLM75 slave address for read */
    I2C_Send7bitAddress(I2C1, eeprom_adr, I2C_Direction_Receiver);
  
    /* Test on EV6 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return 0;
        }
    }

    /* Disable I2C1 acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
  
    /* Send I2C1 STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

    /* Test on EV7 and clear it */
    I2C_TimeOut = 0x3FFFF;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
    {
        if((I2C_TimeOut--) < 1)
        {
            return 0;
        }
    }
  
    /* Store I2C1 received data */
    eeprom_adr = I2C_ReceiveData(I2C1);
  
    /* Return register value */
    return eeprom_adr;
}



/**************************END OF FILE*****************************************/

