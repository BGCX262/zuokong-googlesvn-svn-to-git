/*! \file  parser_hex.c
* \b 接口说明：
*
*
* \b 功能描述:
*
*       解析hex记录
*
* \b 历史记录:
*
*       <作者>          <时间>       <修改记录> <br>
*       zuokongxiao    2012-10-17    创建该文件 <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include <string.h>
#include "parser_hex.h"
#include "crc16.h"

/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/

/******************************************************************************/
/*                              结构定义                                      */
/******************************************************************************/

/******************************************************************************/
/*                             变量定义                                       */
/******************************************************************************/

/******************************************************************************/
/*                            内部接口声明                                    */
/******************************************************************************/




/******************************************************************************/
/*                            内部接口实现                                    */
/******************************************************************************/



/******************************************************************************/
/*                                外部接口                                    */
/******************************************************************************/
/*! \fn       void fill_send_buf(send_buf_t *pstSendbuf, boot_buf_t *pstBootBuf)
*
*  \brief     解析hex记录类型
*
*  \param     pstSendbuf [out] 发送缓冲区
*
*  \param     pstBootBuf [in] 
*
*  \exception 无
*
*  \return    无
*/
void fill_send_buf(send_buf_t *pstSendbuf, boot_buf_t *pstBootBuf)
{
    int32 lRet = EXIT_SUCCESS;
    Data32_8_t ulStartAddress;
    uint16 i = 0, unCRCValue = 0;

    memset(pstSendbuf, 0x00, sizeof(send_buf_t));
    /*! 起始地址 */
    ulStartAddress.d32 = pstBootBuf->ulStaAddr.d32;

    if(RECORD_TYPE_END_OF_FILE != pstBootBuf->ucHexEndFlag)
    {
        if(BOOT_BUF_SIZE == pstBootBuf->unWriteIndex)
        {
            /*! 起始地址 */
            pstSendbuf->aucData[0] = ulStartAddress.d8.s3;
            pstSendbuf->aucData[1] = ulStartAddress.d8.s2;
            pstSendbuf->aucData[2] = ulStartAddress.d8.s1;
            pstSendbuf->aucData[3] = ulStartAddress.d8.s0;
            /*! 数据 */
            for(i=0; i<BOOT_BUF_SIZE; i++)
            {
                pstSendbuf->aucData[4+i] = pstBootBuf->aucBuf[i];
            }
            /*! 计算CRC */
            unCRCValue = crc16(0x00, &(pstSendbuf->aucData[0]), BOOT_BUF_SIZE+4);
            pstSendbuf->aucData[BOOT_BUF_SIZE+4] = (uint8)unCRCValue;
            pstSendbuf->aucData[BOOT_BUF_SIZE+5] = (uint8)(unCRCValue>>8);
            /*! 长度 */
            pstSendbuf->ucDataLen = BOOT_BUF_SIZE+6;
            pstSendbuf->ucSendFlag = 1;
            /*! 更新地址 */
            pstBootBuf->ucPreState = BOOT_STATE_WRITE_FLASH;
            pstBootBuf->ulStaAddr.d32 += BOOT_BUF_SIZE;
            /*! 清写标识 */
            pstBootBuf->unWriteIndex = 0;
        }
    }
    else
    {
        /*! 起始地址 */
        pstSendbuf->aucData[0] = ulStartAddress.d8.s3;
        pstSendbuf->aucData[1] = ulStartAddress.d8.s2;
        pstSendbuf->aucData[2] = ulStartAddress.d8.s1;
        pstSendbuf->aucData[3] = ulStartAddress.d8.s0;
        /*! 数据 */
        for(i=0; i<(pstBootBuf->unWriteIndex); i++)
        {
            pstSendbuf->aucData[4+i] = pstBootBuf->aucBuf[i];
        }
        /*! 计算CRC */
        unCRCValue = crc16(0x00, &(pstSendbuf->aucData[0]), (pstBootBuf->unWriteIndex)+4);
        pstSendbuf->aucData[(pstBootBuf->unWriteIndex)+4] = (uint8)unCRCValue;
        pstSendbuf->aucData[(pstBootBuf->unWriteIndex)+5] = (uint8)(unCRCValue>>8);
        /*! 长度 */
        pstSendbuf->ucDataLen = (pstBootBuf->unWriteIndex)+6;
        pstSendbuf->ucSendFlag = 1;
        /*! 清升级数据结构 */
        memset(pstBootBuf, 0x00, sizeof(boot_buf_t));
    }
}

