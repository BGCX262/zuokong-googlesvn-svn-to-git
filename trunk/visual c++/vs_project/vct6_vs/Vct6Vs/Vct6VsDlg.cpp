
// Vct6VsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Vct6Vs.h"
#include "Vct6VsDlg.h"
#include "afxdialogex.h"

#include "com.h"
#include "config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*! 定义实体 */
CSerialCom cDebugCom; /*!< 调试串口 */
/*! 显示时间线程 */
HANDLE hDisplayTimeThread;
/*! 设置调试串口(DEBUG COM)参数线程相关 */
HANDLE hSetDebugComParaThread;
HANDLE hSetDebugComParaEvent;
BOOL bSetDebugComParaFlag;
/*! 调试串口接收线程相关 */
HANDLE hDebugComRecThread;
HANDLE hDebugComRecThreadEnd;
BOOL bDebugComRecThreadFlag;
DWORD ulDebugComRecCnt = 0; /*!< 接收计数 */





/* Private functions ---------------------------------------------------------*/
/**
  * @file   DWORD WINAPI DebugComRecThread(LPVOID lpParam)
  * @brief  调试串口接收线程
  * @param  lpParam [in] 
  * @retval TRUE
  */
DWORD WINAPI DebugComRecThread(LPVOID lpParam)
{
    UCHAR byGetData[64];
    DWORD dwGetDataLenght = 32;
    DWORD dwReadNumber = 0;
    CString m_CStr = _T("");

    CVct6VsDlg *pDlg = (CVct6VsDlg *)lpParam;
    hDebugComRecThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
    bDebugComRecThreadFlag = TRUE;
    while(bDebugComRecThreadFlag)
    {
        if(cDebugCom.hCom != INVALID_HANDLE_VALUE)
        {
            /*! 读串口 */
            if(cDebugCom.ComReadDate(cDebugCom.hCom, byGetData, dwGetDataLenght, &dwReadNumber, 0))
            {
                ulDebugComRecCnt += dwReadNumber; /*!< 接收计数 */
                CString strcnt;
                strcnt.Format(_T("%ld"), ulDebugComRecCnt);
                pDlg->m_DebugComRecCnt.SetWindowText(strcnt);
                /*! 将接收到的数据写入缓冲区 */
#if 0
                for(UCHAR i=0; i<dwReadNumber; i++)
                {
                    WriteBuf(byGetData[i]);
                }
#endif
                /*! 串口16进制显示 */
                byGetData[dwReadNumber] = '\0';
#if 0
                UCHAR aucHexStr[256] = {'\0'};
                StringToHex(aucHexStr, byGetData, dwReadNumber);
                m_CStr = aucHexStr;
#endif
                m_CStr = byGetData;
                int len = pDlg->m_DisDebugComRec.GetWindowTextLength();
                pDlg->m_DisDebugComRec.SetSel(len, len); /*!< 将插入光标放在最后 */
                pDlg->m_DisDebugComRec.ReplaceSel(m_CStr);
                pDlg->m_DisDebugComRec.ScrollWindow(0, 0);/*!< 滚动到插入点 */
            }
        }
        Sleep(1);
    }
    /*! 接收进程结束 */
    SetEvent(hDebugComRecThreadEnd);
    return TRUE;
}

/**
  * @file   DWORD WINAPI SetDebugComParaTThread(LPVOID lpParam)
  * @brief  调试串口接收线程
  * @param  lpParam [in] 
  * @retval TRUE
  */
