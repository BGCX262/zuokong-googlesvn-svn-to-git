
// Vct6VsDlg.h : 头文件
//

#pragma once


// CVct6VsDlg 对话框
class CVct6VsDlg : public CDialogEx
{
// 构造
public:
	CVct6VsDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VCT6VS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
    CComboBox m_DebugComName; /*!< 调试串口名 */
    CButton m_OpenDebugCom; /*!< 打开调试串口按钮 */
    CEdit m_DisDebugComRec; /*!< 显示DEBUG COM接收的数据 */
    CEdit m_DebugComStatusBar; /*!< 调试串口状态栏 */
    CEdit m_DebugComRecCnt; /*!< 调试串口接收计数 */
    CEdit m_DisplayTime; /*!< 显示时间 */

    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
};
