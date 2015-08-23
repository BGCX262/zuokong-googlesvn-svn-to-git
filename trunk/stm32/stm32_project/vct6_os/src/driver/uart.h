/**
  ******************************************************************************
  * @file    uart.h
  * @author  zuokongxiao
  * @version 
  * @date    2012-10-17
  * @brief   串口驱动.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/*  串口定义 */
#define UART_NUM_1                  (1u)
#define UART_NUM_2                  (2u)
#define UART_NUM_3                  (3u)
#define UART_NUM_4                  (4u)
#define UART_NUM_5                  (5u)
/* 串口缓冲区大小 */
#define UART_BUF_SIZE               (256u)
/* 串口 */
#define UART_EX_BUF_SIZE            (64u)

/* Exported macro ------------------------------------------------------------*/
/* ExPorted variables --------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
void uart_configuration(void);
void uart_send(u8 ucUartNum, const u8 *pucSendBuf, u16 unSendLen);

void uart_read(void);
u8 *retrieve_uart_receive_buf(void);
u16 *retrieve_uart_read_index(void);
u16 *retrieve_uart_write_index(void);


#ifdef __cplusplus
}
#endif

#endif /* end __UART_H */
/**************************end of file*****************************************/

