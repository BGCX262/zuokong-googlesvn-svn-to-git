
// Vct6VsDlg.h : ͷ�ļ�
//

#pragma once


// CVct6VsDlg �Ի���
class CVct6VsDlg : public CDialogEx
{
// ����
public:
	CVct6VsDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VCT6VS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
    CComboBox m_DebugComName; /*!< ���Դ����� */
    CButton m_OpenDebugCom; /*!< �򿪵��Դ��ڰ�ť */
    CEdit m_DisDebugComRec; /*!< ��ʾDEBUG COM���յ����� */
    CEdit m_DebugComStatusBar; /*!< ���Դ���״̬�� */
    CEdit m_DebugComRecCnt; /*!< ���Դ��ڽ��ռ��� */
    CEdit m_DisplayTime; /*!< ��ʾʱ�� */

    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
};