#ifndef COMMON_WINDOWTOOL_H
#define COMMON_WINDOWTOOL_H
#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>

#define WM_USER_PLAYER	(WM_USER + 0x63)	// ������������

namespace QWindow{

class Tool
{
public:
	static HWND FindTopLevelWnd(LPCTSTR lpClassName,  LPCTSTR lpWindowName)
	{
		HWND hWnd = FindWindow(lpClassName, lpWindowName);
		if(NULL != lpClassName)
		{
			HWND hParent = NULL;
			while(hParent = GetParent(hWnd))
			{
				TCHAR szClass[MAX_PATH] = {0};
				GetClassName(hParent, szClass, MAX_PATH);
				if (_tcscmp(szClass, lpClassName) != 0)
				{
					hWnd = NULL;
					break;
				}
				hWnd = hParent;
			}
		}
		return hWnd;
	}
	static void SetForegroundWindow(HWND hWnd)
	{
		::ShowWindow(hWnd ,SW_SHOWNORMAL);
		if(::IsIconic(hWnd))
		{
			::SendMessage(hWnd, WM_SYSCOMMAND, (WPARAM)SC_RESTORE, 0);
		}
		::SetForegroundWindow(hWnd);
	}
	
	static BOOL FlashExplorerWnd(CString path)
	{
		path.Replace(L'/', L'\\');
		path = path.TrimRight(L'\\');
		CString title = path.Right(path.GetLength() - path.ReverseFind(L'\\') - 1);
		HWND hWnd = FindWindowExW(GetDesktopWindow(), NULL, L"CabinetWClass", title);
		CreateThread(0, 0, &QWindow::Tool::FlashProc, (LPVOID)hWnd, 0, 0);
		//FlashWindow(hWnd, TRUE);
		return TRUE;
	}

	static DWORD __stdcall FlashProc(LPVOID hWnd)
	{
		for (int i = 0;  i < 3; i++)
		{
			FlashWindow((HWND)hWnd, TRUE);
			Sleep(1000);
			FlashWindow((HWND)hWnd, FALSE);
		}
		return 0;
	}

	static HWND FindPlayerWnd(LPCTSTR lpStrClassName = NULL)
	{
		if (NULL == lpStrClassName)
		{
			lpStrClassName = _T("MangoTvWnd");
		}

		const TCHAR* lpClassName = lpStrClassName;
		HWND hPlayer = FindWindow(lpClassName, NULL);

		while (hPlayer)
		{
			TCHAR szClass[MAX_PATH] = {0};
			GetClassName(hPlayer, szClass, MAX_PATH);

			if (_tcscmp(szClass, lpClassName) == 0)
			{
				COPYDATASTRUCT copydata = {0};
				if (::SendMessage(hPlayer, WM_COPYDATA, 200, (LPARAM)&copydata))
				{
					return hPlayer;
				}
			}
			hPlayer = GetNextWindow(hPlayer, GW_HWNDNEXT);
		}

		//hPlayer = FindTopLevelWnd(lpClassName, NULL);							//û�ҵ�������֮ǰ�ķ�����ȡ
		
		return hPlayer;
	}