/*! \fn       int32 parser_hex_record_type(boot_buf_t *pstBootBuf, const hex_record_t *pstHexRecord)
*
*  \brief     解析hex记录类型
*
*  \param     pstBootBuf   [out] 
*
*  \param     pstHexRecord [in] hex文件接收标志位
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功，EXIT_FAILURE：失败
*/
int32 parser_hex_record_type(boot_buf_t *pstBootBuf, hex_record_t *pstHexRecord)
{
    uint16 i = 0;
    Data16_8_t stData16Temp;
    int32 lRet = EXIT_SUCCESS;

    /*! 记录类型 */
    switch(pstHexRecord->ucRecTyp)
    {
        /*! record type: 0x00 */
        case RECORD_TYPE_DATA:
            /*! 保存获取到达hex记录行地址 */
            pstBootBuf->ulCurAddr.d16.l = pstHexRecord->unOffset;
            /*! 判断获取的地址是否与上一地址连续 */
            if(pstBootBuf->ulPreAddr.d32 != pstBootBuf->ulCurAddr.d32)
            {
                lRet = EXIT_FAILURE;
                break;
            }
            /*! 判断写到Flash中的起始地址与接收到的地址是否相同 */
            if((BOOT_STATE_START == pstBootBuf->ucPreState) \
                || (BOOT_STATE_WRITE_FLASH == pstBootBuf->ucPreState))
            {
                if(pstBootBuf->ulStaAddr.d32 != pstBootBuf->ulCurAddr.d32)
                {
                    lRet = EXIT_FAILURE;
                    break;
                }
            }
            pstBootBuf->ucPreState = BOOT_STATE_RECEIVE_DATA;
            /*! 复制数据到缓冲区 */
            if(BOOT_BUF_SIZE < (pstBootBuf->unWriteIndex + pstHexRecord->ucRecLen))
            {
                lRet = EXIT_FAILURE;
                break;
            }
            for(i=0; i<( pstHexRecord->ucRecLen); i++)
            {
                pstBootBuf->aucBuf[pstBootBuf->unWriteIndex+i] = pstHexRecord->aucData[i];
            }
            pstBootBuf->unWriteIndex += (pstHexRecord->ucRecLen);
            pstBootBuf->ulPreAddr.d32 += (pstHexRecord->ucRecLen);
            break;
        /*! record type: 0x01 */
        case RECORD_TYPE_END_OF_FILE:
            pstBootBuf->ucHexEndFlag = RECORD_TYPE_END_OF_FILE; /*!< hex结束标志位 */
            /*! end of file record 格式判断 */
            if((0x00 != pstHexRecord->ucRecLen) || (0x0000 != pstHexRecord->unOffset))
            {
                lRet = EXIT_FAILURE;
                break;
            }
            break;
        /*! record type: 0x02 */
        case RECORD_TYPE_EXT_SEG_ADDR:
            lRet = EXIT_FAILURE;
            break;
        /*! record type: 0x03 */
        case RECORD_TYPE_START_SEG_ADDR:
            lRet = EXIT_FAILURE;
            break;
        /*! record type: 0x04 */
        case RECORD_TYPE_EXT_LIN_ADDR:
            if((0x02 != pstHexRecord->ucRecLen) || (0x0000 != pstHexRecord->unOffset))
            {
                lRet = EXIT_FAILURE;
                break;
            }
            /*! 获取地址高16位 */
            stData16Temp.d8.h = pstHexRecord->aucData[0];
            stData16Temp.d8.l = pstHexRecord->aucData[1];
            /*! 更新地址高16位 */
            pstBootBuf->ulCurAddr.d16.h = stData16Temp.d16;
            break;
        /*! record type: 0x05 */
        case RECORD_TYPE_START_LIN_ADRR:
            break;
        /*! 其他值错误 */
        default:
            lRet = EXIT_FAILURE;
            break;
    }
    return lRet;
}


/*! \fn        void init_boot_buf(boot_buf_t *pstBootBuf)
*
*  \brief     初始化缓冲区
*
*  \param     pstBootBuf [in] 指向BOOT缓冲区
*
*  \exception 无
*
*  \return    无
*/
void init_boot_buf(boot_buf_t *pstBootBuf)
{
    memset(pstBootBuf, 0x00, sizeof(boot_buf_t));
    pstBootBuf->ucPreState = BOOT_STATE_START;
    pstBootBuf->ulStaAddr.d32 = USR_CODE_ADR_START;
    pstBootBuf->ulPreAddr.d32 = USR_CODE_ADR_START;
}

