/**
  ******************************************************************************
  * @file    tft.c
  * @author  zuokongxiao
  * @version V1.0
  * @date    2011/07/14
  * @brief   tft240x320(控制器为9325)驱动，使用STM32 FSMC. 地址确定是关键.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_fsmc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "myGPIOType.h"
#include "debug.h"
#include "tft.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct 
{
    vu16 TFT_REG;
    vu16 TFT_RAM;
} TFT_TypeDef;
/* Private define ------------------------------------------------------------*/
//#define TFT_REG   *(vu16 *)0x60000000 
//#define TFT_RAM   *(vu16 *)0x60020000 // HADDR[25:1] -> FSMC_A[24:0]
#define TFT_BASE    ((u32)0x6001fffe)           // 数据是16bits, 注意地址
#define TFT         ((TFT_TypeDef *)TFT_BASE)   // HADDR[25:1] -> FSMC_A[24:0]
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External constant ---------------------------------------------------------*/
extern const unsigned char acFont8x16[][16]; // 32(0x20)->空格开始
extern const unsigned char acFontD36x48[15][240];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void TFT_DelayMS(u16 ms);



/*******************************************************************************
* Function Name  : TFT_PortConfiguration
* Description    : TFT端口设置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_PortConfiguration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 使能FSMC时钟 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    /* PB0 -> BL_EN 背光使能端 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PD PD11->RS(A16) CS GPIO_Pin_7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | 
                                  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 |
                                  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                  GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    /* PE D4~D12 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                                  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* lcd NRESET -> PD12 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // 背光PWM设置; CCRx = 0输出高, CCRx(占空比) > ARR(频率)输出低
    /* 使能TIM3外设时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
    /* */
    TIM_TimeBaseInitStructure.TIM_Prescaler = 359; //35999;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 99; //9;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    /* 配置定时器3输出 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    //TIM_InternalClockConfig(TIM3);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    //TIM_GenerateEvent(TIM3, TIM_EventSource_Update);   // EGR
    TIM_Cmd(TIM3, ENABLE);
    TIM3->CCR3 = 0;
}

/*******************************************************************************
* Function Name  : TFT_FSMCConfiguration
* Description    : TFT 总线配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_FSMCConfiguration(void)
{
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef p;

    // FSMC_Bank1_NORSRAM1 configuration
    p.FSMC_AddressSetupTime = 2;
    p.FSMC_AddressHoldTime = 1;    
    p.FSMC_DataSetupTime = 5;    
    p.FSMC_BusTurnAroundDuration = 0; 
    p.FSMC_CLKDivision = 0;
    p.FSMC_DataLatency = 0; 
    p.FSMC_AccessMode = FSMC_AccessMode_B;

    // LCD configuration 
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;             
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /* BANK 1(of NOR/SRAM Bank 1~4) is enabled */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/*******************************************************************************
* Function Name  : TFT_WriteReg
* Description    : Writes to the selected TFT register.
* Input          : - TFT_Reg: address of the selected register.
*                  - TFT_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_WriteReg(u16 TFT_Reg, u16 TFT_RegValue)
{
    /* Write 16-bit Index, then Write Reg */
    TFT->TFT_REG = TFT_Reg;
    /* Write 16-bit Reg */
    TFT->TFT_RAM = TFT_RegValue;
}

/*******************************************************************************
* Function Name  : TFT_ReadReg
* Description    : Reads the selected TFT Register.
* Input          : None
* Output         : None
* Return         : TFT Register Value.
*******************************************************************************/
u16 TFT_ReadReg(u16 TFT_Reg)
{
    /* Write 16-bit Index (then Read Reg) */
    TFT->TFT_REG = TFT_Reg;
    /* Read 16-bit Reg */
    return (TFT->TFT_RAM);
}

