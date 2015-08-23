/*! \file  debug.c
* \b �ӿ�˵����  
*       
*
* \b ��������:
*
*       ���Խӿ�
*
* \b ��ʷ��¼:     
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2012-05-16   �����ļ�   <br>
*/

/******************************************************************************/
/*                            ͷ�ļ�                                          */
/******************************************************************************/
#include "stm32f10x_type.h"
#include "deftype.h"
#include "debug.h"
#include "stdint.h"
#include "uart.h"


/******************************************************************************/
/*                           �궨��                                           */
/******************************************************************************/

/******************************************************************************/
/*                           ��������                                         */
/******************************************************************************/

/******************************************************************************/
/*                         �ڲ��ӿ�����                                       */
/******************************************************************************/


/******************************************************************************/
/*                         �ⲿ�ӿ�ʵ��                                       */
/******************************************************************************/


/******************************************************************************/
/*                           �ڲ��ӿ�ʵ��                                     */
/******************************************************************************/

/*! \fn       int fputc(int ch, FILE *f)
*
*  \brief     ��һ���ַ���һ������
*
*  \param     ch    [in]  �跢�͵�����
*
*  \param     *f    [in]  
*
*  \exception None
*
*  \return    ch
*/
int fputc(int ch, FILE *f)
{
    uart_send((uint8 *)&ch, 1);

    return ch;
}

/*! \fn       int32 debug_msg(uint8 *pFile, uint16 ulLine)
*
*  \brief     ������ô˺������ļ������к�(debug_msg(__FILE__, __LINE__);)
*
*  \param     *pFile [in]   ָ���ļ����ַ���
*
*  \param     ulLine [in]   �к�(���ܴ���65535)
*
*  \exception ��
*
*  \return    EXIT_SUCCESS���ɹ���EXIT_FAILURE��ʧ��
*/
int32 debug_msg(uint8 *pFile, uint16 ulLine)
{
    uint16 unFileSize = 0;
    uint8 aLineBuf[16] = {0};
    uint8 aLineString[16] = {0};
    uint8 ucLineSize = 0;
    uint16 ulTemp = 0;
    uint8 i = 0;

    if(INVALID_POINTER(pFile))
    {
        return EXIT_FAILURE;
    }
    /*! ����__FILE__�ַ������� */
    while('\0' != *(pFile+unFileSize))
    {
        unFileSize++;
    }
    /*! ��������ļ����� */
    uart_send(pFile, unFileSize);

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
    uart_send(aLineString, ucLineSize+3);

    return EXIT_SUCCESS;
}

/********************************end of file***********************************/
