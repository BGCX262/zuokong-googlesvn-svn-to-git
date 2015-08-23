/*! \file  i2c.c
* \b 接口说明：
*
*       1. i2c_init: 初始化I2C <br>
*
*       2. i2c_write: 向I2C写数据 <br>
*
*       3. i2c_read: 从I2C读取数据 <br>
*
* \b 功能描述:
*
*       I2C驱动
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-11-03   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_dma.h"
#include "myGPIOType.h"
#include "deftype.h"
#include "i2c.h"

/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
/*! I2C DMA */
#define I2C_DMA_EN              (0)

/*! I2C数据寄存器地址 */
#define I2C1_DR_Address         ((u32)0x40005410)
#define I2C2_DR_Address         ((u32)0x40005810)
/*! I2C方向 */
#define I2C_DIR_WR              (0)
#define I2C_DIR_RD              (1)
/*! I2C状态 */
#define I2C_STATE_IDLE          (0x00)
#define I2C_STATE_BUSY          (0x01)
/*! SR1 */
#define SR1_SMBALERT            (0x8000) /*!< SMBus提醒 */
#define SR1_TIMEOUT             (0x4000) /*!< 超时或Tlow错误 */
#define SR1_PECERR              (0x1000) /*!< 在接收时发生PEC错误 */
#define SR1_OVR                 (0x0800) /*!< 过载/欠载 */
#define SR1_AF                  (0x0400) /*!< 应答失败 */
#define SR1_ARLO                (0x0200) /*!< 仲裁丢失（主模式） */
#define SR1_BERR                (0x0100) /*!< 总线出错 */
#define SR1_TXE                 (0x0080) /*!< 数据寄存器为空（发送时） */
#define SR1_RXNE                (0x0040) /*!< 数据寄存器非空（接收时） */
#define SR1_STOPF               (0x0010) /*!< 停止条件检测位（从模式） */
#define SR1_ADD10               (0x0008) /*!< 10位头序列已发送（主模式） */
#define SR1_BTF                 (0x0004) /*!< 字节发送结束 */
#define SR1_ADDR                (0x0002) /*!< 地址已被发送(主模式)/地址匹配(从模式) */
#define SR1_SB                  (0x0001) /*!< 起始位已发送(主模式) */
/*! SR2 */
#define SR2_PEC                 (0xFF00) /*!< 数据包出错值 */
#define SR2_DUALF               (0x0080) /*!< 双标志(从模式) */
#define SR2_SMBHOST             (0x0040) /*!< SMBus主机头系列(从模式) */
#define SR2_SMBDEFAULT          (0x0020) /*!< SMBus设备默认地址(从模式) */
#define SR2_GENCALL             (0x0010) /*!< 广播呼叫地址(从模式) */
#define SR2_TRA                 (0x0004) /*!< 0-接收到数据 1-数据已发送 */
#define SR2_BUSY                (0x0002) /*!< 总线忙 */
#define SR2_MSL                 (0x0001) /*!< 0-从模式 1-主模式 */

/******************************************************************************/
/*                            变量定义                                        */
/******************************************************************************/
/*! DMA */
#if I2C_DMA_EN > 0
static DMA_InitTypeDef DMA_InitStructure = {0};
#endif
/*! I2C状态 */
static volatile uint8_t s_ucI2c1State = I2C_STATE_IDLE;
/*! I2C传输方向 */
static volatile uint8_t s_ucI2c1Dir = I2C_DIR_WR;
/*! I2C读写索引 */
static volatile uint16_t s_unI2c1WrIndex = 0, s_unI2c1RdIndex = 0;
/*! I2C读写缓冲区 */
static volatile i2c_buf_t s_stI2c1WrBuf = {0}, s_stI2c1RdBuf = {0};


/******************************************************************************/
/*                          内部接口声明                                      */
/******************************************************************************/


