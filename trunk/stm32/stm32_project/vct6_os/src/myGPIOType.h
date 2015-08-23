/*******************************************************************************
* File Name          : myGPIOType.h
* Description        : 用位带区地址描述端口.

* Version            : v1.0
* Date               : 2011/03/21
* Author             : zuokong
* Description        : 创建.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYGPIOTYPE_H
#define __MYGPIOTYPE_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define BITBAND(addr, bitnum)    ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)           *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR( BITBAND(addr, bitnum)  )

#define GPIOA_ODR_Addr           (GPIOA_BASE+12)  // 0x4001080C
#define GPIOB_ODR_Addr           (GPIOB_BASE+12)  // 0x40010C0C
#define GPIOC_ODR_Addr           (GPIOC_BASE+12)  // 0x4001100C
#define GPIOD_ODR_Addr           (GPIOD_BASE+12)  // 0x4001140C
#define GPIOE_ODR_Addr           (GPIOE_BASE+12)  // 0x4001180C

#define GPIOA_IDR_Addr           (GPIOA_BASE+8)   // 0x40010808
#define GPIOB_IDR_Addr           (GPIOB_BASE+8)   // 0x40010C08
#define GPIOC_IDR_Addr           (GPIOC_BASE+8)   // 0x40011008
#define GPIOD_IDR_Addr           (GPIOD_BASE+8)   // 0x40011408
#define GPIOE_IDR_Addr           (GPIOE_BASE+8)   // 0x40001808

//-------------------------------------------------------------
#define PA0                      BIT_ADDR(GPIOA_ODR_Addr, 0) 
#define PA1                      BIT_ADDR(GPIOA_ODR_Addr, 1) 
#define PA2                      BIT_ADDR(GPIOA_ODR_Addr, 2) 
#define PA3                      BIT_ADDR(GPIOA_ODR_Addr, 3) 
#define PA4                      BIT_ADDR(GPIOA_ODR_Addr, 4) 
#define PA5                      BIT_ADDR(GPIOA_ODR_Addr, 5) 
#define PA6                      BIT_ADDR(GPIOA_ODR_Addr, 6)
#define PA7                      BIT_ADDR(GPIOA_ODR_Addr, 7) 
#define PA8                      BIT_ADDR(GPIOA_ODR_Addr, 8) 
#define PA9                      BIT_ADDR(GPIOA_ODR_Addr, 9) 
#define PA10                     BIT_ADDR(GPIOA_ODR_Addr, 10) 
#define PA11                     BIT_ADDR(GPIOA_ODR_Addr, 11) 
#define PA12                     BIT_ADDR(GPIOA_ODR_Addr, 12) 
#define PA13                     BIT_ADDR(GPIOA_ODR_Addr, 13) 
#define PA14                     BIT_ADDR(GPIOA_ODR_Addr, 14) 
#define PA15                     BIT_ADDR(GPIOA_ODR_Addr, 15)   

#define PA0IN                    BIT_ADDR(GPIOA_IDR_Addr, 0)
#define PA1IN                    BIT_ADDR(GPIOA_IDR_Addr, 1)
#define PA2IN                    BIT_ADDR(GPIOA_IDR_Addr, 2)
#define PA3IN                    BIT_ADDR(GPIOA_IDR_Addr, 3)
#define PA4IN                    BIT_ADDR(GPIOA_IDR_Addr, 4)
#define PA5IN                    BIT_ADDR(GPIOA_IDR_Addr, 5)
#define PA6IN                    BIT_ADDR(GPIOA_IDR_Addr, 6)
#define PA7IN                    BIT_ADDR(GPIOA_IDR_Addr, 7)
#define PA8IN                    BIT_ADDR(GPIOA_IDR_Addr, 8)
#define PA9IN                    BIT_ADDR(GPIOA_IDR_Addr, 9)
#define PA10IN                   BIT_ADDR(GPIOA_IDR_Addr, 10)
#define PA11IN                   BIT_ADDR(GPIOA_IDR_Addr, 11)
#define PA12IN                   BIT_ADDR(GPIOA_IDR_Addr, 12)
#define PA13IN                   BIT_ADDR(GPIOA_IDR_Addr, 13)
#define PA14IN                   BIT_ADDR(GPIOA_IDR_Addr, 14)
#define PA15IN                   BIT_ADDR(GPIOA_IDR_Addr, 15)

//-------------------------------------------------------------
#define PB0                      BIT_ADDR(GPIOB_ODR_Addr, 0) 
#define PB1                      BIT_ADDR(GPIOB_ODR_Addr, 1) 
#define PB2                      BIT_ADDR(GPIOB_ODR_Addr, 2) 
#define PB3                      BIT_ADDR(GPIOB_ODR_Addr, 3) 
#define PB4                      BIT_ADDR(GPIOB_ODR_Addr, 4) 
#define PB5                      BIT_ADDR(GPIOB_ODR_Addr, 5) 
#define PB6                      BIT_ADDR(GPIOB_ODR_Addr, 6)
#define PB7                      BIT_ADDR(GPIOB_ODR_Addr, 7) 
#define PB8                      BIT_ADDR(GPIOB_ODR_Addr, 8) 
#define PB9                      BIT_ADDR(GPIOB_ODR_Addr, 9) 
#define PB10                     BIT_ADDR(GPIOB_ODR_Addr, 10) 
#define PB11                     BIT_ADDR(GPIOB_ODR_Addr, 11) 
#define PB12                     BIT_ADDR(GPIOB_ODR_Addr, 12) 
#define PB13                     BIT_ADDR(GPIOB_ODR_Addr, 13) 
#define PB14                     BIT_ADDR(GPIOB_ODR_Addr, 14) 
#define PB15                     BIT_ADDR(GPIOB_ODR_Addr, 15)   

#define PB0IN                    BIT_ADDR(GPIOB_IDR_Addr, 0)
#define PB1IN                    BIT_ADDR(GPIOB_IDR_Addr, 1)
#define PB2IN                    BIT_ADDR(GPIOB_IDR_Addr, 2)
#define PB3IN                    BIT_ADDR(GPIOB_IDR_Addr, 3)
#define PB4IN                    BIT_ADDR(GPIOB_IDR_Addr, 4)
#define PB5IN                    BIT_ADDR(GPIOB_IDR_Addr, 5)
#define PB6IN                    BIT_ADDR(GPIOB_IDR_Addr, 6)
#define PB7IN                    BIT_ADDR(GPIOB_IDR_Addr, 7)
#define PB8IN                    BIT_ADDR(GPIOB_IDR_Addr, 8)
#define PB9IN                    BIT_ADDR(GPIOB_IDR_Addr, 9)
#define PB10IN                   BIT_ADDR(GPIOB_IDR_Addr, 10)
#define PB11IN                   BIT_ADDR(GPIOB_IDR_Addr, 11)
#define PB12IN                   BIT_ADDR(GPIOB_IDR_Addr, 12)
#define PB13IN                   BIT_ADDR(GPIOB_IDR_Addr, 13)
#define PB14IN                   BIT_ADDR(GPIOB_IDR_Addr, 14)
#define PB15IN                   BIT_ADDR(GPIOB_IDR_Addr, 15)

//-------------------------------------------------------------
#define PC0                      BIT_ADDR(GPIOC_ODR_Addr, 0) 
#define PC1                      BIT_ADDR(GPIOC_ODR_Addr, 1) 
#define PC2                      BIT_ADDR(GPIOC_ODR_Addr, 2) 
#define PC3                      BIT_ADDR(GPIOC_ODR_Addr, 3) 
#define PC4                      BIT_ADDR(GPIOC_ODR_Addr, 4) 
#define PC5                      BIT_ADDR(GPIOC_ODR_Addr, 5) 
#define PC6                      BIT_ADDR(GPIOC_ODR_Addr, 6)
#define PC7                      BIT_ADDR(GPIOC_ODR_Addr, 7) 
#define PC8                      BIT_ADDR(GPIOC_ODR_Addr, 8) 
#define PC9                      BIT_ADDR(GPIOC_ODR_Addr, 9) 
#define PC10                     BIT_ADDR(GPIOC_ODR_Addr, 10) 
#define PC11                     BIT_ADDR(GPIOC_ODR_Addr, 11) 
#define PC12                     BIT_ADDR(GPIOC_ODR_Addr, 12) 
#define PC13                     BIT_ADDR(GPIOC_ODR_Addr, 13) 
#define PC14                     BIT_ADDR(GPIOC_ODR_Addr, 14) 
#define PC15                     BIT_ADDR(GPIOC_ODR_Addr, 15)   

#define PC0IN                    BIT_ADDR(GPIOC_IDR_Addr, 0)
#define PC1IN                    BIT_ADDR(GPIOC_IDR_Addr, 1)
#define PC2IN                    BIT_ADDR(GPIOC_IDR_Addr, 2)
#define PC3IN                    BIT_ADDR(GPIOC_IDR_Addr, 3)
#define PC4IN                    BIT_ADDR(GPIOC_IDR_Addr, 4)
#define PC5IN                    BIT_ADDR(GPIOC_IDR_Addr, 5)
#define PC6IN                    BIT_ADDR(GPIOC_IDR_Addr, 6)
#define PC7IN                    BIT_ADDR(GPIOC_IDR_Addr, 7)
#define PC8IN                    BIT_ADDR(GPIOC_IDR_Addr, 8)
#define PC9IN                    BIT_ADDR(GPIOC_IDR_Addr, 9)
#define PC10IN                   BIT_ADDR(GPIOC_IDR_Addr, 10)
#define PC11IN                   BIT_ADDR(GPIOC_IDR_Addr, 11)
#define PC12IN                   BIT_ADDR(GPIOC_IDR_Addr, 12)
#define PC13IN                   BIT_ADDR(GPIOC_IDR_Addr, 13)
#define PC14IN                   BIT_ADDR(GPIOC_IDR_Addr, 14)
#define PC15IN                   BIT_ADDR(GPIOC_IDR_Addr, 15)

//-------------------------------------------------------------
#define PD0                      BIT_ADDR(GPIOD_ODR_Addr, 0) 
#define PD1                      BIT_ADDR(GPIOD_ODR_Addr, 1) 
#define PD2                      BIT_ADDR(GPIOD_ODR_Addr, 2) 
#define PD3                      BIT_ADDR(GPIOD_ODR_Addr, 3) 
#define PD4                      BIT_ADDR(GPIOD_ODR_Addr, 4) 
#define PD5                      BIT_ADDR(GPIOD_ODR_Addr, 5) 
#define PD6                      BIT_ADDR(GPIOD_ODR_Addr, 6)
#define PD7                      BIT_ADDR(GPIOD_ODR_Addr, 7) 
#define PD8                      BIT_ADDR(GPIOD_ODR_Addr, 8) 
#define PD9                      BIT_ADDR(GPIOD_ODR_Addr, 9) 
#define PD10                     BIT_ADDR(GPIOD_ODR_Addr, 10) 
#define PD11                     BIT_ADDR(GPIOD_ODR_Addr, 11) 
#define PD12                     BIT_ADDR(GPIOD_ODR_Addr, 12) 
#define PD13                     BIT_ADDR(GPIOD_ODR_Addr, 13) 
#define PD14                     BIT_ADDR(GPIOD_ODR_Addr, 14) 
#define PD15                     BIT_ADDR(GPIOD_ODR_Addr, 15)   

#define PD0IN                    BIT_ADDR(GPIOD_IDR_Addr, 0)
#define PD1IN                    BIT_ADDR(GPIOD_IDR_Addr, 1)
#define PD2IN                    BIT_ADDR(GPIOD_IDR_Addr, 2)
#define PD3IN                    BIT_ADDR(GPIOD_IDR_Addr, 3)
#define PD4IN                    BIT_ADDR(GPIOD_IDR_Addr, 4)
#define PD5IN                    BIT_ADDR(GPIOD_IDR_Addr, 5)
#define PD6IN                    BIT_ADDR(GPIOD_IDR_Addr, 6)
#define PD7IN                    BIT_ADDR(GPIOD_IDR_Addr, 7)
#define PD8IN                    BIT_ADDR(GPIOD_IDR_Addr, 8)
#define PD9IN                    BIT_ADDR(GPIOD_IDR_Addr, 9)
#define PD10IN                   BIT_ADDR(GPIOD_IDR_Addr, 10)
#define PD11IN                   BIT_ADDR(GPIOD_IDR_Addr, 11)
#define PD12IN                   BIT_ADDR(GPIOD_IDR_Addr, 12)
#define PD13IN                   BIT_ADDR(GPIOD_IDR_Addr, 13)
#define PD14IN                   BIT_ADDR(GPIOD_IDR_Addr, 14)
#define PD15IN                   BIT_ADDR(GPIOD_IDR_Addr, 15)

//-------------------------------------------------------------
#define PE0                      BIT_ADDR(GPIOE_ODR_Addr, 0) 
#define PE1                      BIT_ADDR(GPIOE_ODR_Addr, 1) 
#define PE2                      BIT_ADDR(GPIOE_ODR_Addr, 2) 
#define PE3                      BIT_ADDR(GPIOE_ODR_Addr, 3) 
#define PE4                      BIT_ADDR(GPIOE_ODR_Addr, 4) 
#define PE5                      BIT_ADDR(GPIOE_ODR_Addr, 5) 
#define PE6                      BIT_ADDR(GPIOE_ODR_Addr, 6)
#define PE7                      BIT_ADDR(GPIOE_ODR_Addr, 7) 
#define PE8                      BIT_ADDR(GPIOE_ODR_Addr, 8) 
#define PE9                      BIT_ADDR(GPIOE_ODR_Addr, 9) 
#define PE10                     BIT_ADDR(GPIOE_ODR_Addr, 10) 
#define PE11                     BIT_ADDR(GPIOE_ODR_Addr, 11) 
#define PE12                     BIT_ADDR(GPIOE_ODR_Addr, 12) 
#define PE13                     BIT_ADDR(GPIOE_ODR_Addr, 13) 
#define PE14                     BIT_ADDR(GPIOE_ODR_Addr, 14) 
#define PE15                     BIT_ADDR(GPIOE_ODR_Addr, 15)   

#define PE0IN                    BIT_ADDR(GPIOE_IDR_Addr, 0)
#define PE1IN                    BIT_ADDR(GPIOE_IDR_Addr, 1)
#define PE2IN                    BIT_ADDR(GPIOE_IDR_Addr, 2)
#define PE3IN                    BIT_ADDR(GPIOE_IDR_Addr, 3)
#define PE4IN                    BIT_ADDR(GPIOE_IDR_Addr, 4)
#define PE5IN                    BIT_ADDR(GPIOE_IDR_Addr, 5)
#define PE6IN                    BIT_ADDR(GPIOE_IDR_Addr, 6)
#define PE7IN                    BIT_ADDR(GPIOE_IDR_Addr, 7)
#define PE8IN                    BIT_ADDR(GPIOE_IDR_Addr, 8)
#define PE9IN                    BIT_ADDR(GPIOE_IDR_Addr, 9)
#define PE10IN                   BIT_ADDR(GPIOE_IDR_Addr, 10)
#define PE11IN                   BIT_ADDR(GPIOE_IDR_Addr, 11)
#define PE12IN                   BIT_ADDR(GPIOE_IDR_Addr, 12)
#define PE13IN                   BIT_ADDR(GPIOE_IDR_Addr, 13)
#define PE14IN                   BIT_ADDR(GPIOE_IDR_Addr, 14)
#define PE15IN                   BIT_ADDR(GPIOE_IDR_Addr, 15)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#endif /* __MYGPIOTYPE_H */

/************************END OF FILE*******************************************/




