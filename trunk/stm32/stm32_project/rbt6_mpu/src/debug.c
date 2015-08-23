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

/*! \fn       int32 put_string(int8 *pPutStr)
*
*  \brief     输出字符串(在调试串口)
*
*  \param     *pPutStr [in] 指向输出的字符串
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功，EXIT_FAILURE：失败
*/
int32 put_string(int8 *pPutStr)
{
    int32 lRet = EXIT_FAILURE;
    uint16 unOutStrLen = 0;

    do
    {
        /*! 输入参数检查 */
        if(INVALID_POINTER(pPutStr))
        {
            break;
        }
        /*! 计算输出字符串的长度 */
        unOutStrLen = 0;
        while('\0' != *(pPutStr+unOutStrLen))
        {
            unOutStrLen++;
        }
        /*! 串口输出 */
        lRet = uart_send((uint8 *)pPutStr, unOutStrLen);
    }while(0);

    return lRet;
}

/*! \fn       int32 put_file_line(int8 *pFile, uint16 ulLine)
*
*  \brief     输出调用此函数的文件名及行号(debug_msg(__FILE__, __LINE__);)
*
*  \param     *pFile [in] 指向文件名字符串
*
*  \param     ulLine [in] 行号(不能大于65535)
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功，EXIT_FAILURE：失败
*/
int32 put_file_line(int8 *pFile, uint16 ulLine)
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
    /*! 2013-05-22 只打印最后一个“\”后的内容 */
    for(ulTemp=(unFileSize-1); ulTemp != 0; ulTemp--)
    {
        if(0x5C == *(pFile+ulTemp))
        {
            break;
        }
    }
    /*! 串口输出文件名称 */
    uart_send(" ", 1);
    uart_send((uint8 *)(pFile+ulTemp), (unFileSize-ulTemp));

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

/*! \fn       int32 debug_msg(int8 *pString, int8 *pFile, uint16 ulLine)
*
*  \brief     输出调试信息
*
*  \param     pString [in] 输出的字符串
*
*  \param     *pFile  [in] __FILE__/为NULL表示不打印文件名及行号
*
*  \param     ulLine  [in] __LINE__
*
*  \exception 注意：在输出行号后会打印”\r\n“
*
*  \return    EXIT_SUCCESS：成功，EXIT_FAILURE：失败
*/
int32 debug_msg(int8 *pString, int8 *pFile, uint16 ulLine)
{
    int32 lRet = EXIT_SUCCESS;

    /*! 输出字符串 */
    if(NULL != pString)
    {
        if(EXIT_FAILURE == put_string(pString))
        {
            lRet = EXIT_FAILURE;
        }
    }

    /*! 输出调用此函数的文件名与行号 */
    if(NULL != pFile)
    {
        if(EXIT_FAILURE == put_file_line(pFile, ulLine))
        {
            lRet = EXIT_FAILURE;
        }
    }

    return lRet;
}


/********************************end of file***********************************/

