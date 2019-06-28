#include "stdafx.h"
#include "ToolTips.h"

CToolTips::CToolTips()
{
	m_bHasTips = FALSE;
	m_hTipWnd = NULL;
};

BOOL CToolTips::CreateToolTips(HWND hParent)
{
	BOOL bResult = FALSE;
	if( m_hTipWnd && ::IsWindow(m_hTipWnd) )
	{
		::DestroyWindow(m_hTipWnd);
	}
	m_hTipWnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hParent, NULL, _Module.m_hInst, NULL);
	::SetWindowPos(m_hTipWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	if( m_hTipWnd )
	{
		SendMessage(m_hTipWnd, TTM_SETMAXTIPWIDTH, 0, 512);
		SendMessage(m_hTipWnd, TTM_SETDELAYTIME, (WPARAM)TTDT_AUTOPOP, (LPARAM)MAKELONG(0x7FFF,0));
		bResult = TRUE;
		m_bHasTips = TRUE;
	}

	return bResult;
}

void CToolTips::AddTool(HWND hContainWnd,UINT uid,CString strTip,RECT *rcTip )
{
	if( strTip.IsEmpty() || IsRectEmpty(rcTip) || NULL == hContainWnd || !m_bHasTips)
	{
		return;
	}
	m_mapToolText[strTip] = uid;
	map<CString,int>::iterator itMap = m_mapToolText.find(strTip);
	memset(&m_toolInfo, 0, sizeof(TOOLINFO));
#if _WIN32_WINNT>0x0500
	m_toolInfo.cbSize = sizeof(TOOLINFO)-sizeof(void*);
#else
	m_toolInfo.cbSize = sizeof(TOOLINFO);
#endif

	m_toolInfo.uFlags = TTF_SUBCLASS;
	m_toolInfo.uId = uid;
	m_toolInfo.hwnd = hContainWnd;
	m_toolInfo.hinst = _Module.m_hInst;
	m_toolInfo.rect = *rcTip;
	m_toolInfo.lpszText = (LPTSTR)(LPCTSTR)itMap->first;
	SendMessage(m_hTipWnd, TTM_ADDTOOL, 0, (LPARAM)((LPTOOLINFO)&m_toolInfo));
	m_toolInfo.lpszText = NULL;
}

BOOL CToolTips::ActiveTool()
{
	if( m_hTipWnd && m_bHasTips)
	{
		SendMessage(m_hTipWnd,TTM_ACTIVATE,TRUE,NULL);
		return TRUE;
	}
	return FALSE;
}

void CToolTips::AlterTipText(HWND hContainWnd,UINT uid,CString strTip )
{
	if( strTip.IsEmpty() || NULL == hContainWnd || !m_bHasTips)
	{
		return;
	}
	m_mapToolText[strTip] = uid;
	map<CString,int>::iterator itMap = m_mapToolText.find(strTip);
#if _WIN32_WINNT>0x0500
	m_toolInfo.cbSize = sizeof(TOOLINFO)-sizeof(void*);
#else
	m_toolInfo.cbSize = sizeof(TOOLINFO);
#endif
	m_toolInfo.uId = uid;
	m_toolInfo.hwnd = hContainWnd;
	TCHAR szTipText[MAX_PATH];
	szTipText[0] = 0;
	m_toolInfo.lpszText = szTipText;
	LRESULT lResult = SendMessage(m_hTipWnd,TTM_GETTOOLINFO,0,(LPARAM) &m_toolInfo);
	if( FALSE == lResult )
	{
		return;
	}
	m_toolInfo.lpszText = (LPTSTR)(LPCTSTR)itMap->first;
	SendMessage(m_hTipWnd, TTM_SETTOOLINFO, 0, (LPARAM)((LPTOOLINFO)&m_toolInfo));
	m_toolInfo.lpszText = NULL;
}

void CToolTips::Clear()
{
	if( m_hTipWnd && ::IsWindow(m_hTipWnd) )
	{
		SendMessage(m_hTipWnd,TTM_ACTIVATE,FALSE,NULL);
		::DestroyWindow(m_hTipWnd);
	}
	m_bHasTips = FALSE;
	m_hTipWnd = NULL;
	memset(&m_toolInfo,0,sizeof(TOOLINFO));
	m_mapToolText.clear();
}