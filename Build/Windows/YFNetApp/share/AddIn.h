//AddIn.h
#ifndef _ADD_IN_H__
#define _ADD_IN_H__

#ifndef _ADD_IN_H___API_
#define _ADD_IN_H___API_

typedef void* (*pfnVisInit_)(void *, HWND, HWND *);
typedef void  (*pfnVisFree_)(void *);
typedef void  (*pfnVisFileName_)(void *, LPCTSTR , LPCTSTR );
typedef void 	(*pfnVisDraw_)(void *, BYTE *, long);
//typedef void 	(*pfnVisSetFormat_)(void *, WAVEFORMATEX *);
typedef void 	(*pfnVisCurTime_)(void *, LONGLONG);
typedef void 	(*pfnVisTotalTime_)(void *, LONGLONG);
typedef bool 	(*pfnVisGetProperty_)(void *, void *);
typedef bool 	(*pfnVisSetProperty_)(void *, void *);
#endif


class CAddIn
{
public:
	CAddIn();
	~CAddIn();
public:
	BOOL Init(LPCTSTR lpfilename);
	BOOL CreateAddIn(HWND hWnd);
	BOOL LoadFileName(LPCTSTR lpfilename, LPCTSTR lpothername);
	void SetCurTime(LONGLONG nCurTime);
	void SetDurTime(LONGLONG nDurTime);
	void MoveWindow(RECT *lpRect);
	void ShowWindow(int CmdShow);
	void SendCmdToAdd(WPARAM wParam, LPARAM lParam);
	BOOL Property(void *pStruct, BOOL bGetorSet);
private:
	pfnVisInit_						m_pfnVisInit_;
	pfnVisFree_						m_pfnVisFree_;
	pfnVisFileName_					m_pfnVisFileName_;
	pfnVisDraw_						m_pfnVisDraw_;
	pfnVisCurTime_					m_pfnVisCurTime_;
	pfnVisTotalTime_				m_pfnVisTotalTime_;
	pfnVisGetProperty_				m_pfnVisGetProperty_;
	pfnVisSetProperty_				m_pfnVisSetProperty_;
private:
	HMODULE	m_dllHandle;
	void	*m_pVoid;
	HWND	m_hWnd;
	BOOL	m_bShow;
};


#endif