DWORD WINAPI SetDebugComParaTThread(LPVOID lpParam)
{
    BOOL bOpState;
    CString str1,str2;
    CVct6VsDlg *pDlg = (CVct6VsDlg *)lpParam;

    bSetDebugComParaFlag = TRUE;
    hSetDebugComParaEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    while(bSetDebugComParaFlag)
    {
        WaitForSingleObject(hSetDebugComParaEvent, INFINITE);
        pDlg->m_OpenDebugCom.GetWindowText(str1);
        if(str1 == _T("打开串口"))
        {
            /*! 设置窗口参数 */
            pDlg->m_DebugComName.GetWindowText(cDebugCom.CStrComName);
            cDebugCom.CStrBoundrate = _T(CONFIG_DEBUG_BAUD);
            cDebugCom.CStrDataBits = _T("8");
            cDebugCom.CStrStopBits = _T("1");
            cDebugCom.CStrParity = _T("NONE");
            bOpState = cDebugCom.ComInit(cDebugCom.hCom, cDebugCom.CStrComName,
                                    cDebugCom.CStrBoundrate, cDebugCom.CStrDataBits,
                                    cDebugCom.CStrStopBits, cDebugCom.CStrParity);
            if(TRUE == bOpState)
            {
                pDlg->m_OpenDebugCom.SetWindowText(_T("关闭串口"));
                pDlg->m_DebugComStatusBar.SetWindowText(_T(""));
                str2 = "串口打开: ";
                str2 += cDebugCom.CStrComName;
                str2 += ", ";
                str2 += CONFIG_DEBUG_BAUD;
                str2 += ", 8, 1, NONE";
                pDlg->m_DebugComStatusBar.SetWindowText(str2);
                /*! 创建串口接收线程 */
                /*! 清空串口缓冲区 */
                pDlg->m_DisDebugComRec.SetWindowText(L"");
                ulDebugComRecCnt = 0;
                PurgeComm(cDebugCom.hCom, PURGE_TXABORT|PURGE_RXABORT|
                                          PURGE_TXCLEAR|PURGE_RXCLEAR);
                hDebugComRecThread = CreateThread(
                                  NULL, // default security attributes
                                  0, // use default stack size 
                                  DebugComRecThread,// thread function 
                                  pDlg, // argument to thread function 
                                  0, // use default creation flags 
                                  NULL // returns the thread identifier
                                 ); 
                SetThreadPriority(hDebugComRecThread, THREAD_PRIORITY_NORMAL);
                ResumeThread(hDebugComRecThread);
            }
            else
            {
                pDlg->m_OpenDebugCom.SetWindowText(_T("打开串口"));
                AfxMessageBox(_T("当前串口已经被占用！"));
            }
        }
        else
        {
            /*! 结束串口接收线程 */
            if(hDebugComRecThread != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hDebugComRecThreadEnd);
                bDebugComRecThreadFlag = FALSE;
                if(WAIT_TIMEOUT == WaitForSingleObject(hDebugComRecThread, 2000))
                {
                    AfxMessageBox(_T("Kill hDebugComRecThread error!"));
                }
                if(INVALID_HANDLE_VALUE != hDebugComRecThread)
                {
                    CloseHandle(hDebugComRecThread);
                    hDebugComRecThread = INVALID_HANDLE_VALUE;
                }
            }
            hDebugComRecThreadEnd = INVALID_HANDLE_VALUE;
            hDebugComRecThread = INVALID_HANDLE_VALUE;
            /*! 关闭串口 */
            CloseHandle(cDebugCom.hCom);
            cDebugCom.hCom = INVALID_HANDLE_VALUE;
            pDlg->m_OpenDebugCom.SetWindowText(_T("打开串口"));
            pDlg->m_DebugComStatusBar.SetWindowText(_T(""));
            pDlg->m_DebugComStatusBar.SetWindowText(_T("串口未打开!"));
        }
    } /*!< end while(bSetDebugComParaFlag) ... */
    
    return TRUE;
}

/**
  * @file   DWORD WINAPI DisplayTimeThread(LPVOID lpParam)
  * @brief  显示时间
  * @param  lpParam [in] 
  * @retval TRUE
  */
