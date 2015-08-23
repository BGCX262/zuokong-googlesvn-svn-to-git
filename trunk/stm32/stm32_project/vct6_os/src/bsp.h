/**
  ******************************************************************************
  * @file    bsp.h
  * @author  zuokongxiao
  * @version V1.0
  * @date    2012-10-17
  * @brief   Í·ÎÄ¼þ.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H

/* Includes ------------------------------------------------------------------*/
//#include "includes.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define BEPP            PA1
#define BEEP_PIN        GPIO_Pin_1
#define BEEP_PORT       GPIOA
#define BEEP_ON()       (BEPP = 0)
#define BEEP_OFF()      (BEPP = 1)
/* Exported macro ------------------------------------------------------------*/
/* ExPorted variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void target_init(void);
void systick_configuration(void);


#endif /* __BSP_H */

/**************************END OF FILE*****************************************/

