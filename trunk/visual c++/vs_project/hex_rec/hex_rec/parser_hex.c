/*! \file  parser_hex.c
* \b �ӿ�˵����
*
*
* \b ��������:
*
*       ����hex��¼
*
* \b ��ʷ��¼:
*
*       <����>          <ʱ��>       <�޸ļ�¼> <br>
*       zuokongxiao    2012-10-17    �������ļ� <br>
*/

/******************************************************************************/
/*                             ͷ�ļ�                                         */
/******************************************************************************/
#include <string.h>
#include "parser_hex.h"
#include "crc16.h"

/******************************************************************************/
/*                             �궨��                                         */
/******************************************************************************/

/******************************************************************************/
/*                              �ṹ����                                      */
/******************************************************************************/

/******************************************************************************/
/*                             ��������                                       */
/******************************************************************************/

/******************************************************************************/
/*                            �ڲ��ӿ�����                                    */
/******************************************************************************/




/******************************************************************************/
/*                            �ڲ��ӿ�ʵ��                                    */
/******************************************************************************/



/******************************************************************************/
/*                                �ⲿ�ӿ�                                    */
/******************************************************************************/
/*! \fn       void fill_send_buf(send_buf_t *pstSendbuf, boot_buf_t *pstBootBuf)
*
*  \brief     ����hex��¼����
*
*  \param     pstSendbuf [out] ���ͻ�����
*
*  \param     pstBootBuf [in] 
*
*  \exception ��
*
*  \return    ��
*/
void fill_send_buf(send_buf_t *pstSendbuf, boot_buf_t *pstBootBuf)
{
    int32 lRet = EXIT_SUCCESS;
    Data32_8_t ulStartAddress;
    uint16 i = 0, unCRCValue = 0;

    memset(pstSendbuf, 0x00, sizeof(send_buf_t));
    /*! ��ʼ��ַ */
    ulStartAddress.d32 = pstBootBuf->ulStaAddr.d32;

    if(RECORD_TYPE_END_OF_FILE != pstBootBuf->ucHexEndFlag)
    {
        if(BOOT_BUF_SIZE == pstBootBuf->unWriteIndex)
        {
            /*! ��ʼ��ַ */
            pstSendbuf->aucData[0] = ulStartAddress.d8.s3;
            pstSendbuf->aucData[1] = ulStartAddress.d8.s2;
            pstSendbuf->aucData[2] = ulStartAddress.d8.s1;
            pstSendbuf->aucData[3] = ulStartAddress.d8.s0;
            /*! ���� */
            for(i=0; i<BOOT_BUF_SIZE; i++)
            {
                pstSendbuf->aucData[4+i] = pstBootBuf->aucBuf[i];
            }
            /*! ����CRC */
            unCRCValue = crc16(0x00, &(pstSendbuf->aucData[0]), BOOT_BUF_SIZE+4);
            pstSendbuf->aucData[BOOT_BUF_SIZE+4] = (uint8)unCRCValue;
            pstSendbuf->aucData[BOOT_BUF_SIZE+5] = (uint8)(unCRCValue>>8);
            /*! ���� */
            pstSendbuf->ucDataLen = BOOT_BUF_SIZE+6;
            pstSendbuf->ucSendFlag = 1;
            /*! ���µ�ַ */
            pstBootBuf->ucPreState = BOOT_STATE_WRITE_FLASH;
            pstBootBuf->ulStaAddr.d32 += BOOT_BUF_SIZE;
            /*! ��д��ʶ */
            pstBootBuf->unWriteIndex = 0;
        }
    }
    else
    {
        /*! ��ʼ��ַ */
        pstSendbuf->aucData[0] = ulStartAddress.d8.s3;
        pstSendbuf->aucData[1] = ulStartAddress.d8.s2;
        pstSendbuf->aucData[2] = ulStartAddress.d8.s1;
        pstSendbuf->aucData[3] = ulStartAddress.d8.s0;
        /*! ���� */
        for(i=0; i<(pstBootBuf->unWriteIndex); i++)
        {
            pstSendbuf->aucData[4+i] = pstBootBuf->aucBuf[i];
        }
        /*! ����CRC */
        unCRCValue = crc16(0x00, &(pstSendbuf->aucData[0]), (pstBootBuf->unWriteIndex)+4);
        pstSendbuf->aucData[(pstBootBuf->unWriteIndex)+4] = (uint8)unCRCValue;
        pstSendbuf->aucData[(pstBootBuf->unWriteIndex)+5] = (uint8)(unCRCValue>>8);
        /*! ���� */
        pstSendbuf->ucDataLen = (pstBootBuf->unWriteIndex)+6;
        pstSendbuf->ucSendFlag = 1;
        /*! ���������ݽṹ */
        memset(pstBootBuf, 0x00, sizeof(boot_buf_t));
    }
}

