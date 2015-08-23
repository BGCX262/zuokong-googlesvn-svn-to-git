/*! \file  comm.h
* \b 接口说明：  
*
*
* \b 功能描述:
*
*       串口头文件
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2012-04-23   创建文件   <br>
*/
#ifndef __COM_H_
#define __COM_H_

/******************************************************************************/
/*                            头文件                                           */
/******************************************************************************/


/******************************************************************************/
/*                           类定义                                            */
/******************************************************************************/
class CSerialCom
{
    public:
        CString strSerialList[256]; /*!< 临时定义256个字符串组，因为系统最多也就256个 */
        CString CStrComName; /*!< 串口名称 */
        CString CStrBoundrate; /*!< 波特率 */
        CString CStrStopBits; /*!< 停止位 */
        CString CStrDataBits; /*!< 数据位 */
        CString CStrParity; /*!< 奇偶校验 */
        HANDLE  hCom; /*!< 串口句柄 */

    public:
        int EnumerationCom(CString *strSerialList, int &ComNum);
        BOOL ComInit(HANDLE &m_hCom, CString ComName, CString BaudRate, CString Databits, CString Parity, CString Stopbits);

        BOOL ComWriteDate(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght);
        BOOL ComWriteDate1(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght);

        BOOL ComReadDate(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght,DWORD *ReadNumber, DWORD ReadTime);
        BOOL ComReadDate1(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght,DWORD *ReadNumber, DWORD ReadTime);
};

#endif