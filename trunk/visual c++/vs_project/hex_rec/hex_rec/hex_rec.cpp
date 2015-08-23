// hex_rec.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "parser_hex.h"
#include <stdio.h>
#include <string.h>

#define READ_BUF_SIZE       (64u)


/*! \fn     signed int find_hex_record(unsigned char *pucHexRecBuf,
*                                      unsigned char *ucCount,
*                                      const unsigned char *pucReadBuf,
*                                      unsigned int ulLen)
*
*  \brief     查找hex记录
*
*  \param     ulAddress     [in]  指定地址
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功，EXIT_FAILURE：失败
*/
signed int find_hex_record(unsigned char *pucHexRecBuf,
                           unsigned char *ucCount,
                           const unsigned char *pucReadBuf,
                           unsigned int ulLen)
{
    signed int lRet = EXIT_FAILURE;
    unsigned char ucStatus = 0, ucError = 0, ucCnt = 0;
    unsigned int i = 0;

    for(i=0; i<ulLen; i++)
    {
        switch(ucStatus)
        {
            /*! 检查hex行: */
            case 0:
                if(':' == pucReadBuf[i])
                {
                    ucStatus = 1;
                    ucCnt = 0;
                    ucError = 0;
                    pucHexRecBuf[ucCnt++] = ':';
                }
                break;
            case 1:
                if(0x0D != pucReadBuf[i])
                {
                    /*! 判断数据区 */
                    if((pucReadBuf[i] < '0') || (pucReadBuf[i] > 'F'))
                    {
                        ucError = 1;
                        printf("ERR1\r\n");
                        break;
                    }
                    if((pucReadBuf[i] > '9') && (pucReadBuf[i] < 'A'))
                    {
                        ucError = 1;
                        printf("ERR2\r\n");
                        break;
                    }
                    /*! 判断数据计数 */
                    if(ucCnt > 42)
                    {
                        ucError = 1;
                        printf("ERR3\r\n");
                        break;
                    }
                    pucHexRecBuf[ucCnt++] = pucReadBuf[i];
                }
                else
                {
                    pucHexRecBuf[ucCnt++] = pucReadBuf[i];
                    ucStatus = 2;
                }
                break;

            case 2:
                if(0x0A != pucReadBuf[i])
                {
                    ucError = 1;
                    printf("ERR4\r\n");
                    break;
                }
                pucHexRecBuf[ucCnt++] = pucReadBuf[i];
                /*! 判断数据计数 */
                if(ucCnt > 45)
                {
                    ucError = 1;
                    printf("ERR5\r\n");
                    break;
                }
                ucError = 2;
                *ucCount = ucCnt;
                break;
            default:
                ucError = 1;
                printf("ERR6\r\n");
                break;
        } /*!< end switch(ucStatus) */

        if(1 == ucError)
        {
            printf("ERR7\r\n");
            break;
        }
        else if(2 == ucError)
        {
            lRet = EXIT_SUCCESS;
            break;
        }
    } /*!< end for(i=0; i<ulLen; i++) */

    return lRet;
}

/*! \fn       int main()
*
*  \brief     主函数
*
*  \param     None
*
*  \exception None
*
*  \return    None
*/
int _tmain(int argc, _TCHAR* argv[])
{
    FILE *fp;
    unsigned char aucReadBuf[READ_BUF_SIZE] = {0};
    unsigned char aucHexRecord[READ_BUF_SIZE] = {0};
    unsigned char ucHexRecordLen = 0;
    unsigned int ulFileOffset = 0, i = 0, ulReadLen = 0;
    hex_record_t stHexRecord;
    boot_buf_t stBootBuf;
    send_buf_t stSendBuf;

    /*! 以二进制文件打开 */
    fp = fopen("E:\\auto_board_download.hex", "rb");
    if(NULL == fp)
    { /*以文本只写方式打开文件*/
        printf("cannot open file\r\n");
        return 0;
    }
    /*! 初始化缓冲区 */
    init_boot_buf(&stBootBuf);
    /*! 读hex文件 */
    while(0 != (ulReadLen = fread(aucReadBuf, 1, READ_BUF_SIZE, fp)))
    {
        //printf("ulReadLen: %d\r\n", ulReadLen);

        /*! 查找hex记录 */
        if(EXIT_FAILURE == find_hex_record(aucHexRecord, &ucHexRecordLen,
                                           aucReadBuf, READ_BUF_SIZE))
        {
            printf("ERR:find_hex_record() return!\r\n");
            break;
        }
        /*! 解析hex记录 */
        if(EXIT_FAILURE == parser_hex_record(&stHexRecord,
                                             aucHexRecord, ucHexRecordLen))
        {
            printf("ERR:parser_hex_record() return!\r\n");
            break;
        }
        /*! 数据 */
        if(EXIT_FAILURE == parser_hex_record_type(&stBootBuf, &stHexRecord))
        {
            printf("ERR:parser_hex_record_type() return!\r\n");
            break;
        }
        /*! 发送数据 */
        fill_send_buf(&stSendBuf, &stBootBuf);
        if(1 == stSendBuf.ucSendFlag)
        {
            /*! 打印数据 */
            printf("Data Len: %03d  Data: \r\n",
                stSendBuf.ucDataLen);
            for(i=0; i<stSendBuf.ucDataLen; i++)
            {
                printf("0x%02x ", stSendBuf.aucData[i]);
            }
            printf("\r\n\r\n");
        }


#if 0
        if(RECORD_TYPE_END_OF_FILE != stBootBuf.ucHexEndFlag)
        {
            if(BOOT_BUF_SIZE == stBootBuf.unWriteIndex)
            {
                /*! 打印数据 */
                printf("Start Addr: 0x%08x, Data Len: 128, Data: ",
                    stBootBuf.ulStaAddr.d32);
                for(i=0; i<stBootBuf.unWriteIndex; i++)
                {
                    printf("0x%02x ", stBootBuf.aucBuf[i]);
                }
                printf("\r\n\r\n");
                /*! 更新地址 */
                stBootBuf.ucPreState = BOOT_STATE_WRITE_FLASH;
                stBootBuf.ulStaAddr.d32 += BOOT_BUF_SIZE;
                /*! 清写标识 */
                stBootBuf.unWriteIndex = 0;
            }
        }
        else
        {
            /*! 打印数据 */
            printf("Start Addr: 0x%08x, Data Len: %03d, Data: ",
                    stBootBuf.ulStaAddr.d32, stBootBuf.unWriteIndex);
            for(i=0; i<BOOT_BUF_SIZE; i++)
            {
                    printf("0x%02x ", stBootBuf.aucBuf[i]);
            }
            printf("\r\n\r\n");
            /*! 清升级数据结构 */
            memset(&stBootBuf, 0x00, sizeof(boot_buf_t));
        }
#endif

#if 0
        else /*! 打印hex记录 */
        {
            printf("Rec Type: 0x%02x, Offset: 0x%04x, Data Len: %02d, Data: ",
                stHexRecord.ucRecTyp, stHexRecord.unOffset, stHexRecord.ucRecLen);
            for(i=0; i<stHexRecord.ucRecLen; i++)
            {
                printf("0x%02x ", stHexRecord.aucData[i]);
            }
            printf("\r\n");
        }
#endif
        /*! 更新偏移地址 */
        ulFileOffset += ucHexRecordLen;
        /*! 更新文件指针 */
        if(0 != fseek(fp, ulFileOffset, 0))
        {
            printf("ERR:fseek return!\r\n");
            break;
        }
    }
    /*! 关闭文件 */
    fclose(fp);
    /*! 按Enter键退出 */
    getchar();

    return 0;
}

