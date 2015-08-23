/*! \file  com.cpp
* \b �ӿ�˵����
*       
*
* \b ��������:
*
*       ����Դ�ļ�
*
* \b ��ʷ��¼:     
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2012-04-23   �����ļ�   <br>
*/

/******************************************************************************/
/*                             ͷ�ļ�                                          */
/******************************************************************************/
#include "stdafx.h"
#include "com.h"


/******************************************************************************/
/*                             �궨��                                          */
/******************************************************************************/
#define MAXBLOCK        1024 /*!< ���建������С */
#define XON             0x11
#define XOFF            0x13


/******************************************************************************/
/*                             ��������                                       */
/******************************************************************************/



/******************************************************************************/
/*                             �ӿ�ʵ��                                        */
/******************************************************************************/

/*! \fn       BOOL CSerialCom::EnumerationCom(CString *strSerialList, int &ComNum)
*  \brief     ö�ٴ��� //��ѯע���
*  \param     *strSerialList [in] 
*  \param     ComNum         [out] ���ں�
*  \exception ��
*  \return    TRUE or ����(-1)
*/
BOOL CSerialCom::EnumerationCom(CString *strSerialList, int &ComNum)
{
    HKEY hKey;
    LPCTSTR data_Set=L"HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
    long ret0 = (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey));
    if(ret0 != ERROR_SUCCESS)
    {
        return -1;
    }

    int i = 0;
    CHAR Name[50];
    UCHAR szPortName[25]={0};
    LONG Status;
    DWORD dwIndex = 0;
    DWORD dwName;
    DWORD dwSizeofPortName;
    DWORD Type;

    dwSizeofPortName = sizeof(szPortName);
    do
    {
        dwName = sizeof(Name);

        Status = RegEnumValue(hKey, dwIndex++, (LPWSTR)Name, &dwName, NULL, 
                              &Type, szPortName, &dwSizeofPortName);

        if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA)) 
        { 
            /*! �����ַ������� */
            strSerialList[i] = CString(szPortName) + CString(szPortName+2) + 
                               CString(szPortName+4) + CString(szPortName+6);
            /*! ���ڼ��� */
            i++;
        }
    } while((Status == ERROR_SUCCESS) || (Status == ERROR_MORE_DATA));

    ComNum = i;
    RegCloseKey(hKey);

    return TRUE;
}