DWORD WINAPI DisplayTimeThread(LPVOID lpParam)
{
    SYSTEMTIME st;
    CString strDate, strTime;
    CVct6VsDlg *pDlg = (CVct6VsDlg *)lpParam;

    while(TRUE)
    {
        GetLocalTime(&st);
        //strDate.Format(L"%4d-%02d-%02d", st.wYear,st.wMonth, st.wDay);
        strTime.Format(L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
        pDlg->m_DisplayTime.SetWindowText(strTime);
        Sleep(1000);
    }
}




/******************************************************************************/
/******************************************************************************/
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

/******************************************************************************/
/******************************************************************************/
// CVct6VsDlg 对话框

CVct6VsDlg::CVct6VsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVct6VsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVct6VsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
    /*! ++++++++++++添加++++++++++++++ */
    /*! 调试串口相关 */
    DDX_Control(pDX, IDC_COMBO1, m_DebugComName);
    DDX_Control(pDX, IDC_BUTTON1, m_OpenDebugCom);
    DDX_Control(pDX, IDC_EDIT1, m_DisDebugComRec);
    DDX_Control(pDX, IDC_EDIT2, m_DebugComStatusBar);
    DDX_Control(pDX, IDC_EDIT3, m_DebugComRecCnt);
    DDX_Control(pDX, IDC_EDIT4, m_DisplayTime);
}

BEGIN_MESSAGE_MAP(CVct6VsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CVct6VsDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CVct6VsDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CVct6VsDlg 消息处理程序

BOOL CVct6VsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
    /**************************************************************************/
    /**************************************************************************/
    /*! 枚举串口 */
    int ComNum;
    CString CStrComName[256];
    if(TRUE == cDebugCom.EnumerationCom(CStrComName, ComNum))
    {
        for(int i=0; i<ComNum; i++)
        {
            m_DebugComName.InsertString(i, CStrComName[i].GetBuffer());
        }
        m_DebugComName.SetCurSel(0); /*!< 下拉框串口名 */
    }
    else
    {
        AfxMessageBox(_T("此电脑无串口，请确认！"));
        // 关闭程序
    }
    /*! 初始化打开串口按钮 */
    m_OpenDebugCom.SetWindowText(_T("打开串口"));
    m_DebugComStatusBar.SetWindowText(_T("串口未打开!"));
    /*! 创建设置调试串口线程 */
    hSetDebugComParaThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size 
        SetDebugComParaTThread, // thread function 
        this,                   // argument to thread function 
        CREATE_SUSPENDED,       // use default creation flags 
        NULL                    // returns the thread identifier
        ); 
    SetThreadPriority(hSetDebugComParaThread, THREAD_PRIORITY_NORMAL);
    ResumeThread(hSetDebugComParaThread);
    /*! 初始化调试串口计数显示 */
    m_DebugComRecCnt.SetWindowText(_T("0"));
    cDebugCom.hCom = INVALID_HANDLE_VALUE;

    /*! 设置输出窗口为等宽字体 */
    CWnd* pEdit;
    pEdit = GetDlgItem(IDC_EDIT1);
    HFONT hFont = (HFONT)::GetStockObject(SYSTEM_FIXED_FONT);
    CFont* pFont = CFont::FromHandle(hFont);
    pEdit->SetFont(pFont);

    /*! 创建显示时间线程 */
    hDisplayTimeThread = CreateThread( 
        NULL,                  // default security attributes
        0,                     // use default stack size 
        DisplayTimeThread,     // thread function 
        this,                  // argument to thread function 
        0,                     // use default creation flags 
        NULL                   // returns the thread identifier
        );


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVct6VsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVct6VsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVct6VsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/******************************************************************************/
/******************************************************************************/
/*! 打开关闭串口按钮 */
void CVct6VsDlg::OnBnClickedButton1()
{
    SetEvent(hSetDebugComParaEvent);
}

/*! 清除调试输出窗口内容 */
void CVct6VsDlg::OnBnClickedButton2()
{
    m_DisDebugComRec.SetWindowText(L"");
    m_DebugComRecCnt.SetWindowText(_T("0"));
    ulDebugComRecCnt = 0;
}
