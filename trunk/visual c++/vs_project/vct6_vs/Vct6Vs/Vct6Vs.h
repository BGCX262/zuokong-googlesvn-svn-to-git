
// Vct6Vs.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CVct6VsApp:
// �йش����ʵ�֣������ Vct6Vs.cpp
//

class CVct6VsApp : public CWinApp
{
public:
	CVct6VsApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CVct6VsApp theApp;