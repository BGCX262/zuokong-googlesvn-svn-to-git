/*! \file  com.cpp
* \b 接口说明：
*       
*
* \b 功能描述:
*
*       串口源文件
*
* \b 历史记录:     
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2012-04-23   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                          */
/******************************************************************************/
#include "stdafx.h"
#include "com.h"


/******************************************************************************/
/*                             宏定义                                          */
/******************************************************************************/
#define MAXBLOCK        1024 /*!< 定义缓冲区大小 */
#define XON             0x11
#define XOFF            0x13


/******************************************************************************/
/*                             变量定义                                       */
/******************************************************************************/



/******************************************************************************/
/*                             接口实现                                        */
/******************************************************************************/

/*! \fn       BOOL CSerialCom::EnumerationCom(CString *strSerialList, int &ComNum)
*  \brief     枚举串口 //查询注册表
*  \param     *strSerialList [in] 
*  \param     ComNum         [out] 串口号
*  \exception 无
*  \return    TRUE or 错误(-1)
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
            /*! 串口字符串保存 */
            strSerialList[i] = CString(szPortName) + CString(szPortName+2) + 
                               CString(szPortName+4) + CString(szPortName+6);
            /*! 串口计数 */
            i++;
        }
    } while((Status == ERROR_SUCCESS) || (Status == ERROR_MORE_DATA));

    ComNum = i;
    RegCloseKey(hKey);

    return TRUE;
}

/*! \fn       CSerialCom::ComInit
*  \brief     初始化串口
*  \param     m_hCom [out] 串口句柄
*  \param     ComName,BaudRate,Databit,Parity,Stopbit [in] 串口参数
*  \exception 无
*  \return    TRUE：始化成功；FALSE：失败
*/
BOOL CSerialCom::ComInit(HANDLE &m_hCom, CString ComName, CString BaudRate, 
                         CString Databit, CString Parity, CString Stopbit)
{
    COMMTIMEOUTS TimeOuts;          /*!< 串口输出时间 超时设置 */
    DCB dcb;                        /*!< 与端口匹配的设备 */
    static CString StrComNameTemp;
    int i;

    if((INVALID_HANDLE_VALUE == m_hCom) || (StrComNameTemp != ComName))
    {
        if(INVALID_HANDLE_VALUE != m_hCom)
        {
            CloseHandle(m_hCom);
        }
        
        /*! 以重叠方式打开串口 */
        m_hCom = CreateFile(ComName.GetBuffer(), 
                            GENERIC_READ | GENERIC_WRITE, /*!< 允许读写 */
                            0, 
                            NULL,
                            OPEN_EXISTING, /*!< 打开 */
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                            NULL);

        if(INVALID_HANDLE_VALUE == m_hCom)
        {
            /*! 打开失败，重试5次 */
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
                /*! 重叠方式 异步通信（INVALID_HANDLE_VALUE）函数失败 */
                AfxMessageBox(ComName + L"已被占用！请选择其他串口！");
            }
            return FALSE;
        } /*!< end if(INVALID_HANDLE_VALUE... */

        /*! 设置串行口的输入和输出缓冲区的大小 */
        SetupComm(m_hCom, MAXBLOCK, MAXBLOCK);

        /*! 设置设备读写操作的超时时间参数 */
        memset(&TimeOuts, 0, sizeof(TimeOuts));
        /*! 把间隔超时设为最大，把总超时设为0将导致ReadFile立即返回并完成操作 */
        TimeOuts.ReadIntervalTimeout = MAXDWORD;
        /*! 读时间系数 */
        TimeOuts.ReadTotalTimeoutMultiplier = 0;
        /*! 读时间常量 */
        TimeOuts.ReadTotalTimeoutConstant = 0;
        /*! 总超时=时间系数*要求读/写的字符数+时间常量 */
        TimeOuts.WriteTotalTimeoutMultiplier = 50;
        /*! 设置写超时以指定WriteComm成员函数中的GetOverlappedResult函数的等待时间 */
        TimeOuts.WriteTotalTimeoutConstant = 2000;
        /*! 设置超时参数 */
        if(!SetCommTimeouts(m_hCom, &TimeOuts))
        {
            /*! 返回0表示错误 */
            AfxMessageBox(L"Set Com Time Failed!");
            return FALSE;
        }
    } 

    /*! 获得COM口的设备控制块，从而获得相关参数 */
    if(!GetCommState(m_hCom, &dcb))
    {
        AfxMessageBox(L"Get Com State Failed");
        return FALSE;
    }
    
    /*! 设置参数 */
    dcb.fBinary = TRUE;
    /*! 允许奇偶校验 */
    dcb.fParity = TRUE;
    /*! 设置校验位 */
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
    /*! 设置停止位 */
    if(L"1" == Stopbit)
    {
        dcb.StopBits = ONESTOPBIT;
    }
    if(L"2" == Stopbit)
    {
        dcb.StopBits = TWOSTOPBITS;
    }
    //BOOL m_bEcho=FALSE;
    /*! 设置波特率 */
    dcb.BaudRate=_wtol(BaudRate);
    /*! 数据位 */
    dcb.ByteSize=(BYTE)_wtol(Databit);
    /*! 硬件流控制设置 */
    int m_nFlowCtrl = 0;
    dcb.fOutxCtsFlow=m_nFlowCtrl==1;
    dcb.fRtsControl=m_nFlowCtrl==1?RTS_CONTROL_HANDSHAKE:RTS_CONTROL_ENABLE;
    /*! XON/XOFF流控制设置（软件流控制！） */
    dcb.fInX=dcb.fOutX=m_nFlowCtrl==2;
    dcb.XonChar=XON;
    dcb.XoffChar=XOFF;
    dcb.XonLim=50;
    dcb.XoffLim=50;

    /*! 设置COM口的设备控制块 */
    if(SetCommState(m_hCom, &dcb))
    {
        return TRUE;
    }
    else
    {
        AfxMessageBox(L"串口已打开，设置失败");
        return FALSE;
    }
    /*! 清空缓冲区 */
    PurgeComm(m_hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
} 