/*******************************************************************************
* Function Name  : TFT_WriteRAM_Prepare
* Description    : Prepare to write to the TFT RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_WriteRAM_Prepare(void)
{   
    TFT->TFT_REG = 0x22; 
}

/*******************************************************************************
* Function Name  : TFT_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position(0~239).
*                  - Ypos: specifies the Y position(0~319). 
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_SetCursor(u16 Xpos, u16 Ypos)
{ 
#if TFT_MODE > 0
    TFT_WriteReg(0x20, Ypos);   // 行
    TFT_WriteReg(0x21, Xpos);   // 列
#else
    TFT_WriteReg(0x20, Xpos);   // 行
    TFT_WriteReg(0x21, Ypos);   // 列
#endif
}

/*******************************************************************************
* Function Name  : TFT_Clear
* Description    : Clears the hole TFT.
* Input          : Color: the color of the background.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_Clear(u16 Color)
{
    u16 index = 0;
  
    TFT_SetCursor(0x00, 0x00);      // 0x13f
    TFT_WriteRAM_Prepare();        /* Prepare to write GRAM */
    for(index = 0; index < 7680; index++)
    {
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
        TFT->TFT_RAM = Color;
    }
}

/*******************************************************************************
* Function Name  : TFT_Init
* Description    : 初始化TFT.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_Init(void)
{
#if TFT_DEBUG_EN > 0
    u16 tft_type = 0;
#endif

    TFT_PortConfiguration();            /* 初始化端口及背光PWM */
    TFT_FSMCConfiguration();            /* 配置FSMC */
    TFT_PIN_NRESET = 0;                 /* 复位TFT 拉底lcd nreset */
    TFT_DelayMS(10);                    /* delay 10 ms */
    TFT_PIN_NRESET = 1;                 /* 拉高lcd nreset */
    TFT_DelayMS(50);                    /* delay 50 ms */
#if TFT_DEBUG_EN > 0
    tft_type = TFT_ReadReg(0x00);       /* read device code */
    tft_type = tft_type;
    //DEBUG_MSG("D: TFT Type = %x\r\n", tft_type);
#endif

    /* Start Initial Sequence ------------------------------------------------*/
    TFT_WriteReg(0xe5, 0x8000); /* Set the internal vcore voltage */
    TFT_WriteReg(0x00, 0x0001); /* Start internal OSC. */
#if TFT_MODE > 0
    TFT_WriteReg(0x01, 0x0100); /* set SS and SM bit */
#else
    TFT_WriteReg(0x01, 0x0100); /* set SS and SM bit */
