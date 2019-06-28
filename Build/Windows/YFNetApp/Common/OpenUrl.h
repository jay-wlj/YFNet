// OpenURL.h file
//
// Author: 关玮
// Date:   2012-07-12
//
// Description: 
//         OpenURL的目的是取代ShellExecute，避免被杀软拦截，用默认浏览器打开，当注册表损坏时用IE打开
//         OpenURL(strURL);
//         指定用IE打开  OpenURL(strURL, TRUE);
//         取代 ShellExecute(NULL,_T("open"),strURL,NULL,NULL,SW_SHOW);
//
// 参考: http://support.microsoft.com/kb/224816
//       http://support.microsoft.com/kb/330337
//       http://support.microsoft.com/kb/160957
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <ddeml.h>
#include "Thread.h"

typedef struct _SHELL_REGDATA
{
	BOOL bDDE;
	CString strPath;
	CString strParam;
	CString strDDEParam;
	CString strDDEAppName;
	CString strDDETopic; // 谓词
	_SHELL_REGDATA()
	{
		bDDE = FALSE;
	}
}SHELL_REGDATA;

namespace common{

class OpenUrl : public Thread::Delegate
{
public:
	static OpenUrl& Instance()
	{
		static OpenUrl inst;
		return inst;
	}
	void OpenDefault(CString url)
	{
		m_bUseIE = FALSE;
		Open(url);
	}
	void OpenIE(CString url)
	{
		m_bUseIE = TRUE;
		Open(url);
	}
private:
	OpenUrl():m_thread(this)
	{

	}
	~OpenUrl()
	{
		m_thread.WaitThreadExit();
	}
	void Open(const CString& url)
	{
		if(url.IsEmpty() && m_thread.IsRunning())
		{
			return;
		}
		m_url = url;
		m_thread.Run();
	}
	void OnThread(Thread *pThread)
	{
		CString str;
		BOOL isOK = FALSE;
		SHELL_REGDATA shellData;
		if(m_bUseIE)
		{
			SetIEPath(&shellData);
		}
		else if(!GetDefaultBrowser(&shellData))
		{
			SetIEPath(&shellData);
		}

		// 发送DDE数据
		if(shellData.bDDE)
		{
			isOK = DDE_Exec(&shellData, m_url);
		}
		else // 按设定参数启动进程
		{
			PROCESS_INFORMATION pInfo = {0};
			STARTUPINFO sInfo = {0};
			sInfo.cb = sizeof(STARTUPINFO);
			shellData.strParam.Replace(_T("%1"), m_url);
			str.Format(_T("\"%s\" %s"), shellData.strPath, shellData.strParam);
			isOK = CreateProcess(NULL, (TCHAR*)(LPCTSTR)str, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP|CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &sInfo, &pInfo);
		}

		// 都不成功，用IE打开
		if(!isOK)
		{
			BOOL bIE = FALSE;
			str = shellData.strPath;
			str.MakeLower();
			if(str.Find(_T("iexplore.exe")) > 0)
			{
				bIE = TRUE;
			}
			if(!bIE)
			{
				SetIEPath(&shellData);
				isOK = DDE_Exec(&shellData, m_url);
			}
		}
	}
private:
	Thread m_thread;
	CString m_url;
	BOOL m_bUseIE;

private:
	// 查找注册表中默认浏览器的打开键
	static HKEY GetDefaultHttpKey()
	{
		CString strChoice;
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
		if(osvi.dwMajorVersion >= 6)
		{
			// win7
			HKEY hKey = NULL;
			RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"), 0, KEY_QUERY_VALUE, &hKey);
			if(hKey)
			{
				TCHAR szShell[1024] = {0};
				DWORD dwShellSize = 1024*sizeof(TCHAR);
				RegQueryValueEx(hKey, _T("Progid"), NULL, NULL, (LPBYTE)szShell, &dwShellSize);
				RegCloseKey(hKey);
				strChoice = szShell;
			}
		}
		if(strChoice.GetLength() == 0)
		{
			// xp
			strChoice = _T("http");
		}

		BOOL bRet = FALSE;
		HKEY hKey = NULL;
		// read command data
		CString strReg;
		strReg.Format(_T("%s\\shell"), strChoice);
		RegOpenKeyEx(HKEY_CLASSES_ROOT, strReg, 0, KEY_QUERY_VALUE, &hKey);
		if(hKey)
		{
			TCHAR szShell[1024] = {0};
			DWORD dwShellSize = 1024*sizeof(TCHAR);
			RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE)szShell, &dwShellSize);
			RegCloseKey(hKey);
			hKey = NULL;
			if(szShell[0] == 0)
			{
				lstrcpy(szShell, _T("open"));
			}
			strReg.Format(_T("%s\\shell\\%s"), strChoice, szShell);
			RegOpenKeyEx(HKEY_CLASSES_ROOT, strReg, 0, KEY_QUERY_VALUE, &hKey);
		}
		return hKey;
	}

	// 获取默认浏览器和启动参数 资讯快播
	static BOOL GetDefaultBrowser(SHELL_REGDATA *pShellData)
	{
		if(pShellData == NULL)
		{
			return FALSE;
		}

		HKEY hKeyOpen = GetDefaultHttpKey();
		if(hKeyOpen == NULL)
		{
			return FALSE;
		}

		TCHAR szValue[1024] = {0};
		DWORD dwSize = 1024*sizeof(TCHAR);
		HKEY hKey = NULL;
		RegOpenKeyEx(hKeyOpen, _T("command"), 0, KEY_QUERY_VALUE, &hKey);
		if(hKey)
		{
			RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE)szValue, &dwSize);
			RegCloseKey(hKey);
		}

		// 解析脚本
		if(szValue[0])
		{
			if(szValue[0] == '"')
			{
				TCHAR *pStart = szValue + 1;
				if(*pStart)
				{
					TCHAR *pEnd = _tcschr(pStart, '"');
					if(pEnd)
					{
						*pEnd = 0;
						pShellData->strPath = pStart;
						pShellData->strPath.Trim();
						pShellData->strParam = pEnd + 1;
						pShellData->strParam.Trim();
					}
				}
			}
			else
			{
				CString sPath,sParam;
				sPath=szValue;
				sParam=sPath;
				sPath.MakeLower();
				int n=sPath.Find(_T(".exe"));//返回.exe子串的位置
				if(n!=-1)
				{
					pShellData->strPath=(sPath.Left(n+4));
					pShellData->strParam=(sParam.Mid(n+5));
					pShellData->strParam.Trim();
				}
			}
		}

		//测试浏览器路径是否存在
		DWORD dwAtt = GetFileAttributes(pShellData->strPath);
		if((dwAtt == INVALID_FILE_ATTRIBUTES) || (dwAtt & FILE_ATTRIBUTE_DIRECTORY))
		{
			RegCloseKey(hKeyOpen);
			return FALSE;
		}

		// 测试是否可以不用DDE
		if((pShellData->strParam.GetLength() > 0) && (pShellData->strParam.Find(_T("%1")) != -1))
		{
			RegCloseKey(hKeyOpen);
			return TRUE;
		}

		// read ddeexec data
		dwSize = 1024*sizeof(TCHAR);
		memset(szValue, 0, dwSize);
		RegOpenKeyEx(hKeyOpen, _T("ddeexec"), 0, KEY_QUERY_VALUE, &hKey);
		RegCloseKey(hKeyOpen);
		if(hKey == NULL)
		{
			return FALSE;
		}

		BOOL bRet = FALSE;
		RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE)szValue, &dwSize);
		if(szValue[0])
		{
			pShellData->strDDEParam = szValue;

			// Application string
			HKEY hSubKey = NULL;
			RegOpenKeyEx(hKey, _T("Application"), 0, KEY_QUERY_VALUE, &hSubKey);
			if(hSubKey)
			{
				dwSize = 1024*sizeof(TCHAR);
				memset(szValue, 0, dwSize);
				RegQueryValueEx(hSubKey, NULL, NULL, NULL, (LPBYTE)szValue, &dwSize);
				pShellData->strDDEAppName = szValue;
				RegCloseKey(hSubKey);
			}

			// Topic string
			hSubKey = NULL;
			RegOpenKeyEx(hKey, _T("Topic"), 0, KEY_QUERY_VALUE, &hSubKey);
			if(hSubKey)
			{
				dwSize = 1024*sizeof(TCHAR);
				memset(szValue, 0, dwSize);
				RegQueryValueEx(hSubKey, NULL, NULL, NULL, (LPBYTE)szValue, &dwSize);
				pShellData->strDDETopic = szValue;
				RegCloseKey(hSubKey);
			}
			if((pShellData->strDDEParam.GetLength()>0)&&
				(pShellData->strDDEAppName.GetLength()>0)&&
				(pShellData->strDDETopic.GetLength()>0))
			{
				pShellData->bDDE = TRUE;
				if(pShellData->strDDEParam.Find(_T("%1")) != -1)
				{
					bRet = TRUE;
				}
			}
		}
		RegCloseKey(hKey);
		return bRet;
	}

	static HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, 
										 HDDEDATA hdata, ULONG_PTR dwData1, ULONG_PTR dwData2)
	{
		return NULL;
	}

	// 还原IE的打开方式
	static void SetIEPath(SHELL_REGDATA *pShellData)
	{
		TCHAR szPath[MAX_PATH] = {0};
		GetEnvironmentVariable(_T("ProgramFiles"), szPath, MAX_PATH);
		pShellData->strPath = szPath;
		pShellData->strPath += _T("\\Internet Explorer\\IEXPLORE.EXE");
		pShellData->bDDE = TRUE;
		pShellData->strParam = _T("-nohome");
		pShellData->strDDEParam = _T("\"%1\",,-1,0,,,,");
		pShellData->strDDEAppName = _T("IExplore");
		pShellData->strDDETopic = _T("WWW_OpenURLNewWindow");
	}

	// DDE方式打开
	static BOOL DDE_Exec(SHELL_REGDATA *pShellData, LPCTSTR pURL)
	{
		BOOL bRet = FALSE;
		CString str;
		DWORD dwInst = 0;
		DdeInitialize(&dwInst, DdeCallback, APPCLASS_STANDARD, 0);
		HSZ hszApp = DdeCreateStringHandle(dwInst, pShellData->strDDEAppName, CP_WINUNICODE);
		HSZ hszTopic = NULL;

		BOOL bIE = FALSE;
		str = pShellData->strPath;
		str.MakeLower();
		if(str.Find(_T("iexplore.exe")) > 0)
		{
			bIE = TRUE;
		}
		if(bIE)
		{
			// 如果是IE，用WWW_OpenURLNewWindow谓词，避免WWW_OpenURL替换老页面
			hszTopic = DdeCreateStringHandle(dwInst, _T("WWW_OpenURLNewWindow"), CP_WINUNICODE);
		}
		else
		{
			hszTopic = DdeCreateStringHandle(dwInst, pShellData->strDDETopic, CP_WINUNICODE);
		}
		HCONV hConv = DdeConnect(dwInst, hszApp, hszTopic, NULL);
		BOOL bConncetReady = FALSE;
		if(hConv == NULL)
		{
			bConncetReady = FALSE;
			PROCESS_INFORMATION pInfo = {0};
			STARTUPINFO sInfo = {0};
			sInfo.cb = sizeof(STARTUPINFO);
			str.Format(_T("\"%s\" %s"), pShellData->strPath, pShellData->strParam);
			if(CreateProcess(NULL, (TCHAR*)(LPCTSTR)str, NULL,NULL,FALSE,CREATE_NEW_PROCESS_GROUP|CREATE_UNICODE_ENVIRONMENT,NULL,NULL,&sInfo,&pInfo))
			{
				bConncetReady = TRUE;
			}
		}
		if(bConncetReady)
		{
			long count = 50;
			while(hConv == NULL)
			{
				count--;
				hConv = DdeConnect(dwInst, hszApp, hszTopic, NULL);
				if(hConv)
				{
					break;
				}
				if(count == 0)
				{
					break;
				}
				Sleep(20);
			}
		}

		if(hConv)
		{
			str = pShellData->strDDEParam;
			str.Replace(_T("%1"), pURL);
			DWORD dwRes = 0;
			HDDEDATA hDDEData = DdeClientTransaction((LPBYTE)(LPCTSTR)str, (str.GetLength()+1)*sizeof(TCHAR), hConv, NULL, CF_UNICODETEXT, XTYP_EXECUTE, 500, &dwRes);
			if(hDDEData)
			{
				DdeFreeDataHandle(hDDEData);
			}
			DdeDisconnect(hConv);
			bRet = TRUE;
		}

		DdeFreeStringHandle(dwInst, hszApp);
		DdeFreeStringHandle(dwInst, hszTopic);
		DdeUninitialize(dwInst);
		return bRet;
	}

	// 打开链接
};

} // namespace common

