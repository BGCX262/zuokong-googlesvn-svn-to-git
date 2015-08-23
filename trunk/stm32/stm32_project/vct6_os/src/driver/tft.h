/*******************************************************************************
* File Name          : tft.h
* Description        : 头文件

* Version            : V1.0
* Date               : 2011/07/14
* Author             : zuokong
* Description        : 创建.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TFT_H
#define __TFT_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
typedef enum 
{
    TFT_BL_PERCENT_100 = 0, 
    TFT_BL_PERCENT_90  = 10,
    TFT_BL_PERCENT_80  = 20,
    TFT_BL_PERCENT_70  = 30,
    TFT_BL_PERCENT_60  = 40,
    TFT_BL_PERCENT_50  = 50,
    TFT_BL_PERCENT_40  = 60,
    TFT_BL_PERCENT_30  = 70,
    TFT_BL_PERCENT_20  = 80,
    TFT_BL_PERCENT_10  = 90,
    TFT_BL_PERCENT_0   = 100
} TFTBLMode_TypeDef;

/* Exported constants --------------------------------------------------------*/
#define TFT_DEBUG_EN        1       // Enale(1) or Disable debug
#define TFT_MODE            0       // 0:240 X 320; 1: 320 X 240
/* define TFT size */
#if TFT_MODE > 0
 #define TFT_XSIZE          (320)
 #define TFT_YSIZE          (240)
 #define TFT_MAX_XSIZE      (319)
 #define TFT_MAX_YSIZE      (239)
#else
 #define TFT_XSIZE          (240)
 #define TFT_YSIZE          (320)
 #define TFT_MAX_XSIZE      (239)
 #define TFT_MAX_YSIZE      (319)
#endif
/* define lcd pins */
#define TFT_PIN_NRESET  PD12
#define TFT_PIN_BL      PB0
#define TFT_PIN_RD      PD4
#define TFT_PIN_WR      PD5
#define TFT_PIN_CS      PD7
#define TFT_PIN_RS      PD11
/* LCD color */
#define White           0xFFFF
#define Black           0x0000
#define Grey            0xF7DE
#define Blue            0x001F
#define Blue2           0x051F
#define Red             0xF800
#define Magenta         0xF81F
#define Green           0x07E0
#define Cyan            0x7FFF
#define Yellow          0xFFE0
/* Exported macro ------------------------------------------------------------*/
/* ExPorted variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TFT_SetBlackLight(TFTBLMode_TypeDef TFT_BL_Mode);
void TFT_Init(void);
void TFT_SetCursor(u16 Xpos, u16 Ypos);
void TFT_Clear(u16 Color);

void TFT_SetPixel(u16 Xpos, u16 Ypos, u16 Color);
u16 TFT_GetPixel(u16 Xpos, u16 Ypos);

void TFT_DrawHLine(u16 Xpos0, u16 Ypos, u16 Xpos1, u16 Color);
void TFT_DrawVLine(u16 Xpos, u16 Ypos0, u16 Ypos1, u16 Color);
void TFT_FillRect(u16 Xpos0, u16 Ypos0, u16 Xpos1, u16 Ypos1, u16 Color, u8 Fill);

void TFT_DisplayChar(u16 Xpos, u16 Ypos, u8 Ascii ,u16 TextColor, u16 BackColor);
void TFT_DisplayString(u16 Xpos, u16 Ypos, u8 *ptr ,u16 TextColor, u16 BackColor);

void TFT_DisplayD36x48(u16 Xpos, u16 Ypos, u8 Ascii ,u16 TextColor, u16 BackColor);

void TFT_DisplayIcon(u16 Xpos, u16 Ypos, const u8 *pIcon, u16 IconColor, u16 BackColor);

#endif /* __TFT_H */

/**************************END OF FILE*****************************************/