#endif
    TFT_WriteReg(0x02, 0x0700); /* set 1 line inversion */
    TFT_WriteReg(0x03, 0x1030); /* set GRAM write direction and BGR=1. */
    TFT_WriteReg(0x04, 0x0000); /* Resize register */
    TFT_WriteReg(0x08, 0x0202); /* set the back porch and front porch */
    TFT_WriteReg(0x09, 0x0000); /* set non-display area refresh cycle ISC[3:0] */
    TFT_WriteReg(0x0a, 0x0000); /* FMARK function */
    TFT_WriteReg(0x0c, 0x0000); /* RGB interface setting */
    TFT_WriteReg(0x0d, 0x0000); /* Frame marker Position */
    TFT_WriteReg(0x0f, 0x0000); /* RGB interface polarity */

    /* Power On sequence -----------------------------------------------------*/
    TFT_WriteReg(0x10, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    TFT_WriteReg(0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    TFT_WriteReg(0x12, 0x0000); /* VREG1OUT voltage */
    TFT_WriteReg(0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */
    TFT_DelayMS(50);            /* Dis-charge capacitor power voltage (200ms) */
    TFT_WriteReg(0x10, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    TFT_WriteReg(0x11, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    TFT_DelayMS(10);            /* Delay 50 ms */
    TFT_WriteReg(0x12, 0x0139); /* VREG1OUT voltage */
    TFT_DelayMS(10);            /* Delay 50 ms */
    TFT_WriteReg(0x13, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    TFT_WriteReg(0x29, 0x0013); /* VCM[4:0] for VCOMH */
    TFT_DelayMS(10);            /* Delay 50 ms */
    TFT_WriteReg(0x20, 0x0000); /* GRAM horizontal Address */
    TFT_WriteReg(0x21, 0x0000); /* GRAM Vertical Address */

    /* Adjust the Gamma Curve ------------------------------------------------*/
    TFT_WriteReg(0x30, 0x0006);
    TFT_WriteReg(0x31, 0x0101);
    TFT_WriteReg(0x32, 0x0003);
    TFT_WriteReg(0x35, 0x0106);
    TFT_WriteReg(0x36, 0x0b02);
    TFT_WriteReg(0x37, 0x0302);
    TFT_WriteReg(0x38, 0x0707);
    TFT_WriteReg(0x39, 0x0007);
    TFT_WriteReg(0x3c, 0x0600);
    TFT_WriteReg(0x3d, 0x020b);

    /* Set GRAM area ---------------------------------------------------------*/
    TFT_WriteReg(0x50, 0x0000); /* Horizontal GRAM Start Address */
    TFT_WriteReg(0x51, 0x00EF); /* Horizontal GRAM End Address */
    TFT_WriteReg(0x52, 0x0000); /* Vertical GRAM Start Address */
    TFT_WriteReg(0x53, 0x013F); /* Vertical GRAM End Address */

#if TFT_MODE > 0
    TFT_WriteReg(0x60, 0x2700);
#else
    TFT_WriteReg(0x60, 0xa700); /* 0x2700 Gate Scan Line */
#endif
    TFT_WriteReg(0x61, 0x0001); /* NDL,VLE, REV */
    TFT_WriteReg(0x6a, 0x0000); /* set scrolling line */

    /* Partial Display Control -----------------------------------------------*/
    TFT_WriteReg(0x80, 0x0000);
    TFT_WriteReg(0x81, 0x0000);
    TFT_WriteReg(0x82, 0x0000);
    TFT_WriteReg(0x83, 0x0000);
    TFT_WriteReg(0x84, 0x0000);
    TFT_WriteReg(0x85, 0x0000);

    /* Panel Control ---------------------------------------------------------*/
    TFT_WriteReg(0x90, 0x0010);
    TFT_WriteReg(0x92, 0x0000);
    TFT_WriteReg(0x93, 0x0003);
    TFT_WriteReg(0x95, 0x0110);
    TFT_WriteReg(0x97, 0x0000);
    TFT_WriteReg(0x98, 0x0000);

    /* Set GRAM write direction and BGR = 1 */
    /* I/D=01 (Horizontal : increment, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
#if TFT_MODE > 0
    TFT_WriteReg(0x03, 0x1018);
#else
    TFT_WriteReg(0x03, 0x1030);  
#endif
    TFT_WriteReg(0x07, 0x0173); /* 262K color and display ON */ 
#if LCD_DEBUG_EN > 0
    TFT_Clear(White);
#else
    TFT_Clear(White);
#endif
    //TFT_DelayMS(10);
}

/*******************************************************************************
* Function Name  : TFT_SetBlackLight
* Description    : 设置背光.
* Input          : - TFT_BL_Mode: 背光模式.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_SetBlackLight(TFTBLMode_TypeDef TFT_BL_Mode)
{
    // 背光PWM设置; CCRx = 0输出高(100%), CCRx(占空比) > 9(频率)输出低
    TIM3->CCR3 = (u8)TFT_BL_Mode;
}

/*******************************************************************************
* Function Name  : TFT_DelayMS
* Description    : 延时
* Input          : ms: delay value.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_DelayMS(u16 ms)
{
    u16 i = 0;

    while(ms--)
    {
        for(i=20000; i>0; i++);
    }
}

/*******************************************************************************
* Function Name  : TFT_SetPixel
* Description    : 设置tft像素
* Input          : - Xpos: 横坐标.
*                  - yPos: 纵坐标.
*                  - Color: pixel color.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_SetPixel(u16 Xpos, u16 Ypos, u16 Color)
{
    /* 参数检查 */
    if((Xpos > TFT_MAX_XSIZE) || (Ypos > TFT_MAX_YSIZE))
    {
        return;
    }

    TFT_SetCursor(Xpos, Ypos);
    TFT_WriteReg(0x22, Color);   
}

/*******************************************************************************
* Function Name  : TFT_GetPixel
* Description    : 获取tft像素
* Input          : - Xpos: 横坐标.
*                  - yPos: 纵坐标.
* Output         : None
* Return         : pixel color.
*******************************************************************************/
u16 TFT_GetPixel(u16 Xpos, u16 Ypos)
{
    u16 Color = 0;    
    /* 参数检查 */
    if((Xpos > TFT_MAX_XSIZE) || (Ypos > TFT_MAX_YSIZE))
    {
        return 0;
    }
    TFT_SetCursor(Xpos, Ypos);
    Color = TFT_ReadReg(0x22);
    
    return Color;   
}

/*******************************************************************************
* Function Name  : TFT_DrawHLine
* Description    : 画水平线(假设在水平扫描方式下).
* Input          : - Xpos0: 起始点横坐标. 
*                  - Ypos : 起始点纵坐标. 
*                  - Xpos1: 终点横坐标.
*                  - Color: line color.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_DrawHLine(u16 Xpos0, u16 Ypos, u16 Xpos1, u16 Color)
{
    u16 i = 0;
    
    if(Xpos0 > Xpos1)
    {
        i = Xpos1;
        Xpos1 = Xpos0;
        Xpos0 = i;
    }
    /* 参数检查 */
    if((Xpos1 > TFT_MAX_XSIZE)|| (Ypos > TFT_MAX_YSIZE))
    {
        return;
    }
    
    TFT_WriteReg(0x50, Xpos0);  // Horizontal GRAM Start Address
    TFT_WriteReg(0x51, Xpos1);  // Horizontal GRAM End Address
    TFT_WriteReg(0x52, Ypos);   // Vertical GRAM Start Address
    TFT_WriteReg(0x53, Ypos);   // Vertical GRAM End Address 
    TFT_SetCursor(Xpos0, Ypos); // 设置起始坐标
    TFT_WriteRAM_Prepare();     // Prepare to write GRAM
    for(i=Xpos0; i<=Xpos1; i++)
    {
        TFT->TFT_RAM = Color;
    }
}

/*******************************************************************************
* Function Name  : TFT_DrawVLine
* Description    : 画垂直线(假设在水平扫描方式下).
* Input          : - Xpos : 起始点横坐标. 
*                  - Ypos0: 起始点纵坐标. 
*                  - Ypos1: 终点纵坐标.
*                  - Color: line color.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_DrawVLine(u16 Xpos, u16 Ypos0, u16 Ypos1, u16 Color)
{
    u16 i = 0;
    
    if(Ypos0 > Ypos1)
    {
        i = Ypos1;
        Ypos1 = Ypos0;
        Ypos0 = i;
    }
    /* 参数检查 */
    if((Ypos1 > TFT_MAX_YSIZE) || (Xpos > TFT_MAX_XSIZE))
    {
        return;
    }

    TFT_WriteReg(0x50, Xpos);   // Horizontal GRAM Start Address
    TFT_WriteReg(0x51, Xpos);   // Horizontal GRAM End Address
    TFT_WriteReg(0x52, Ypos0);  // Vertical GRAM Start Address
    TFT_WriteReg(0x53, Ypos1);  // Vertical GRAM End Address 
    TFT_SetCursor(Xpos, Ypos0); // 设置起始坐标
    TFT_WriteRAM_Prepare();     // Prepare to write GRAM
    for(i=Ypos0; i<=Ypos1; i++)
    {
        TFT->TFT_RAM = Color;
    }
}

/*******************************************************************************
* Function Name  : TFT_DrawRect
* Description    : 画矩形区域(假设在水平扫描方式下).
* Input          : - Xpos0: 起始点横坐标. 
*                  - Ypos0: 起始点纵坐标.
*                  - Xpos1: 终点横坐标.
*                  - Ypos1: 终点纵坐标.
*                  - Color: color.
*                  - Fill : 1填充, 0不填充. 
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_FillRect(u16 Xpos0, u16 Ypos0, u16 Xpos1, u16 Ypos1, u16 Color, u8 Fill)
{
    u16 i = 0, j = 0;

    j = j;
    if(Xpos0 > Xpos1)
    {
        i = Xpos1;
        Xpos1 = Xpos0;
        Xpos0 = i;
    }
    if(Ypos0 > Ypos1)
    {
        i = Ypos1;
        Ypos1 = Ypos0;
        Ypos0 = i;
    }
    /* 参数检查 */
    if((Xpos1 > TFT_MAX_XSIZE)|| (Ypos1 > TFT_MAX_YSIZE))
    {
        return;
    }

    if(Fill == 1)
    {
        TFT_WriteReg(0x50, Xpos0);  // Horizontal GRAM Start Address
        TFT_WriteReg(0x51, Xpos1);  // Horizontal GRAM End Address
        TFT_WriteReg(0x52, Ypos0);  // Vertical GRAM Start Address
        TFT_WriteReg(0x53, Ypos1);  // Vertical GRAM End Address 
        TFT_SetCursor(Xpos0, Ypos0);// 设置起始坐标
        TFT_WriteRAM_Prepare();     // Prepare to write GRAM 

        for(j=Ypos0; j<=Ypos1; j++)
        {
            for(i=Xpos0; i<=Xpos1; i++)
            {
                TFT->TFT_RAM = Color;
            }
        }
    }
    else
    {
        if(Fill == 0)
        {
            TFT_DrawHLine(Xpos0, Ypos0, Xpos1, Color);
            TFT_DrawHLine(Xpos0, Ypos1, Xpos1, Color);
            TFT_DrawVLine(Xpos0, Ypos0, Ypos1, Color);
            TFT_DrawVLine(Xpos1, Ypos0, Ypos1, Color);
        }  
    }
}


/*******************************************************************************
* Function Name  : TFT_DisplayChar
* Description    : Displays one character (8 x 16). 
* Input          : - Xpos: 0~232
*                  - Ypos: 0~304.
*                  - Ascii: character ascii code, must be between 0x20 and 0x7E.
*                  - TextColor: font color.
*                  - BackColor: background colour.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_DisplayChar(u16 Xpos, u16 Ypos, u8 Ascii ,u16 TextColor, u16 BackColor)
{
    u16 index = 0, i = 0, Yaddress = 0;
    u8 Xaddress = 0, buffer = 0;

    /* 参数检查 */
    if((Xpos > (TFT_XSIZE - 8)) || (Ypos > (TFT_YSIZE - 16)))
    {
        return;
    }
    /* ascii从空格开始(32) */
    Ascii -= 32;
    Xaddress = Xpos;
    Yaddress = Ypos;
    /* 设置坐标 */
    TFT_WriteReg(0x50, Xaddress);       // Horizontal GRAM Start Address
    TFT_WriteReg(0x51, Xaddress + 7);   // Horizontal GRAM End Address
    TFT_WriteReg(0x52, Yaddress);       // Vertical GRAM Start Address
    TFT_WriteReg(0x53, Yaddress + 15);  // Vertical GRAM End Address 
    TFT_SetCursor(Xaddress, Yaddress);
    TFT_WriteRAM_Prepare();             // Prepare to write GRAM

    for(index = 0; index < 16; index++)
    {
        buffer = acFont8x16[Ascii][index];

        //TFT_WriteRAM_Prepare();           // Prepare to write GRAM
        for(i = 0; i < 8; i++)
        {
            if((buffer & (0x80 >> i)) == 0x00)
            {
                TFT->TFT_RAM = BackColor;
            }
            else
            {
                TFT->TFT_RAM = TextColor;
            }
        }
        //Yaddress++;
        //TFT_SetCursor(Xaddress, Yaddress);
    }
}

