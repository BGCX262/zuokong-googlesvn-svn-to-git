/**
  ******************************************************************************
  * @file    debug.c
  * @author  zuokongxiao
  * @version 
  * @date    2012-10-17
  * @brief   printf��������.
  ******************************************************************************
  * @attention
  *
  * xxxxx.
  *
  * <h2><center>&copy; COPYRIGHT 2012 Zuokong2006</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <ctype.h>
#include "stm32f10x.h"
#include "uart.h"
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* External functions --------------------------------------------------------*/
/**
  * @file   int fputc(int ch, FILE *f)
  * @brief  ���ô���
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    uart_send(UART_NUM_1, (u8 *)&ch, 1);

    return ch;
}

/**
  * @file   void uart_configuration(void)
  * @brief  ���ô���
  * @param  None
  * @retval None
  */
ErrorStatus debug_msg(u8 *pFile, u16 ulLine)
{
    u16 unFileSize = 0;
    u8 aLineBuf[16] = {0};
    u8 aLineString[16] = {0};
    u8 ucLineSize = 0;
    u16 ulTemp = 0;
    u8 i = 0;

    if(NULL == pFile)
    {
        return ERROR;
    }
    /*! ����__FILE__�ַ������� */
    while('\0' != *(pFile+unFileSize))
    {
        unFileSize++;
    }
    /*! ��������ļ����� */
    uart_send(UART_NUM_1, pFile, unFileSize);

    /*! ��__LINE__ת��Ϊ�ַ��� */
    aLineString[0] = ' ';
    if(ulLine < 1)
    {
        ucLineSize = 1;
        aLineString[1] = '0';
        aLineString[2] = '\r';
        aLineString[3] = '\n';
    }
    else
    {
        ucLineSize = 0;
        ulTemp = ulLine;
        while(ulTemp != 0)
        {
            aLineBuf[ucLineSize] = ulTemp % 10 + '0';
            ulTemp = ulTemp / 10;
            ucLineSize++;
        }
        /*! �ַ�����˳�� */
        for(i=0; i<ucLineSize; i++)
        {
            aLineString[i+1] = aLineBuf[ucLineSize-i-1];
        }
        aLineString[ucLineSize+1] = '\r'; /*!< �ַ�����β */
        aLineString[ucLineSize+2] = '\n';
    }
    /*! ��������кżӻ��� */
    uart_send(UART_NUM_1, aLineString, ucLineSize+3);

    return SUCCESS;
}

