/*******************************************************************************
* File Name          : gps.h
* Description        : GPS头文件

* Version            : V1.0
* Date               : 2011/11/06
* Author             : zuokong
* Description        : 创建.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPS_H
#define __GPS_H

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Exported types ------------------------------------------------------------*/
typedef struct gps_info
{ 
    char utc_time[32];
    char status; 
    float latitude_value;
    char latitude; 
    float longtitude_value;
    char longtitude;
    float speed;
    float azimuth_angle; 
    char utc_data[32]; 
} GPS_INFO;
/* Exported constants --------------------------------------------------------*/
#define GPS_EN_PIN          GPIO_Pin_8
#define GPS_EN_PORT         GPIOA
#define GPS_EN              PA8
#define GPS_ENABLE()        {GPS_EN = 0;}      
#define GPS_DISABLE()       {GPS_EN = 1;} 
/* 缓冲区大小 */
#define GPS_BUFFER_SIZE     512
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern char gps_buf[GPS_BUFFER_SIZE]; // gps数据缓冲区
/* Exported functions ------------------------------------------------------- */

void GPS_Configuration(void);
char GPS_GetRMC(GPS_INFO *rmc_info, char *ram_buf);



#endif /* __GPS_H */
/**************************END OF FILE*****************************************/