/******************************************************************************/
/*                          内部接口实现                                      */
/******************************************************************************/
/*! \fn       static void i2c_gpio_configuration(I2C_TypeDef *I2Cx)
*
*  \brief     I2C GPIO配置
*
*  \param     I2Cx [in] I2C外设
*
*  \exception 无
*
*  \return    无
*/
static void i2c_gpio_configuration(I2C_TypeDef *I2Cx)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    uint16_t unScl = 0, unSda = 0;
    
    if(I2C1 == I2Cx)
    {
        unScl = GPIO_Pin_6; /*!< I2C1_SCL->PB6 I2C1_SDA->PB7 */
        unSda = GPIO_Pin_7;
    }
    else
    {
        unScl = GPIO_Pin_10; /*!< I2C2_SCL->PB10 I2C2_SDA->PB11 */
        unSda = GPIO_Pin_11;
    }
    /*! SCL/SDA开漏复用输出 */
    GPIO_InitStructure.GPIO_Pin = unScl|unSda;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*! \fn       static void i2c_nvic_configuration(I2C_TypeDef *I2Cx)
*
*  \brief     NVIC配置
*
*  \param     I2Cx [in] I2C外设
*
*  \exception 无
*
*  \return    无
*/
static void i2c_nvic_configuration(I2C_TypeDef *I2Cx)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    uint8_t ucEvent = 0, ucError = 0;
    
    if(I2C1 == I2Cx)
    {
        ucEvent = I2C1_EV_IRQn;
        ucError = I2C1_ER_IRQn;
    }
    else
    {
        ucEvent = I2C2_EV_IRQn;
        ucError = I2C2_ER_IRQn;
    }
    /* I2C EVENT -> 设置I2C事件中断为最高优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = ucEvent;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* I2C ERROR */
    NVIC_InitStructure.NVIC_IRQChannel = ucError;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

#if I2C_DMA_EN > 0
/*! \fn       static void i2c_dma_init(I2C_TypeDef *I2Cx)
*
*  \brief     DMA初始化
*
*  \param     I2Cx [in] I2C外设
*
*  \exception 无
*
*  \return    无
*/
static void i2c_dma_init(I2C_TypeDef *I2Cx)
{
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0; /*!< @todo */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /*!< @todo */
    DMA_InitStructure.DMA_BufferSize = 0xFFFF; /*!< @todo */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    if(I2C1 == I2Cx)
    {   /* I2C1 TX DMA Channel configuration */
        DMA_DeInit(DMA1_Channel6);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
        DMA_Init(DMA1_Channel6, &DMA_InitStructure);
        /* I2C1 RX DMA Channel configuration */
        DMA_DeInit(DMA1_Channel7);
        DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    }
    else /* I2Cx = I2C2 */
    {
        /* I2C2 TX DMA Channel configuration */
        DMA_DeInit(DMA1_Channel4);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C2_DR_Address;
        DMA_Init(DMA1_Channel4, &DMA_InitStructure);
        /* I2C2 RX DMA Channel configuration */
        DMA_DeInit(DMA1_Channel5);
        DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    }
}

/*! \fn       static void i2c_dma_config(I2C_TypeDef *I2Cx)
*
*  \brief     DMA配置
*
*  \param     I2Cx [in] I2C外设
*
*  \exception 无
*
*  \return    无
*/
static void i2c_dma_config(I2C_TypeDef *I2Cx, uint8 *pucBuf, uint32 ulSize, uint8 ucDir)
{
    if(I2C_DIR_WR == ucDir) /*!< I2C发送数据 */
    {
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pucBuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = ulSize;
        if(I2Cx == I2C1)
        {
            DMA_InitStructure.DMA_PeripheralBaseAddr = I2C1_DR_Address;
            DMA_Cmd(DMA1_Channel6, DISABLE);
            DMA_Init(DMA1_Channel6, &DMA_InitStructure);
            DMA_Cmd(DMA1_Channel6, ENABLE);
        }
        else
        {
            DMA_InitStructure.DMA_PeripheralBaseAddr = I2C2_DR_Address;
            DMA_Cmd(DMA1_Channel4, DISABLE);
            DMA_Init(DMA1_Channel4, &DMA_InitStructure);
            DMA_Cmd(DMA1_Channel4, ENABLE);
        }
    }
    else /*!< I2C接收数据 */
    {
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pucBuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_BufferSize = ulSize;
        if(I2Cx == I2C1)
        {
            DMA_InitStructure.DMA_PeripheralBaseAddr = I2C1_DR_Address;
            DMA_Cmd(DMA1_Channel7, DISABLE);
            DMA_Init(DMA1_Channel7, &DMA_InitStructure);
            DMA_Cmd(DMA1_Channel7, ENABLE);
        }
        else
        {
            DMA_InitStructure.DMA_PeripheralBaseAddr = I2C2_DR_Address;
            DMA_Cmd(DMA1_Channel5, DISABLE);
            DMA_Init(DMA1_Channel5, &DMA_InitStructure);
            DMA_Cmd(DMA1_Channel5, ENABLE);
        }
    }
}
#endif

