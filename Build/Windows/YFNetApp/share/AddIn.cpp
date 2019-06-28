//AddIn.cpp
#include <windows.h>
#include <TCHAR.H>
#include "AddIn.h"

CAddIn::CAddIn()
{
	m_dllHandle = NULL;
	m_hWnd = NULL;
	Init(NULL);
	m_bShow = FALSE;
}

CAddIn::~CAddIn()
{
	Init(NULL);
}

BOOL CAddIn::Init(LPCTSTR lpfilename)
{
	if(m_dllHandle != NULL && m_pVoid != NULL && m_pfnVisFree_ != NULL)
	{
		m_pfnVisFree_(m_pVoid);
		::FreeLibrary(m_dllHandle);
	}
	m_pVoid     = NULL;
	m_dllHandle = NULL;
	m_pfnVisInit_ = NULL;
	m_pfnVisFree_ = NULL;
	m_pfnVisFileName_ = NULL;
	m_pfnVisDraw_ = NULL;
	m_pfnVisCurTime_ = NULL;
	m_pfnVisTotalTime_ = NULL;
	m_pfnVisGetProperty_ = NULL;
	m_pfnVisSetProperty_ = NULL;
	m_hWnd = NULL;
	if(lpfilename)
	{
		m_dllHandle = ::LoadLibrary(lpfilename);
		if(m_dllHandle)
		{
			m_pfnVisInit_ = (pfnVisInit_) GetProcAddress(m_dllHandle,				"VisInit_");
			m_pfnVisFree_ = (pfnVisFree_) GetProcAddress(m_dllHandle,				"VisFree_");
			m_pfnVisFileName_ = (pfnVisFileName_) GetProcAddress(m_dllHandle,		"VisFileName_");
			m_pfnVisDraw_ = (pfnVisDraw_) GetProcAddress(m_dllHandle,				"VisDraw_");
			m_pfnVisCurTime_ = (pfnVisCurTime_) GetProcAddress(m_dllHandle,			"VisCurTime_");
			m_pfnVisTotalTime_ = (pfnVisCurTime_) GetProcAddress(m_dllHandle,		"VisTotalTime_");
			m_pfnVisGetProperty_ = (pfnVisGetProperty_) GetProcAddress(m_dllHandle, "VisGetProperty_");
			m_pfnVisSetProperty_ = (pfnVisSetProperty_) GetProcAddress(m_dllHandle, "VisSetProperty_");	
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CAddIn::CreateAddIn(HWND hWnd)
{
	if(!m_dllHandle || !m_pfnVisInit_)
		return FALSE;
	m_pVoid = m_pfnVisInit_(m_pVoid, hWnd, &m_hWnd);
	if(!m_pVoid)
		return FALSE;
	if(!m_bShow)
		ShowWindow(SW_HIDE);
	return TRUE;
}

BOOL CAddIn::LoadFileName(LPCTSTR lpfilename, LPCTSTR lpothername)
{
	if(!m_dllHandle || !m_pfnVisFileName_ || !m_pVoid)
		return FALSE;
	m_pfnVisFileName_(m_pVoid, lpfilename, lpothername);
	return TRUE;
}

void CAddIn::SetCurTime(LONGLONG nCurTime)
{
	if(!m_dllHandle || !m_pfnVisCurTime_ || !m_pVoid)
		return ;
	m_pfnVisCurTime_(m_pVoid, nCurTime);
}

void CAddIn::SetDurTime(LONGLONG nDurTime)
{
	if(!m_dllHandle || !m_pfnVisTotalTime_ || !m_pVoid)
		return ;
	m_pfnVisTotalTime_(m_pVoid, nDurTime);
}

void CAddIn::MoveWindow(RECT *lpRect)
{
	if(m_hWnd)
	{
		::MoveWindow(m_hWnd, lpRect->left, lpRect->top, lpRect->right-lpRect->left, lpRect->bottom - lpRect->top, m_bShow);
	}
}

void CAddIn::ShowWindow(int CmdShow)
{
	if(m_hWnd)
	{
		m_bShow = (CmdShow == SW_SHOW) ? TRUE : FALSE;
		::ShowWindow(m_hWnd, CmdShow);
	}
}

void CAddIn::SendCmdToAdd(WPARAM wParam, LPARAM lParam)
{
	if(m_hWnd)
	{
		PostMessage(m_hWnd, WM_COMMAND, wParam, lParam);
	}	
}

BOOL CAddIn::Property(void *pStruct, BOOL bGetorSet)
{
	if(!m_dllHandle || !m_pfnVisGetProperty_ || !m_pfnVisSetProperty_ || !m_pVoid)
		return FALSE;
	if(bGetorSet)
		return m_pfnVisGetProperty_(m_pVoid, pStruct);
	else
		return m_pfnVisSetProperty_(m_pVoid, pStruct);
}