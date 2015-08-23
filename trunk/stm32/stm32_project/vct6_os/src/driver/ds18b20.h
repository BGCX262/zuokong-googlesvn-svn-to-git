/**
  ******************************************************************************
  * @file    ds18B20.h
  * @author  zuokongxiao
  * @version V1.0
  * @date    2011/11/11
  * @brief   DS18B20头文件
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DS18B20_H
#define __DS18B20_H

/* Includes ------------------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* 温度转换精度定义 */
#define DS18B20_RESOLUTION_BITS_9           (0x1f)
#define DS18B20_RESOLUTION_BITS_10          (0x3f)
#define DS18B20_RESOLUTION_BITS_11          (0x5f)
#define DS18B20_RESOLUTION_BITS_12          (0x7f)
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
void DS18B20_Configuration(void);
ErrorStatus DS18B20_WriteScratchpad(u8 temph, u8 templ, u8 bits);
ErrorStatus DS18B20_CopyScratchpad(void);
ErrorStatus DS18B20_StartTempConvert(void);
ErrorStatus DS18B20_ReadTemperature(float *ptemp);



#endif /* __DS18B20_H */
/**************************END OF FILE*****************************************/