/******************************************************************************/
/*                          外部接口实现                                      */
/******************************************************************************/
/*! \fn       void i2c_init(I2C_TypeDef *I2Cx)
*
*  \brief     初始化I2C
*
*  \param     I2Cx [in] I2C外设
*
*  \exception 无
*
*  \return    无
*/
void i2c_init(I2C_TypeDef *I2Cx)
{
    I2C_InitTypeDef I2C_InitStructure = {0};
    
    /* 使能I2c时钟 */
    if(I2C1 == I2Cx)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 , ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2 , ENABLE);
    }
    /*! I2C GPIO配置 */
    i2c_gpio_configuration(I2Cx);
    /*! NVIC配置 */
    i2c_nvic_configuration(I2Cx);
    /*! DMA初始化 */
#if I2C_DMA_EN > 0
    i2c_dma_init(I2Cx);
#endif
    /*! I2C配置 */
    I2C_InitStructure.I2C_ClockSpeed = 400000; /*!< 400KHz */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2Cx, &I2C_InitStructure);
    
    /*! 设置I2C中断 */
    //I2C_ITConfig(I2Cx, I2C_IT_BUF|I2C_IT_EVT|I2C_IT_ERR, ENABLE);
    //I2C_Cmd(I2Cx, ENABLE);
}

/*! \fn       int32_t i2c_write(I2C_TypeDef *I2Cx, uint8_t ucAddr, \
*                               uint8_t *pucBuf, uint16_t unSize)
*
*  \brief     向I2C写数据
*
*  \param     I2Cx   [in] I2C外设
*
*  \param     ucAddr [in] 地址
*
*  \param     pucBuf [in] 数据缓冲区
*
*  \param     unSize [in] 写数据大小
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功; EXIT_FAILURE：失败
*/
int32_t i2c_write(I2C_TypeDef *I2Cx, uint8_t ucAddr, \
                  uint8_t *pucBuf, uint16_t unSize)
{
    /*! 输入参数检查 */
    if(INVALID_POINTER(I2Cx) || INVALID_POINTER(pucBuf) || (1 > unSize))
    {
        return EXIT_FAILURE;
    }
    
    if(I2C1 == I2Cx)
    {
        /*! I2C状态 */
        if(I2C_STATE_BUSY == s_ucI2c1State)
        {
            return EXIT_FAILURE;
        }
        /*! 使能I2C错误中断 */
        I2Cx->CR2 |= I2C_IT_ERR;
        /*! 使能I2C事件中断 */
        I2Cx->CR2 |= I2C_IT_EVT;
        /*! 使能I2C缓冲区中断 */
        I2Cx->CR2 |= I2C_IT_BUF;
        /*! 设置I2C缓冲区 */
        s_ucI2c1Dir = I2C_DIR_WR;
        s_stI2c1WrBuf.ucSalveAddr = ucAddr & ((uint16_t)0xFFFE);
        s_stI2c1WrBuf.unBufSize = unSize;
        s_stI2c1WrBuf.pucBuf = pucBuf;
        /*! 发送起始位 */
        I2Cx->CR1 |= ((uint16_t)0x0100);
        /*! 等待起始位产生 */
        while(0x100 == (I2Cx->CR1&0x100));
        /*! 等待BUSY标志清零：停止位在总线产生表示传输结束 */
        while(0x0002 == (I2Cx->SR2 &0x0002));
    }
    
    return EXIT_SUCCESS;
}

