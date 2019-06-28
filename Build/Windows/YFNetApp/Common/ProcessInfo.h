#pragma once
#include <Tlhelp32.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")
#include "SystemInfo.h"

namespace common{
	class Process
	{
	public:
		static BOOL HasProcessInSnapShot(const CString& strProcess)		//½ø³ÌÃû
		{
			BOOL bRet = FALSE;
			EnableDebugPrivilege();
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			BOOL bFind = Process32First(hSnapshot, &pe);

			if(bFind)
			{
				do
				{
					CString strPro = pe.szExeFile;

					strPro = strPro.Right(strPro.GetLength() - strPro.ReverseFind(_T('\\')) - 1);
					if (strPro.CompareNoCase(strProcess) == 0)
					{
						bRet = TRUE;
						break;
					}

				}while (Process32Next(hSnapshot, &pe));	
			}
			else
			{
				OutputDebugString(L"Create snap failed");
			}

			if (hSnapshot)
			{
				CloseHandle(hSnapshot);
			}
			return bRet;
		}
		static BOOL IsRunAsAdmin(HANDLE h = NULL)
		{
			BOOL bIsElevated = TRUE;
			if(CSysteminfo::Instance().IsWin7())
			{
				bIsElevated = FALSE;
				HANDLE hToken = NULL;
				if (OpenProcessToken(h? h: GetCurrentProcess(), TOKEN_QUERY, &hToken))
				{
					TOKEN_ELEVATION te;  
					DWORD dwReturnLength = 0;  

					if (GetTokenInformation(hToken, (_TOKEN_INFORMATION_CLASS)TokenElevation, &te, sizeof(te), &dwReturnLength))
					{
						if (dwReturnLength == sizeof(te))
						{
							bIsElevated = te.TokenIsElevated;
						}
					}
					CloseHandle(hToken);
				}
			}
			return bIsElevated;
		}
	private:
		static BOOL EnableDebugPrivilege()
		{
			HANDLE   hToken;
			BOOL   fOk=FALSE;
			if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
			{
				TOKEN_PRIVILEGES   tp;
				tp.PrivilegeCount=1;
				if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid))
					return FALSE;
				tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
				if(!AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL))
					return FALSE;
				fOk=(GetLastError()==ERROR_SUCCESS);
				CloseHandle(hToken);
			}
			return   fOk;
		}
	};
}