	static BOOL GetWorkArea(Gdiplus::Rect &rcWorkArea)
	{
		RECT rect;
		if(SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0))
		{
			rcWorkArea.X = rect.left;
			rcWorkArea.Y = rect.top;
			rcWorkArea.Width = rect.right - rect.left;
			rcWorkArea.Height = rect.bottom - rect.top;
			return TRUE;
		}
		return FALSE;
	}
	static BOOL GetDesktopRect(HWND hWnd, OUT Gdiplus::Rect &rcDesktop)
	{
		Gdiplus::Rect rcScreen;
		if(!GetScreenRect(hWnd, rcScreen))
		{
			return FALSE;
		}

		Gdiplus::Rect rcWorkArea;
		if(!GetWorkArea(rcWorkArea))
		{
			return FALSE;
		}
	
		Gdiplus::Point left_top(rcScreen.X, rcScreen.Y);
		Gdiplus::Point right_bottom(rcScreen.X + rcScreen.Width, rcScreen.Y + rcScreen.Height);
		
		Gdiplus::Point left_top1(rcWorkArea.X, rcWorkArea.Y);
		Gdiplus::Point right_bottom1(rcWorkArea.X + rcWorkArea.Width, rcWorkArea.Y + rcWorkArea.Height);
		if(left_top.Equals(left_top1) || right_bottom.Equals(right_bottom1))
		{
			rcDesktop = rcWorkArea;
		}
		else
		{
			rcDesktop = rcScreen;
		}
		return TRUE;
	}
	static BOOL GetScreenRect(HWND hWnd, OUT Gdiplus::Rect &rcScreen)
	{
		std::vector<Gdiplus::Rect> vMonitors;
		GetDisplayMonitors(vMonitors);
		if(vMonitors.empty())
		{
			return FALSE;
		}

		long index = -1;
		RECT rect;
		::GetWindowRect(hWnd, &rect);
		std::vector<long> vSquare;	// �������
		Gdiplus::Rect rectSrc(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
		for(long i=0; i<vMonitors.size(); i++)
		{
			Gdiplus::Rect rectDest = vMonitors[i];
			Gdiplus::Rect rectIntsc;
			if(Rect::Intersect(rectIntsc, rectDest, rectSrc))
			{
				vSquare.push_back(rectIntsc.Width*rectIntsc.Height);
			}
			else
			{
				vSquare.push_back(0);
			}
		}

		// �ĸ�������
		long nSquareMax = 0;
		for(long i=0; i<vSquare.size(); i++)
		{
			if(vSquare[i] > nSquareMax)
			{
				nSquareMax = vSquare[i];
				index = i;
			}
		}

		// ��û����Ĭ�ϵ�һ��Ļ
		if(index == -1) 
		{
			index = 0;
		}
		rcScreen = vMonitors[index];
		return TRUE;
	}

	static BOOL HasFullWindow()
	{
		BOOL bFull = FALSE;
		//	�д���ȫ��ʱ������
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);
		POINT pzPoint[] = {{1,1},{w-1,1}};
		HWND hWnd = WindowFromPoint(pzPoint[0]);
		if (hWnd != WindowFromPoint(pzPoint[1]))
		{
			return bFull;
		}
		CRect rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		if (rcWnd.Width() >= w && rcWnd.Height() >= h)
		{
			bFull = TRUE;
		}

		HWND hParent = GetParent(hWnd);
		while(hParent)
		{
			hWnd = hParent;
			hParent = GetParent(hWnd);
		}

		if (! bFull)
		{
			CRect rcWnd;
			GetWindowRect(hWnd, &rcWnd);
			if (rcWnd.Width() >= w && rcWnd.Height() >= h)
			{
				bFull = TRUE;
			}
		}
		if( bFull )
		{
			if( hWnd )
			{
				TCHAR szName[MAX_PATH];
				szName[0] = 0;
				if (  GetWindowText(hWnd,szName,MAX_PATH) )
				{
					if( 0 == _tcsicmp(szName,_T("Program Manager")) )
					{
						bFull = FALSE;
					}
				}
				if( bFull )
				{
					RECT rt;
					GetClientRect(hWnd, &rt);
					if( rt.bottom != h || rt.right != w )
					{
						bFull = FALSE;
					}
					if( bFull && GetClassName(hWnd,szName,MAX_PATH) )
					{
						if( 0 == _tcsicmp(szName,_T("workerw")))
						{
							bFull = FALSE;
						}
					}
				}
			}
		}

		//CString str;
		//str.Format(_T("HasFullWindow bFull=%d hWnd=%0x"), bFull, hWnd);
		//OutputDebugString(str);

		return bFull;
	}

	static void Sleeps(DWORD dwTick, BOOL & bExit)
	{
		int nCount = dwTick/20;
		for(int i=0; i < nCount && !bExit; i++)
		{
			Sleep(20);
		}
	}

	/*********************************************************************************
	Name		: ExeIsAdmin
	Description	: �жϵ�ǰexe�Ƿ���й���ԱȨ��
	Arguments	: void
	Return		: �����ǰexe���й���ԱȨ�ޣ��򷵻�TRUE�����򷵻�FALSE
	********************************************************************************/ 