/*! \fn       int32_t i2c_read(I2C_TypeDef *I2Cx, uint8_t ucAddr, \
*                               uint8_t *pucBuf, uint16_t unSize)
*
*  \brief     从I2C读取数据
*
*  \param     I2Cx   [in] I2C外设
*
*  \param     ucAddr [in] 地址
*
*  \param     pucBuf [in] 数据缓冲区
*
*  \param     unSize [in] 写数据大小
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功; EXIT_FAILURE：失败
*/
int32_t i2c_read(I2C_TypeDef *I2Cx, uint8_t ucAddr, \
                  uint8_t *pucBuf, uint16_t unSize)
{
    /*! 输入参数检查 */
    if(INVALID_POINTER(I2Cx) || INVALID_POINTER(pucBuf) || (1 > unSize))
    {
        return EXIT_FAILURE;
    }
    
    if(I2C1 == I2Cx)
    {
        /*! I2C状态 */
        if(I2C_STATE_BUSY == s_ucI2c1State)
        {
            return EXIT_FAILURE;
        }
        /*! 使能I2C错误中断 */
        I2Cx->CR2 |= I2C_IT_ERR;
        /*! 使能I2C事件中断 */
        I2Cx->CR2 |= I2C_IT_EVT;
        /*! 使能I2C缓冲区中断 */
        I2Cx->CR2 |= I2C_IT_BUF;
        /*! 设置I2C缓冲区 */
        s_ucI2c1Dir = I2C_DIR_RD;
        s_stI2c1RdBuf.ucSalveAddr = ucAddr | ((uint16_t)0x0001);
        s_stI2c1RdBuf.unBufSize = unSize;
        s_stI2c1RdBuf.pucBuf = pucBuf;
        /*! 发送起始位 */
        I2Cx->CR1 |= ((uint16_t)0x0100);
        /*! 等待起始位产生 */
        while(0x100 == (I2Cx->CR1&0x100));
        /*! 等待BUSY标志清零：停止位在总线产生表示传输结束 */
        while(0x0002 == (I2Cx->SR2 &0x0002));
        /*! Enable Acknowledgement to be ready for another reception */
        I2Cx->CR1 |= ((uint16_t)0x0400);
    }
    
    return EXIT_SUCCESS;
}

/******************************************************************************/
/*                       I2C1中断处理函数                                     */
/******************************************************************************/
/*! \fn       void I2C1_EV_IRQHandler(void)
*
*  \brief     I2C1事件中断
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void I2C1_EV_IRQHandler(void)
{
    volatile uint32_t ulSR1Reg = 0;
    volatile uint32_t ulSR2Reg = 0;
    
    /*! 读取状态寄存器 */
    ulSR1Reg = I2C1->SR1;
    ulSR2Reg = I2C1->SR2;
    /*! 从模式(MSL flag = 0) */
    if((ulSR2Reg&SR2_MSL) != SR2_MSL)
    {
        if(SR1_ADDR == (ulSR1Reg & SR1_ADDR)) /*!< ADDR = 1: EV1 */
        {
            ulSR1Reg = 0;
            ulSR2Reg = 0;
            s_unI2c1WrIndex = 0;
            s_unI2c1RdIndex = 0;
        }
        if(SR1_TXE == (ulSR1Reg & SR1_TXE)) /*!< TXE = 1: EV3 */
        {
            //I2C1->DR = s_aucWrBuf1[s_ucWrIndex1++]; /*!< 写数据到数据寄存器 */
            ulSR1Reg = 0;
            ulSR2Reg = 0;
        }
        if(SR1_RXNE == (ulSR1Reg & SR1_RXNE)) /*!< If RXNE = 1: EV2 */
        {
            //s_aucRdBuf1[s_ucRdIndex1++] = I2C1->DR; /*< 从数据寄存器中读取数据 */
            ulSR1Reg = 0;
            ulSR2Reg = 0;
        }
        /*! If STOPF =1: EV4 停止条件检测位（从模式） */
        if(SR1_STOPF == (ulSR1Reg & SR1_STOPF))
        {
            I2C1->CR1 |= ((uint16_t)0x0001);
            ulSR1Reg = 0;
            ulSR2Reg = 0;
        }
    } /*!< 从模式 */

    /*! 起始位已发送(主模式): EV5 */
    if(SR1_SB == (ulSR1Reg & SR1_SB))
    {
        if(I2C_DIR_WR == s_ucI2c1Dir) /*!< 写 */
        {
            I2C1->DR = s_stI2c1WrBuf.ucSalveAddr;
        }
        else /*!< 读 */
        {
            I2C1->DR = s_stI2c1RdBuf.ucSalveAddr;
        }
        ulSR1Reg = 0;
        ulSR2Reg = 0;
    }
    
    /*! I2C主模式 */
    if(SR2_MSL == (ulSR2Reg&SR2_MSL))
    {
        /* 地址已被发送(主模式), EV6 */
        if(SR1_ADDR == (ulSR1Reg&SR1_ADDR))
        {
            if(I2C_DIR_WR == s_ucI2c1Dir) /*!< 发送模式时写第一个数据 */
            {
                s_unI2c1WrIndex = 0;
                I2C1->DR = s_stI2c1WrBuf.pucBuf[s_unI2c1WrIndex];
                (s_stI2c1WrBuf.unBufSize)--;
                /*! 如果没有其他数据，禁止中断（I2C BUF IT） */
                if(0 == s_stI2c1WrBuf.unBufSize)
                {
                    I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
                }
            }
            else /*!< 主模式读数据 */
            {
                s_unI2c1RdIndex = 0;
                if(1 == s_stI2c1RdBuf.unBufSize) /*!< I2C读取一个字节数据，数据还未接收完成 */
                {
                    I2C1->CR1 &= ((uint16_t)0xFBFF); /*!< Clear ACK */
                    I2C1->CR1 |= ((uint16_t)0x0200); /*!< Program the STOP */
                }
            }
            ulSR1Reg = 0;
            ulSR2Reg = 0;
        }
        
        /*! TXE == 1 */
        if((ulSR1Reg &0x0084) == 0x0080)
        {
            if(0 != s_stI2c1WrBuf.unBufSize)
            {
                I2C1->DR = s_stI2c1WrBuf.pucBuf[s_unI2c1WrIndex];
                (s_stI2c1WrBuf.unBufSize)--;
                /*! 如果没有其他数据，禁止中断（I2C BUF IT） */
                if(0 == s_stI2c1WrBuf.unBufSize)
                {
                    I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
                }
            }
            ulSR1Reg = 0;
            ulSR2Reg = 0;
        }
        
        /*! BTF==1 TXE==1 (EV8_2)，产生STOP */
        if((ulSR1Reg & 0x0084) == 0x0084)
        {
            I2C1->CR1 |= ((uint16_t)0x0200); /*!< Program the STOP */
            /* 禁止事件中断，In order to not have again a BTF IT */
            I2C1->CR2 &= (uint16_t)~((uint16_t)0x0200);
            ulSR1Reg = 0;
            ulSR2Reg = 0;
        }
        
        /*! 数据寄存器非空 */
        if(SR1_RXNE == (ulSR1Reg & SR1_RXNE)) 
        {
            s_stI2c1RdBuf.pucBuf[s_unI2c1RdIndex] = I2C1->DR;
            (s_stI2c1RdBuf.unBufSize)--;
            /*! 接收最后字节 */
            if(1 == s_stI2c1RdBuf.unBufSize)
            {
                I2C1->CR1 &= ((uint16_t)0xFBFF); /*!< Clear ACK */
                I2C1->CR1 |= ((uint16_t)0x0200); /*!< Program the STOP */
            }
            ulSR1Reg = 0;
            ulSR2Reg = 0;
        }
    } /*!< end if(SR2_MSL == (ulSR2Reg&SR2_MSL))... */
}

