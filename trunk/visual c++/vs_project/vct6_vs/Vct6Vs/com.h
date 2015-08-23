/*! \file  comm.h
* \b �ӿ�˵����  
*
*
* \b ��������:
*
*       ����ͷ�ļ�
*
* \b ��ʷ��¼:
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2012-04-23   �����ļ�   <br>
*/
#ifndef __COM_H_
#define __COM_H_

/******************************************************************************/
/*                            ͷ�ļ�                                           */
/******************************************************************************/


/******************************************************************************/
/*                           �ඨ��                                            */
/******************************************************************************/
class CSerialCom
{
    public:
        CString strSerialList[256]; /*!< ��ʱ����256���ַ����飬��Ϊϵͳ���Ҳ��256�� */
        CString CStrComName; /*!< �������� */
        CString CStrBoundrate; /*!< ������ */
        CString CStrStopBits; /*!< ֹͣλ */
        CString CStrDataBits; /*!< ����λ */
        CString CStrParity; /*!< ��żУ�� */
        HANDLE  hCom; /*!< ���ھ�� */

    public:
        int EnumerationCom(CString *strSerialList, int &ComNum);
        BOOL ComInit(HANDLE &m_hCom, CString ComName, CString BaudRate, CString Databits, CString Parity, CString Stopbits);

        BOOL ComWriteDate(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght);
        BOOL ComWriteDate1(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght);

        BOOL ComReadDate(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght,DWORD *ReadNumber, DWORD ReadTime);
        BOOL ComReadDate1(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght,DWORD *ReadNumber, DWORD ReadTime);
};

#endif