/*! \fn       CSerialCom::ComWriteDate
*  \brief     向串口写数据
*  \param     hComm         [in] 串口句柄
*  \param     *pDataBuff    [in] 指向需发送的缓冲区
*  \param     dwDataLenght  [in] 需发送长度
*  \exception 无
*  \return    TRUE：成功；FALSE：失败
*/
BOOL CSerialCom::ComWriteDate(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght)
{
    DWORD nBytesWrite, endtime, lrc;
    DWORD dwErrorMask, dwError;
    COMSTAT comstat;
    static OVERLAPPED m_osWrite;

    if (hComm == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(L"串口未打开！");
        return FALSE;
    }

    m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    m_osWrite.Offset = 0;
    m_osWrite.OffsetHigh = 0;
    m_osWrite.hEvent = NULL; /*!< 标识事件，数据传送完成时，将它设为信号状态 */
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
                    AfxMessageBox(L"写串口时间过长，目前串口发送缓冲区中的数据数目为空");
                    break;
                }
                if(ERROR_IO_INCOMPLETE == dwError)
                {
                    /*! 未完全读完时的正常返回结果 */
                    continue;
                }
                else
                {
                    /*! 发生错误，尝试恢复！ */
                    ClearCommError(hCom, &dwError, &comstat);
                    break;
                }
            }
        }
    }
    
    /*! 清空输出缓冲区 */
    PurgeComm(hCom, PURGE_TXCLEAR);
    CloseHandle(m_osWrite.hEvent);
    return TRUE;
}

/*! \fn       CSerialCom::ComWriteDate1
*  \brief     向串口写数据
*  \param     hComm         [in] 串口句柄
*  \param     *pDataBuff    [in] 指向需发送的缓冲区
*  \param     dwDataLenght  [in] 需发送长度
*  \exception 无
*  \return    TRUE：成功；FALSE：失败
*/
BOOL CSerialCom::ComWriteDate1(HANDLE hComm, UCHAR *pDataBuff, DWORD dwDataLenght)
{
    DWORD nBytesWrite, endtime, lrc;
    DWORD dwErrorMask, dwError;
    COMSTAT comstat;
    static OVERLAPPED m_osWrite;

    if(hComm == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(L"串口未打开！");
        return FALSE;
    }

    m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    m_osWrite.Offset = 0;
    m_osWrite.OffsetHigh = 0;
    m_osWrite.hEvent = NULL; /*!< 标识事件，数据传送完成时，将它设为信号状态 */
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
                    AfxMessageBox(L"写串口时间过长，目前串口发送缓冲区中的数据数目为空");
                    break;
                }
                if(ERROR_IO_INCOMPLETE == dwError)
                {
                    /*! 未完全读完时的正常返回结果 */
                    continue;
                }
                else
                {
                    /*! 发生错误，尝试恢复！ */
                    ClearCommError(hCom, &dwError, &comstat);
                    break;
                }
            }
        }
    }
    
    /*! 清空输出缓冲区 */
    PurgeComm(hCom, PURGE_TXCLEAR);
    CloseHandle(m_osWrite.hEvent);
    return TRUE;
}