/*! \fn       void I2C1_ER_IRQHandler(void)
*
*  \brief     I2C1错误中断
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void I2C1_ER_IRQHandler(void)
{
    volatile uint32_t ulSR1Reg = 0;

    /* Read the I2C1 status register */
    ulSR1Reg = I2C1->SR1;

    /* If AF = 1 */
    if((ulSR1Reg & 0x0400) == 0x0400)
    {
        I2C1->SR1 &= 0xFBFF;
        ulSR1Reg = 0;
    }
    /* If ARLO = 1 */
    if((ulSR1Reg & 0x0200) == 0x0200)
    {
        I2C1->SR1 &= 0xFBFF;
        ulSR1Reg = 0;
    }
    /* If BERR = 1 */
    if((ulSR1Reg & 0x0100) == 0x0100)
    {
        I2C1->SR1 &= 0xFEFF;
        ulSR1Reg = 0;
    }
    /* If OVR = 1 */
    if((ulSR1Reg & 0x0800) == 0x0800)
    {
        I2C1->SR1 &= 0xF7FF;
        ulSR1Reg = 0;
    }
}

/******************************************************************************/
/*                       I2C2中断处理函数                                     */
/******************************************************************************/
/*! \fn       void I2C2_EV_IRQHandler(void)
*
*  \brief     I2C2事件中断
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void I2C2_EV_IRQHandler(void)
{
    
}

/*! \fn       void I2C2_ER_IRQHandler(void)
*
*  \brief     I2C2错误中断
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void I2C2_ER_IRQHandler(void)
{
    
}

/********************************end of file***********************************/

