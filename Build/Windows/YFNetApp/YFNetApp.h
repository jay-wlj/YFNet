
// YFNetApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CYFNetAppApp:
// �йش����ʵ�֣������ YFNetApp.cpp
//

class CYFNetAppApp : public CWinApp
{
public:
	CYFNetAppApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CYFNetAppApp theApp;