/*! \fn       CSerialCom::ComInit
*  \brief     ��ʼ������
*  \param     m_hCom [out] ���ھ��
*  \param     ComName,BaudRate,Databit,Parity,Stopbit [in] ���ڲ���
*  \exception ��
*  \return    TRUE��ʼ���ɹ���FALSE��ʧ��
*/
BOOL CSerialCom::ComInit(HANDLE &m_hCom, CString ComName, CString BaudRate, 
                         CString Databit, CString Parity, CString Stopbit)
{
    COMMTIMEOUTS TimeOuts;          /*!< �������ʱ�� ��ʱ���� */
    DCB dcb;                        /*!< ��˿�ƥ����豸 */
    static CString StrComNameTemp;
    int i;

    if((INVALID_HANDLE_VALUE == m_hCom) || (StrComNameTemp != ComName))
    {
        if(INVALID_HANDLE_VALUE != m_hCom)
        {
            CloseHandle(m_hCom);
        }
        
        /*! ���ص���ʽ�򿪴��� */
        m_hCom = CreateFile(ComName.GetBuffer(), 
                            GENERIC_READ | GENERIC_WRITE, /*!< ������д */
                            0, 
                            NULL,
                            OPEN_EXISTING, /*!< �� */
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                            NULL);

        if(INVALID_HANDLE_VALUE == m_hCom)
        {
            /*! ��ʧ�ܣ�����5�� */
            for(i=0;i<5;i++)
            {
                Sleep(100); /*!< 100ms */
                m_hCom = CreateFile(ComName.GetBuffer(), 
                                    GENERIC_READ | GENERIC_WRITE,
                                    0, 
                                    NULL,
                                    OPEN_EXISTING, 
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                                    NULL);
                if(m_hCom != INVALID_HANDLE_VALUE) 
                {
                    break;
                }
            }
            if(5 != i)
            {
                /*! �ص���ʽ �첽ͨ�ţ�INVALID_HANDLE_VALUE������ʧ�� */
                AfxMessageBox(ComName + L"�ѱ�ռ�ã���ѡ���������ڣ�");
            }
            return FALSE;
        } /*!< end if(INVALID_HANDLE_VALUE... */

        /*! ���ô��пڵ����������������Ĵ�С */
        SetupComm(m_hCom, MAXBLOCK, MAXBLOCK);

        /*! �����豸��д�����ĳ�ʱʱ����� */
        memset(&TimeOuts, 0, sizeof(TimeOuts));
        /*! �Ѽ����ʱ��Ϊ��󣬰��ܳ�ʱ��Ϊ0������ReadFile�������ز���ɲ��� */
        TimeOuts.ReadIntervalTimeout = MAXDWORD;
        /*! ��ʱ��ϵ�� */
        TimeOuts.ReadTotalTimeoutMultiplier = 0;
        /*! ��ʱ�䳣�� */
        TimeOuts.ReadTotalTimeoutConstant = 0;
        /*! �ܳ�ʱ=ʱ��ϵ��*Ҫ���/д���ַ���+ʱ�䳣�� */
        TimeOuts.WriteTotalTimeoutMultiplier = 50;
        /*! ����д��ʱ��ָ��WriteComm��Ա�����е�GetOverlappedResult�����ĵȴ�ʱ�� */
        TimeOuts.WriteTotalTimeoutConstant = 2000;
        /*! ���ó�ʱ���� */
        if(!SetCommTimeouts(m_hCom, &TimeOuts))
        {
            /*! ����0��ʾ���� */
            AfxMessageBox(L"Set Com Time Failed!");
            return FALSE;
        }
    } 

    /*! ���COM�ڵ��豸���ƿ飬�Ӷ������ز��� */
    if(!GetCommState(m_hCom, &dcb))
    {
        AfxMessageBox(L"Get Com State Failed");
        return FALSE;
    }
    
    /*! ���ò��� */
    dcb.fBinary = TRUE;
    /*! ������żУ�� */
    dcb.fParity = TRUE;
    /*! ����У��λ */
    if(L"NONE" == Parity)
    {
        dcb.Parity = NOPARITY;
    }
    if(L"ODD" == Parity)
    {
        dcb.Parity = ODDPARITY;
    }
    if(L"EVEN" == Parity)
    {
        dcb.Parity = EVENPARITY;
    }
    /*! ����ֹͣλ */
    if(L"1" == Stopbit)
    {
        dcb.StopBits = ONESTOPBIT;
    }
    if(L"2" == Stopbit)
    {
        dcb.StopBits = TWOSTOPBITS;
    }
    //BOOL m_bEcho=FALSE;
    /*! ���ò����� */
    dcb.BaudRate=_wtol(BaudRate);
    /*! ����λ */
    dcb.ByteSize=(BYTE)_wtol(Databit);
    /*! Ӳ������������ */
    int m_nFlowCtrl = 0;
    dcb.fOutxCtsFlow=m_nFlowCtrl==1;
    dcb.fRtsControl=m_nFlowCtrl==1?RTS_CONTROL_HANDSHAKE:RTS_CONTROL_ENABLE;
    /*! XON/XOFF���������ã����������ƣ��� */
    dcb.fInX=dcb.fOutX=m_nFlowCtrl==2;
    dcb.XonChar=XON;
    dcb.XoffChar=XOFF;
    dcb.XonLim=50;
    dcb.XoffLim=50;

    /*! ����COM�ڵ��豸���ƿ� */
    if(SetCommState(m_hCom, &dcb))
    {
        return TRUE;
    }
    else
    {
        AfxMessageBox(L"�����Ѵ򿪣�����ʧ��");
        return FALSE;
    }
    /*! ��ջ����� */
    PurgeComm(m_hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
} 








/*! \fn       CSerialCom::ComWriteDate
*  \brief     �򴮿�д����
*  \param     hComm         [in] ���ھ��
*  \param     *pDataBuff    [in] ָ���跢�͵Ļ�����
*  \param     dwDataLenght  [in] �跢�ͳ���
*  \exception ��
*  \return    TRUE���ɹ���FALSE��ʧ��
*/
BOOL CSerialCom::ComWriteDate(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght)
{
    DWORD nBytesWrite, endtime, lrc;
    DWORD dwErrorMask, dwError;
    COMSTAT comstat;
    static OVERLAPPED m_osWrite;

    if (hComm == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(L"����δ�򿪣�");
        return FALSE;
    }

    m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    m_osWrite.Offset = 0;
    m_osWrite.OffsetHigh = 0;
    m_osWrite.hEvent = NULL; /*!< ��ʶ�¼������ݴ������ʱ��������Ϊ�ź�״̬ */
    ClearCommError(hComm, &dwErrorMask, &comstat);

    if(!WriteFile(hCom, pDataBuff, dwDataLenght, &nBytesWrite, &m_osWrite)) 
    {
        if(ERROR_IO_PENDING == (lrc=GetLastError()))
        {
            endtime=GetTickCount()+10000;
            while(!GetOverlappedResult(hCom,&m_osWrite,&nBytesWrite,FALSE))
            {
                dwError = GetLastError();
                if(GetTickCount()>endtime)
                {
                    AfxMessageBox(L"д����ʱ�������Ŀǰ���ڷ��ͻ������е�������ĿΪ��");
                    break;
                }
                if(ERROR_IO_INCOMPLETE == dwError)
                {
                    /*! δ��ȫ����ʱ���������ؽ�� */
                    continue;
                }
                else
                {
                    /*! �������󣬳��Իָ��� */
                    ClearCommError(hCom, &dwError, &comstat);
                    break;
                }
            }
        }
    }
    
    /*! ������������ */
    PurgeComm(hCom, PURGE_TXCLEAR);
    CloseHandle(m_osWrite.hEvent);
    return TRUE;
}

/*! \fn       CSerialCom::ComWriteDate1
*  \brief     �򴮿�д����
*  \param     hComm         [in] ���ھ��
*  \param     *pDataBuff    [in] ָ���跢�͵Ļ�����
*  \param     dwDataLenght  [in] �跢�ͳ���
*  \exception ��
*  \return    TRUE���ɹ���FALSE��ʧ��
*/
BOOL CSerialCom::ComWriteDate1(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght)
{
    DWORD nBytesWrite, endtime, lrc;
    DWORD dwErrorMask, dwError;
    COMSTAT comstat;
    static OVERLAPPED m_osWrite;

    if(hComm == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(L"����δ�򿪣�");
        return FALSE;
    }

    m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    m_osWrite.Offset = 0;
    m_osWrite.OffsetHigh = 0;
    m_osWrite.hEvent = NULL; /*!< ��ʶ�¼������ݴ������ʱ��������Ϊ�ź�״̬ */
    ClearCommError(hComm, &dwErrorMask, &comstat);

    if(!WriteFile(hCom, pDataBuff, dwDataLenght, &nBytesWrite, &m_osWrite)) 
    {
        if(ERROR_IO_PENDING == (lrc=GetLastError()))
        {
            endtime=GetTickCount()+10000;
            while(!GetOverlappedResult(hCom,&m_osWrite,&nBytesWrite,FALSE))
            {
                dwError = GetLastError();
                if(GetTickCount()>endtime)
                {
                    AfxMessageBox(L"д����ʱ�������Ŀǰ���ڷ��ͻ������е�������ĿΪ��");
                    break;
                }
                if(ERROR_IO_INCOMPLETE == dwError)
                {
                    /*! δ��ȫ����ʱ���������ؽ�� */
                    continue;
                }
                else
                {
                    /*! �������󣬳��Իָ��� */
                    ClearCommError(hCom, &dwError, &comstat);
                    break;
                }
            }
        }
    }
    
    /*! ������������ */
    PurgeComm(hCom, PURGE_TXCLEAR);
    CloseHandle(m_osWrite.hEvent);
    return TRUE;
}








/*! \fn       CSerialCom::ComInit
*  \brief     �Ӵ��ڶ�ȡ����
*  \param     hComm         [in]  ���ھ��
*  \param     *pDataBuff    [out] ָ����ջ�����
*  \param     dwDataLenght  [in]  ����ճ���
*  \param     *ReadNumber   [out] ��ȡ���ֽ���2012-07-24
*  \param     ReadTime      [in]  ��ȡʱ��
*  \exception ��
*  \return    TRUE���ɹ���FALSE��ʧ��
*/
BOOL CSerialCom::ComReadDate(HANDLE hComm, UCHAR *pDataBuff, 
                             DWORD dwDataLenght, DWORD *ReadNumber, DWORD ReadTime)
{
    DWORD  lrc;                 /*!< ��������У�� */
    DWORD  endtime;             /*!< ���� */
    static OVERLAPPED m_osRead;
    //DWORD  ReadNumber=0;        /*!< ���ƶ�ȡ����Ŀ */
    DWORD  dwErrorMask,nToRead;
    COMSTAT comstat;

    if (INVALID_HANDLE_VALUE == hComm)
    {
        AfxMessageBox(L"����δ�򿪣�");
        return FALSE;
    }
    
    /*! ����ļ���ʼ���ֽ�ƫ���� */
    m_osRead.Offset = 0;
    /*! ��ʼ�������ݵ��ֽ�ƫ�����ĸ�λ�֣��ܵ���ͨ��ʱ���ý��̿ɺ��� */
    m_osRead.OffsetHigh = 0;
    /*! ��ʶ�¼������ݴ������ʱ��Ϊ�ź�״̬ */
    m_osRead.hEvent = NULL;
    
    m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    /*! GetTickCount()ȡ��ϵͳ��ʼ�������õ�ʱ��(����) */
    endtime = GetTickCount() + ReadTime;
    Sleep(ReadTime);     
    ClearCommError(hCom, &dwErrorMask, &comstat);
    nToRead = min(2000, comstat.cbInQue);
    if(int(nToRead) < 1)
    {
        CloseHandle(m_osRead.hEvent);
        return FALSE;
    }

    if(!ReadFile(hComm, pDataBuff, dwDataLenght, ReadNumber, &m_osRead))
    {
        if(ERROR_IO_PENDING == (lrc=GetLastError()))
        {
            /*! GetTickCount()ȡ��ϵͳ��ʼ�������õ�ʱ��(����) */
            endtime = GetTickCount() + ReadTime;
            /*! �ú���ȡ���ص������Ľ�� */
            while(!GetOverlappedResult(hCom, &m_osRead, ReadNumber,FALSE))
            {
                if(GetTickCount() > endtime)
                    break;
            }
        }
    }

    CloseHandle(m_osRead.hEvent);
    return TRUE;
}

/*! \fn       CSerialCom::ComInit
*  \brief     �Ӵ��ڶ�ȡ����
*  \param     hComm         [in]  ���ھ��
*  \param     *pDataBuff    [out] ָ����ջ�����
*  \param     dwDataLenght  [in]  ����ճ���
*  \param     *ReadNumber   [out] ��ȡ���ֽ���2012-07-24
*  \param     ReadTime      [in]  ��ȡʱ��
*  \exception ��
*  \return    TRUE���ɹ���FALSE��ʧ��
*/
BOOL CSerialCom::ComReadDate1(HANDLE hComm, UCHAR *pDataBuff, 
                             DWORD dwDataLenght, DWORD *ReadNumber, DWORD ReadTime)
{
    DWORD  lrc;                 /*!< ��������У�� */
    DWORD  endtime;             /*!< ���� */
    static OVERLAPPED m_osRead;
    //DWORD  ReadNumber=0;        /*!< ���ƶ�ȡ����Ŀ */
    DWORD  dwErrorMask,nToRead;
    COMSTAT comstat;

    if (INVALID_HANDLE_VALUE == hComm)
    {
        AfxMessageBox(L"����δ�򿪣�");
        return FALSE;
    }
    
    /*! ����ļ���ʼ���ֽ�ƫ���� */
    m_osRead.Offset = 0;
    /*! ��ʼ�������ݵ��ֽ�ƫ�����ĸ�λ�֣��ܵ���ͨ��ʱ���ý��̿ɺ��� */
    m_osRead.OffsetHigh = 0;
    /*! ��ʶ�¼������ݴ������ʱ��Ϊ�ź�״̬ */
    m_osRead.hEvent = NULL;
    
    m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    /*! GetTickCount()ȡ��ϵͳ��ʼ�������õ�ʱ��(����) */
    endtime = GetTickCount() + ReadTime;
    Sleep(ReadTime);     
    ClearCommError(hCom, &dwErrorMask, &comstat);
    nToRead = min(2000, comstat.cbInQue);
    if(int(nToRead) < 1)
    {
        CloseHandle(m_osRead.hEvent);
        return FALSE;
    }

    if(!ReadFile(hComm, pDataBuff, dwDataLenght, ReadNumber, &m_osRead))
    {
        if(ERROR_IO_PENDING == (lrc=GetLastError()))
        {
            /*! GetTickCount()ȡ��ϵͳ��ʼ�������õ�ʱ��(����) */
            endtime = GetTickCount() + ReadTime;
            /*! �ú���ȡ���ص������Ľ�� */
            while(!GetOverlappedResult(hCom, &m_osRead, ReadNumber,FALSE))
            {
                if(GetTickCount() > endtime)
                    break;
            }
        }
    }

    CloseHandle(m_osRead.hEvent);
    return TRUE;
}