/*! \fn       int32 parser_hex_record_type(boot_buf_t *pstBootBuf, const hex_record_t *pstHexRecord)
*
*  \brief     ����hex��¼����
*
*  \param     pstBootBuf   [out] 
*
*  \param     pstHexRecord [in] hex�ļ����ձ�־λ
*
*  \exception ��
*
*  \return    EXIT_SUCCESS���ɹ���EXIT_FAILURE��ʧ��
*/
int32 parser_hex_record_type(boot_buf_t *pstBootBuf, hex_record_t *pstHexRecord)
{
    uint16 i = 0;
    Data16_8_t stData16Temp;
    int32 lRet = EXIT_SUCCESS;

    /*! ��¼���� */
    switch(pstHexRecord->ucRecTyp)
    {
        /*! record type: 0x00 */
        case RECORD_TYPE_DATA:
            /*! �����ȡ����hex��¼�е�ַ */
            pstBootBuf->ulCurAddr.d16.l = pstHexRecord->unOffset;
            /*! �жϻ�ȡ�ĵ�ַ�Ƿ�����һ��ַ���� */
            if(pstBootBuf->ulPreAddr.d32 != pstBootBuf->ulCurAddr.d32)
            {
                lRet = EXIT_FAILURE;
                break;
            }
            /*! �ж�д��Flash�е���ʼ��ַ����յ��ĵ�ַ�Ƿ���ͬ */
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
            /*! �������ݵ������� */
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
            pstBootBuf->ucHexEndFlag = RECORD_TYPE_END_OF_FILE; /*!< hex������־λ */
            /*! end of file record ��ʽ�ж� */
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
            /*! ��ȡ��ַ��16λ */
            stData16Temp.d8.h = pstHexRecord->aucData[0];
            stData16Temp.d8.l = pstHexRecord->aucData[1];
            /*! ���µ�ַ��16λ */
            pstBootBuf->ulCurAddr.d16.h = stData16Temp.d16;
            break;
        /*! record type: 0x05 */
        case RECORD_TYPE_START_LIN_ADRR:
            break;
        /*! ����ֵ���� */
        default:
            lRet = EXIT_FAILURE;
            break;
    }
    return lRet;
}


/*! \fn        void init_boot_buf(boot_buf_t *pstBootBuf)
*
*  \brief     ��ʼ��������
*
*  \param     pstBootBuf [in] ָ��BOOT������
*
*  \exception ��
*
*  \return    ��
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
*  \brief     ����hex��¼��
*
*  \param     pstHexRecord  [out] ָ������󻺳���
*
*  \param     pucDataBuf    [in] ��¼��
*
*  \param     ucDataLen     [in] ��¼����
*
*  \exception ��
*
*  \return    EXIT_SUCCESS���ɹ���EXIT_FAILURE��ʧ��
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
            /*! mdk���ɵ�hex�ļ����¼�����ݳ���Ϊ16 */
            break;
        }
        pstHexRecord->ucRecLen = stData8Temp.d8;
        /*! ����У��� */
        ucCheckSum = stData8Temp.d8;
        /*! ����ƫ�Ƶ�ַload offset */
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
            /*! �������¼���ͳ��� */
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
        /*! �ж�У����Ƿ���ȷ */
        if(0x00 != ucCheckSum)
        {
            break;
        }

        lRet = EXIT_SUCCESS;
    }
    while(0);

    return lRet;
}