/*! \fn       CSerialCom::ComInit
*  \brief     从串口读取数据
*  \param     hComm         [in]  串口句柄
*  \param     *pDataBuff    [out] 指向接收缓冲区
*  \param     dwDataLenght  [in]  需接收长度
*  \param     *ReadNumber   [out] 读取的字节数2012-07-24
*  \param     ReadTime      [in]  读取时间
*  \exception 无
*  \return    TRUE：成功；FALSE：失败
*/
BOOL CSerialCom::ComReadDate(HANDLE hComm, UCHAR *pDataBuff, 
                             DWORD dwDataLenght, DWORD *ReadNumber, DWORD ReadTime)
{
    DWORD  lrc;                 /*!< 纵向冗余校验 */
    DWORD  endtime;             /*!< 接收 */
    static OVERLAPPED m_osRead;
    //DWORD  ReadNumber=0;        /*!< 控制读取的数目 */
    DWORD  dwErrorMask,nToRead;
    COMSTAT comstat;

    if (INVALID_HANDLE_VALUE == hComm)
    {
        AfxMessageBox(L"串口未打开！");
        return FALSE;
    }
    
    /*! 相对文件开始的字节偏移量 */
    m_osRead.Offset = 0;
    /*! 开始传送数据的字节偏移量的高位字，管道和通信时调用进程可忽略 */
    m_osRead.OffsetHigh = 0;
    /*! 标识事件，数据传送完成时设为信号状态 */
    m_osRead.hEvent = NULL;
    
    m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    /*! GetTickCount()取回系统开始至此所用的时间(毫秒) */
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
            /*! GetTickCount()取回系统开始至此所用的时间(毫秒) */
            endtime = GetTickCount() + ReadTime;
            /*! 该函数取回重叠操作的结果 */
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
*  \brief     从串口读取数据
*  \param     hComm         [in]  串口句柄
*  \param     *pDataBuff    [out] 指向接收缓冲区
*  \param     dwDataLenght  [in]  需接收长度
*  \param     *ReadNumber   [out] 读取的字节数2012-07-24
*  \param     ReadTime      [in]  读取时间
*  \exception 无
*  \return    TRUE：成功；FALSE：失败
*/
BOOL CSerialCom::ComReadDate1(HANDLE hComm, UCHAR *pDataBuff, 
                             DWORD dwDataLenght, DWORD *ReadNumber, DWORD ReadTime)
{
    DWORD  lrc;                 /*!< 纵向冗余校验 */
    DWORD  endtime;             /*!< 接收 */
    static OVERLAPPED m_osRead;
    //DWORD  ReadNumber=0;        /*!< 控制读取的数目 */
    DWORD  dwErrorMask,nToRead;
    COMSTAT comstat;

    if (INVALID_HANDLE_VALUE == hComm)
    {
        AfxMessageBox(L"串口未打开！");
        return FALSE;
    }
    
    /*! 相对文件开始的字节偏移量 */
    m_osRead.Offset = 0;
    /*! 开始传送数据的字节偏移量的高位字，管道和通信时调用进程可忽略 */
    m_osRead.OffsetHigh = 0;
    /*! 标识事件，数据传送完成时设为信号状态 */
    m_osRead.hEvent = NULL;
    
    m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    /*! GetTickCount()取回系统开始至此所用的时间(毫秒) */
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
            /*! GetTickCount()取回系统开始至此所用的时间(毫秒) */
            endtime = GetTickCount() + ReadTime;
            /*! 该函数取回重叠操作的结果 */
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

