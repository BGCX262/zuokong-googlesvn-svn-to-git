/*! \file  debug.c
* \b 接口说明：  
*       
*
* \b 功能描述:
*
*       调试接口
*
* \b 历史记录:     
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2012-05-16   创建文件   <br>
*/

/******************************************************************************/
/*                            头文件                                          */
/******************************************************************************/
#include "stm32f10x_type.h"
#include "deftype.h"
#include "debug.h"
#include "stdint.h"
#include "uart.h"


/******************************************************************************/
/*                           宏定义                                           */
/******************************************************************************/

/******************************************************************************/
/*                           变量定义                                         */
/******************************************************************************/

/******************************************************************************/
/*                         内部接口声明                                       */
/******************************************************************************/


/******************************************************************************/
/*                         外部接口实现                                       */
/******************************************************************************/


/******************************************************************************/
/*                           内部接口实现                                     */
/******************************************************************************/

/*! \fn       int fputc(int ch, FILE *f)
*
*  \brief     送一个字符到一个流中
*
*  \param     ch    [in]  需发送的数据
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
*  \brief     输出调用此函数的文件名及行号(debug_msg(__FILE__, __LINE__);)
*
*  \param     *pFile [in]   指向文件名字符串
*
*  \param     ulLine [in]   行号(不能大于65535)
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功，EXIT_FAILURE：失败
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
    /*! 计算__FILE__字符串长度 */
    while('\0' != *(pFile+unFileSize))
    {
        unFileSize++;
    }
    /*! 串口输出文件名称 */
    uart_send(pFile, unFileSize);

    /*! 将__LINE__转换为字符串 */
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
        /*! 字符串反顺序 */
        for(i=0; i<ucLineSize; i++)
        {
            aLineString[i+1] = aLineBuf[ucLineSize-i-1];
        }
        aLineString[ucLineSize+1] = '\r'; /*!< 字符串结尾 */
        aLineString[ucLineSize+2] = '\n';
    }
    /*! 串口输出行号加换行 */
    uart_send(aLineString, ucLineSize+3);

    return EXIT_SUCCESS;
}

/********************************end of file***********************************/