/*******************************************************************************
* Function Name  : TFT_DisplayString
* Description    : 显示字符串.
* Input          : - Xpos: 0~232
*                  - Ypos: 0~304.
*                  - *ptr: pointer to string to display on TFT.
*                  - TextColor: font color.
*                  - BackColor: background colour.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_DisplayString(u16 Xpos, u16 Ypos, u8 *ptr ,u16 TextColor, u16 BackColor)
{
    u16 xtemp = 0, ytemp = 0;

    /* 参数检查 */
    if((Xpos > (TFT_XSIZE - 8)) || (Ypos > (TFT_YSIZE - 16)))
    {
        return;
    }

    xtemp = Xpos;
    ytemp = Ypos;
    /* Send the string character by character on lCD */
    while(*ptr != 0)
    {
        TFT_DisplayChar(xtemp, ytemp, *ptr, TextColor, BackColor);
        xtemp += 8;
        if(xtemp > (TFT_XSIZE - 8))
        {
            xtemp = Xpos;
            ytemp += 16;
            if(ytemp > (TFT_YSIZE - 16))
            {
                return;
            }
        }
        ptr++;
    }
}

/*******************************************************************************
* Function Name  : TFT_DisplayD36x48
* Description    : Displays Font data (36x48). 
* Input          : - Xpos: 0~TFT_XSIZE - 36
*                  - Ypos: 0~TFT_YSIZE - 48.
*                  - Ascii: character ascii code .
*                  - TextColor: font color.
*                  - BackColor: background colour.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_DisplayD36x48(u16 Xpos, u16 Ypos, u8 Ascii ,u16 TextColor, u16 BackColor)
{
    u16 index = 0, i = 0, j = 0, Yaddress = 0;
    u8 Xaddress = 0, buffer = 0;

    /* 参数检查 */
    if((Xpos > (TFT_XSIZE - 36)) || (Ypos > (TFT_YSIZE - 48)))
    {
        return;
    }

    //  0    1    2    3    4    5    6    7    8    9    10   11   12   13  14
    // '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-', ' ', '.', ':'
    switch(Ascii)
    {
        case '+': Ascii = 10;
                  break;
        case '-': Ascii = 11;
                  break;
        case ' ': Ascii = 12;
                  break;
        case '.': Ascii = 13;
                  break;
        case ':': Ascii = 14;
                  break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': Ascii -= '0';
                  break;
        default:  Ascii = 12;
                  break;
    }
    Xaddress = Xpos;
    Yaddress = Ypos;
    /* 设置坐标 */
    TFT_WriteReg(0x50, Xaddress);       // Horizontal GRAM Start Address
    TFT_WriteReg(0x51, Xaddress + 35);   // Horizontal GRAM End Address
    TFT_WriteReg(0x52, Yaddress);       // Vertical GRAM Start Address
    TFT_WriteReg(0x53, Yaddress + 47);  // Vertical GRAM End Address 
    TFT_SetCursor(Xaddress, Yaddress);
    TFT_WriteRAM_Prepare();             // Prepare to write GRAM

    for(index = 0; index < 240; index++)
    {
        for(j=0; j<4; j++)
        {
            buffer = acFontD36x48[Ascii][index];
            for(i = 0; i < 8; i++)
            {
                if((buffer & (0x80 >> i)) == 0x00)
                {
                    TFT->TFT_RAM = BackColor;
                }
                else
                {
                    TFT->TFT_RAM = TextColor;
                }
            }
            index++;
        }

        buffer = acFontD36x48[Ascii][index];
        for(i = 0; i < 4; i++)
        {
            if((buffer & (0x80 >> i)) == 0x00)
            {
                TFT->TFT_RAM = BackColor;
            }
            else
            {
                TFT->TFT_RAM = TextColor;
            }
        }
    }
}

