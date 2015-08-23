
// Vct6VsDlg.cpp : ʵ���ļ�
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
/*! ����ʵ�� */
CSerialCom cDebugCom; /*!< ���Դ��� */
/*! ��ʾʱ���߳� */
HANDLE hDisplayTimeThread;
/*! ���õ��Դ���(DEBUG COM)�����߳���� */
HANDLE hSetDebugComParaThread;
HANDLE hSetDebugComParaEvent;
BOOL bSetDebugComParaFlag;
/*! ���Դ��ڽ����߳���� */
HANDLE hDebugComRecThread;
HANDLE hDebugComRecThreadEnd;
BOOL bDebugComRecThreadFlag;
DWORD ulDebugComRecCnt = 0; /*!< ���ռ��� */





/* Private functions ---------------------------------------------------------*/
/**
  * @file   DWORD WINAPI DebugComRecThread(LPVOID lpParam)
  * @brief  ���Դ��ڽ����߳�
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
            /*! ������ */
            if(cDebugCom.ComReadDate(cDebugCom.hCom, byGetData, dwGetDataLenght, &dwReadNumber, 0))
            {
                ulDebugComRecCnt += dwReadNumber; /*!< ���ռ��� */
                CString strcnt;
                strcnt.Format(_T("%ld"), ulDebugComRecCnt);
                pDlg->m_DebugComRecCnt.SetWindowText(strcnt);
                /*! �����յ�������д�뻺���� */
#if 0
                for(UCHAR i=0; i<dwReadNumber; i++)
                {
                    WriteBuf(byGetData[i]);
                }
#endif
                /*! ����16������ʾ */
                byGetData[dwReadNumber] = '\0';
#if 0
                UCHAR aucHexStr[256] = {'\0'};
                StringToHex(aucHexStr, byGetData, dwReadNumber);
                m_CStr = aucHexStr;
#endif
                m_CStr = byGetData;
                int len = pDlg->m_DisDebugComRec.GetWindowTextLength();
                pDlg->m_DisDebugComRec.SetSel(len, len); /*!< �������������� */
                pDlg->m_DisDebugComRec.ReplaceSel(m_CStr);
                pDlg->m_DisDebugComRec.ScrollWindow(0, 0);/*!< ����������� */
            }
        }
        Sleep(1);
    }
    /*! ���ս��̽��� */
    SetEvent(hDebugComRecThreadEnd);
    return TRUE;
}

/**
  * @file   DWORD WINAPI SetDebugComParaTThread(LPVOID lpParam)
  * @brief  ���Դ��ڽ����߳�
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
        if(str1 == _T("�򿪴���"))
        {
            /*! ���ô��ڲ��� */
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
                pDlg->m_OpenDebugCom.SetWindowText(_T("�رմ���"));
                pDlg->m_DebugComStatusBar.SetWindowText(_T(""));
                str2 = "���ڴ�: ";
                str2 += cDebugCom.CStrComName;
                str2 += ", ";
                str2 += CONFIG_DEBUG_BAUD;
                str2 += ", 8, 1, NONE";
                pDlg->m_DebugComStatusBar.SetWindowText(str2);
                /*! �������ڽ����߳� */
                /*! ��մ��ڻ����� */
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
                pDlg->m_OpenDebugCom.SetWindowText(_T("�򿪴���"));
                AfxMessageBox(_T("��ǰ�����Ѿ���ռ�ã�"));
            }
        }
        else
        {
            /*! �������ڽ����߳� */
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
            /*! �رմ��� */
            CloseHandle(cDebugCom.hCom);
            cDebugCom.hCom = INVALID_HANDLE_VALUE;
            pDlg->m_OpenDebugCom.SetWindowText(_T("�򿪴���"));
            pDlg->m_DebugComStatusBar.SetWindowText(_T(""));
            pDlg->m_DebugComStatusBar.SetWindowText(_T("����δ��!"));
        }
    } /*!< end while(bSetDebugComParaFlag) ... */
    
    return TRUE;
}

/**
  * @file   DWORD WINAPI DisplayTimeThread(LPVOID lpParam)
  * @brief  ��ʾʱ��
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
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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
// CVct6VsDlg �Ի���

CVct6VsDlg::CVct6VsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVct6VsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVct6VsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
    /*! ++++++++++++����++++++++++++++ */
    /*! ���Դ������ */
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


// CVct6VsDlg ��Ϣ��������

BOOL CVct6VsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MINIMIZE);

	// TODO: �ڴ����Ӷ���ĳ�ʼ������
    /**************************************************************************/
    /**************************************************************************/
    /*! ö�ٴ��� */
    int ComNum;
    CString CStrComName[256];
    if(TRUE == cDebugCom.EnumerationCom(CStrComName, ComNum))
    {
        for(int i=0; i<ComNum; i++)
        {
            m_DebugComName.InsertString(i, CStrComName[i].GetBuffer());
        }
        m_DebugComName.SetCurSel(0); /*!< �����򴮿��� */
    }
    else
    {
        AfxMessageBox(_T("�˵����޴��ڣ���ȷ�ϣ�"));
        // �رճ���
    }
    /*! ��ʼ���򿪴��ڰ�ť */
    m_OpenDebugCom.SetWindowText(_T("�򿪴���"));
    m_DebugComStatusBar.SetWindowText(_T("����δ��!"));
    /*! �������õ��Դ����߳� */
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
    /*! ��ʼ�����Դ��ڼ�����ʾ */
    m_DebugComRecCnt.SetWindowText(_T("0"));
    cDebugCom.hCom = INVALID_HANDLE_VALUE;

    /*! �����������Ϊ�ȿ����� */
    CWnd* pEdit;
    pEdit = GetDlgItem(IDC_EDIT1);
    HFONT hFont = (HFONT)::GetStockObject(SYSTEM_FIXED_FONT);
    CFont* pFont = CFont::FromHandle(hFont);
    pEdit->SetFont(pFont);

    /*! ������ʾʱ���߳� */
    hDisplayTimeThread = CreateThread( 
        NULL,                  // default security attributes
        0,                     // use default stack size 
        DisplayTimeThread,     // thread function 
        this,                  // argument to thread function 
        0,                     // use default creation flags 
        NULL                   // returns the thread identifier
        );


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CVct6VsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CVct6VsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/******************************************************************************/
/******************************************************************************/
/*! �򿪹رմ��ڰ�ť */
void CVct6VsDlg::OnBnClickedButton1()
{
    SetEvent(hSetDebugComParaEvent);
}

/*! ������������������ */
void CVct6VsDlg::OnBnClickedButton2()
{
    m_DisDebugComRec.SetWindowText(L"");
    m_DebugComRecCnt.SetWindowText(_T("0"));
    ulDebugComRecCnt = 0;
}