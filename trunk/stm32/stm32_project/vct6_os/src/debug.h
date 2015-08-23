/**
  ******************************************************************************
  * @file    debug.h
  * @author  zuokongxiao
  * @version 
  * @date    2012-10-17
  * @brief   printf.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
//#include <string.h>
//#include <ctype.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/*! printf 宏定义 */
/* 打印调试信息 */
#ifndef DEBUG_EN
 #define DEBUG_MSG(arg...)      printf(##arg)
#else
 #define DEBUG_MSG(arg...)      ((void)0)
#endif

/* 打印错误信息 */
#ifndef ERROR_EN
 #define ERROR_MSG(arg...)      printf(##arg)
#else
 #define ERROR_MSG(arg...)      ((void)0)
#endif

/* Exported macro ------------------------------------------------------------*/

/* ExPorted variables --------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* end __DEBUG_H */
/**************************end of file*****************************************/