	static BOOL ExeIsAdmin()
	{
#define ACCESS_READ  1
#define ACCESS_WRITE 2
#define LEAVEISADMIN\
	if (pACL) LocalFree(pACL);\
	if (psdAdmin) LocalFree(psdAdmin);\
	if (psidAdmin) FreeSid(psidAdmin);\
	return bReturn;

		HANDLE hToken;
		DWORD dwStatus;
		DWORD dwAccessMask;
		DWORD dwAccessDesired;
		DWORD dwACLSize;
		DWORD dwStructureSize = sizeof(PRIVILEGE_SET);
		PACL pACL = NULL;
		PSID psidAdmin = NULL;
		BOOL bReturn = FALSE;
		PRIVILEGE_SET ps;
		GENERIC_MAPPING GenericMapping;
		PSECURITY_DESCRIPTOR psdAdmin = NULL;
		SID_IDENTIFIER_AUTHORITY SystemSidAuthority = SECURITY_NT_AUTHORITY;

		if(!ImpersonateSelf(SecurityImpersonation))
		{
			LEAVEISADMIN
		}

		if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &hToken))
		{
			if (GetLastError() != ERROR_NO_TOKEN)
			{
				LEAVEISADMIN
			}

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			{
				LEAVEISADMIN
			}

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			{
				LEAVEISADMIN
			}
		}

		if (!AllocateAndInitializeSid(&SystemSidAuthority, 2,
			SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0, &psidAdmin))
		{
			LEAVEISADMIN
		}

		psdAdmin = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (psdAdmin == NULL)
		{
			LEAVEISADMIN
		}

		if (!InitializeSecurityDescriptor(psdAdmin,
			SECURITY_DESCRIPTOR_REVISION))
		{
			LEAVEISADMIN
		}

		dwACLSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) +
			GetLengthSid(psidAdmin) - sizeof(DWORD);

		pACL = (PACL)LocalAlloc(LPTR, dwACLSize);
		if (pACL == NULL)
		{
			LEAVEISADMIN
		}

		if (!InitializeAcl(pACL, dwACLSize, ACL_REVISION2))
		{
			LEAVEISADMIN
		}

		dwAccessMask= ACCESS_READ | ACCESS_WRITE;

		if (!AddAccessAllowedAce(pACL, ACL_REVISION2, dwAccessMask, psidAdmin))
		{
			LEAVEISADMIN
		}

		if (!SetSecurityDescriptorDacl(psdAdmin, TRUE, pACL, FALSE))
		{
			LEAVEISADMIN
		}

		if(!SetSecurityDescriptorGroup(psdAdmin, psidAdmin, FALSE))
		{
			LEAVEISADMIN
		}

		if(!SetSecurityDescriptorOwner(psdAdmin, psidAdmin, FALSE))
		{
			LEAVEISADMIN
		}

		if (!IsValidSecurityDescriptor(psdAdmin))
		{
			LEAVEISADMIN
		}

		dwAccessDesired = ACCESS_READ;

		GenericMapping.GenericRead = ACCESS_READ;
		GenericMapping.GenericWrite = ACCESS_WRITE;
		GenericMapping.GenericExecute = 0;
		GenericMapping.GenericAll = ACCESS_READ | ACCESS_WRITE;

		if (!AccessCheck(psdAdmin, hToken, dwAccessDesired,
			&GenericMapping, &ps, &dwStructureSize, &dwStatus, &bReturn))
		{
			LEAVEISADMIN
		}

		if( !RevertToSelf() )
			bReturn = FALSE;

		LEAVEISADMIN

	}

private:
	static void GetDisplayMonitors(OUT std::vector<Gdiplus::Rect> &vMonitors)
	{
		EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&vMonitors);
	}
	static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
	{
		std::vector<Gdiplus::Rect> *pvMonitors = (std::vector<Gdiplus::Rect>*)dwData;
		if(pvMonitors && lprcMonitor)
		{
			Gdiplus::Rect rcMonitor(lprcMonitor->left, lprcMonitor->top, 
				lprcMonitor->right - lprcMonitor->left,lprcMonitor->bottom - lprcMonitor->top);
			pvMonitors->push_back(rcMonitor);
			return TRUE;
		}
		return FALSE;
	}

};

} // namespace QWindow
#endif