/*******************************************************************************
* Function Name  : TFT_DisplayIcon
* Description    : Displays ICON (64x64). 
* Input          : - Xpos: 0~TFT_XSIZE - 64
*                  - Ypos: 0~TFT_YSIZE - 64.
*                  - *pIcon: Icon data.
*                  - IconColor: font color.
*                  - BackColor: background colour.
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_DisplayIcon(u16 Xpos, u16 Ypos, const u8 *pIcon, u16 IconColor, u16 BackColor)
{
    u16 index = 0, i = 0, j = 0, Yaddress = 0;
    u8 Xaddress = 0, buffer = 0;

    /* 参数检查 */
    if((Xpos > (TFT_XSIZE - 64)) || (Ypos > (TFT_YSIZE - 64)))
    {
        return;
    }

    Xaddress = Xpos;
    Yaddress = Ypos;
    /* 设置坐标 */
    TFT_WriteReg(0x50, Xaddress);       // Horizontal GRAM Start Address
    TFT_WriteReg(0x51, Xaddress + 63); // Horizontal GRAM End Address
    TFT_WriteReg(0x52, Yaddress);       // Vertical GRAM Start Address
    TFT_WriteReg(0x53, Yaddress + 63); // Vertical GRAM End Address 
    TFT_SetCursor(Xaddress, Yaddress);
    TFT_WriteRAM_Prepare();             // Prepare to write GRAM

    for(index = 0; index < 64; index++)
    {
        for(j=0; j<8; j++)
        {
            buffer = *pIcon;
            for(i = 0; i < 8; i++)
            {
                if((buffer & (0x80 >> i)) == 0x00)
                {
                    TFT->TFT_RAM = BackColor;
                }
                else
                {
                    TFT->TFT_RAM = IconColor;
                }
            }
            pIcon++;
        }
    }
}

/**************************END OF FILE*****************************************/