/*! \fn       int32 parser_hex_record(hex_record_t *pstHexRecord,
*                                     uint8 *pucDataBuf, uint8 ucDataLen)
*
*  \brief     解析hex记录行
*
*  \param     pstHexRecord  [out] 指向解析后缓冲区
*
*  \param     pucDataBuf    [in] 记录行
*
*  \param     ucDataLen     [in] 记录长度
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功，EXIT_FAILURE：失败
*/
int32 parser_hex_record(hex_record_t *pstHexRecord, uint8 *pucDataBuf, uint8 ucDataLen)
{
    int32 lRet = EXIT_FAILURE;
    uint8 ucRecordLen = 0, i= 0;
    uint8 aucRecordBuf[64] = {0};
    uint8 ucRecordIndex = 0;
    uint8 ucCheckSum = 0;
    Data8_4_t stData8Temp;
    Data16_4_t stData16Temp;

    do
    {
        /*! record mark */
        if((':' != pucDataBuf[0]) || (0x0D != pucDataBuf[ucDataLen-2]) \
           || (0x0A != pucDataBuf[ucDataLen-1]))
        {
            break;
        }
        /*! ascii -> 0~15 */
        ucRecordLen = ucDataLen-3;
        for(i=0; i<ucRecordLen; i++)
        {
            if(('0' <= pucDataBuf[i+1]) && ('9' >= pucDataBuf[i+1]))
            {
                aucRecordBuf[i] = pucDataBuf[i+1] - '0';
            }
            else if(('A' <= pucDataBuf[i+1]) && ('F' >= pucDataBuf[i+1]))
            {
                aucRecordBuf[i] = pucDataBuf[i+1] - 'A' + 10;
            }
            else if(('a' <= pucDataBuf[i+1]) && ('f' >= pucDataBuf[i+1]))
            {
                aucRecordBuf[i] = pucDataBuf[i+1] - 'a' + 10;
            }
            else
            {
                return EXIT_FAILURE;
            }
        }
        /*! record length */
        stData8Temp.d4.l = aucRecordBuf[1];
        stData8Temp.d4.h = aucRecordBuf[0];
        if((16 < stData8Temp.d8) || ((ucRecordLen-10) != ((stData8Temp.d8)<<1)))
        {
            /*! mdk生成的hex文件最长记录的数据长度为16 */
            break;
        }
        pstHexRecord->ucRecLen = stData8Temp.d8;
        /*! 计算校验和 */
        ucCheckSum = stData8Temp.d8;
        /*! 解析偏移地址load offset */
        stData16Temp.d4.s0 = aucRecordBuf[5];
        stData16Temp.d4.s1 = aucRecordBuf[4];
        stData16Temp.d4.s2 = aucRecordBuf[3];
        stData16Temp.d4.s3 = aucRecordBuf[2];
        pstHexRecord->unOffset = stData16Temp.d16;
        ucCheckSum += stData16Temp.d8[0];
        ucCheckSum += stData16Temp.d8[1];
        /*! record type */
        stData8Temp.d4.l = aucRecordBuf[7];
        stData8Temp.d4.h = aucRecordBuf[6];
        if(0x05 < stData8Temp.d8)
        {
            /*! 解析后记录类型出错 */
            break;
        }
        pstHexRecord->ucRecTyp = stData8Temp.d8;
        ucCheckSum += stData8Temp.d8;
        /*! data or info */
        ucRecordLen = ucRecordLen - 8;
        ucRecordLen >>= 1;
        ucRecordIndex = 8;
        for(i=0; i<ucRecordLen; i++)
        {
            stData8Temp.d4.l = aucRecordBuf[ucRecordIndex+1];
            stData8Temp.d4.h = aucRecordBuf[ucRecordIndex];
            pstHexRecord->aucData[i] = stData8Temp.d8;
            ucRecordIndex += 2;
            ucCheckSum += stData8Temp.d8;
        }
        /*! 判断校验和是否正确 */
        if(0x00 != ucCheckSum)
        {
            break;
        }

        lRet = EXIT_SUCCESS;
    }
    while(0);

    return lRet;
}

