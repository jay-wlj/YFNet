#include "stdafx.h"
#include "Coredump.h"
#include <Windows.h>
#include <time.h>
#include "..\Common\MachineTool.h"

#ifndef _IMAGEHLP_
#include <DbgHelp.h>
#pragma comment(lib,"dbghelp")
#endif

#include <Shellapi.h>


const TCHAR kCrashReportExeName[] = _T("CrashReport.exe");

#define DUMPNAME _T("crash.dmp")
#define LOGNAME  _T("crash.log")

TCHAR CoreDump::m_processVersion[20] ={0};
TCHAR CoreDump::m_processName[60] = {0};
TCHAR CoreDump::m_reportExeFilePath[MAX_PATH] = {0};

CString CoreDump::GetProcessName()
{
	TCHAR szFileNameBuffer[MAX_PATH] = {0};
	GetModuleFileName(NULL, szFileNameBuffer, MAX_PATH);
	CString fileName = szFileNameBuffer;
	int pos = fileName.ReverseFind('\\');
	if(pos > -1)
	{
		return fileName.Right(fileName.GetLength() - pos -1);
	}
	return _T("");
}

CString CoreDump::GetProcessVersion()
{
	CString version;
	TCHAR szQvodPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szQvodPath, MAX_PATH);
	CString szVersion;
	DWORD dwFileVersionInfoSize = GetFileVersionInfoSize(szQvodPath, NULL);
	if(dwFileVersionInfoSize == 0)
	{
		return version;
	}
	BYTE *lpData = new BYTE[dwFileVersionInfoSize];
	if(lpData == NULL)
	{
		return version;
	}
	if (!GetFileVersionInfo(szQvodPath, NULL, dwFileVersionInfoSize, lpData))
	{
		return version;
	}
	UINT unInfoLen = 0;
	VS_FIXEDFILEINFO* vsFileInfo = 0;
	if (! VerQueryValue(lpData, _T("\\"), (LPVOID*)&vsFileInfo, &unInfoLen))
	{
		return version;
	}
	if (unInfoLen != sizeof(VS_FIXEDFILEINFO))
	{
		return version;
	}
	// 版本信息 a.b.c.d
	version.Format(_T("%d.%d.%d.%d"),HIWORD(vsFileInfo->dwFileVersionMS), LOWORD(vsFileInfo->dwFileVersionMS),
		HIWORD(vsFileInfo->dwFileVersionLS), LOWORD(vsFileInfo->dwFileVersionLS));
	delete []lpData;
	return version;
}

BOOL CoreDump::SetExceptionHandle()
{
	TCHAR szBuffer[MAX_PATH] = {0};
	GetModuleFileName(NULL, szBuffer, MAX_PATH);
	CString processFile = szBuffer;
	int pos = processFile.ReverseFind('\\');
	if(pos < 0)
	{
		return FALSE;
		
	}
	CString name = processFile.Right(processFile.GetLength() - pos -1);
	CString reportExeFile = processFile.Left(pos+1) + kCrashReportExeName;
	pos = name.ReverseFind('.');
	if(pos >= 0)
	{
		name = name.Left(pos);
	}
	

	SetUnhandledExceptionFilter(CoreDump::MyUnhandledExceptionFilter); 

	lstrcpyn(m_processName,name,MAX_PATH);
	lstrcpyn(m_processVersion,GetProcessVersion(),MAX_PATH);
	lstrcpyn(m_reportExeFilePath,reportExeFile,MAX_PATH);
	return TRUE;
}
LONG WINAPI CoreDump::MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	TCHAR szTempDir[MAX_PATH] = {0};
	TCHAR szDumpFile[MAX_PATH] = {0};
	TCHAR szLogFile[MAX_PATH] = {0};
	GetTempDirPath(szTempDir);
	_sntprintf_s(szDumpFile,MAX_PATH,_T("%s\\%s"),szTempDir,DUMPNAME);

	if(!WriteDump(ExceptionInfo,szDumpFile))
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}
#ifdef CRASH_LOG
	_sntprintf_s(szLogFile,MAX_PATH,_T("%s\\%s"),szTempDir,LOGNAME);
	WriteLog(szLogFile);
#endif


	HMODULE hMoudle = NULL;
	TCHAR szModuleName[MAX_PATH] = {0};
	if(!GetExceptionInfo(ExceptionInfo->ExceptionRecord->ExceptionAddress,
		hMoudle,szModuleName,ARRAYSIZE(szModuleName))
		)
	{
		lstrcpyn(szModuleName,m_processName,MAX_PATH);
	}
	DWORD offsetAddr = (DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress - (DWORD)(void*)hMoudle;
	TCHAR szPara[MAX_PATH*5]={0};

#ifdef CRASH_LOG
	_sntprintf_s(szPara,MAX_PATH*5, _T("-d %s -f %s %s -pn %s -pv %s -mn %s -addr 0x%x -s"),
		         szTempDir,DUMPNAME, LOGNAME, m_processName, m_processVersion, szModuleName,0x55+(DWORD)offsetAddr);
#else
	_sntprintf_s(szPara,MAX_PATH*5, _T("-d %s -f %s -pn %s -pv %s -mn %s -addr 0x%x -s"),
		szTempDir,DUMPNAME, m_processName, m_processVersion, szModuleName,0x55+(DWORD)offsetAddr);
#endif
	
#ifdef CRASH_LOG
	CString strLogFrom;
	strLogFrom.Format(_T("%slog"), g_szAppPath);

	CString strLogTo;
	strLogTo.Format(_T("%s\\log"), szTempDir);

	QFile::Tool::CopyFile(strLogFrom, strLogTo);
#endif

	static BOOL s_bFirstReport = TRUE;
	if (s_bFirstReport)
	{
		s_bFirstReport = FALSE;
		RunRexport(szPara);				//同一个崩溃可能上报了两次crash
	}
	
	return EXCEPTION_EXECUTE_HANDLER;
}
void CoreDump::SuspendOthersThread()
{
}
BOOL CoreDump::GetExceptionInfo(IN PVOID pExceptionAddr,OUT HMODULE &hModule,OUT LPTSTR lpMouduleName, UINT nSize)
{
	if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)pExceptionAddr, &hModule))
	{
		TCHAR szModuleName[MAX_PATH]={0};
		if(GetModuleFileName(hModule, szModuleName, nSize) > 0)
		{
			TCHAR *p = _tcsrchr(szModuleName,_T('\\'));
			if(p)
			{
				lstrcpyn(lpMouduleName,p+1,nSize);
			}
			else
			{
				lstrcpyn(lpMouduleName,szModuleName,nSize);
			}
			p = _tcsrchr(lpMouduleName,_T('.'));
			if(p)
			{
				*p = 0;
			}
			return TRUE;
		}
	}
	return FALSE;
}
void CoreDump::GetTempDirPath(OUT LPTSTR lpTempDirPath)
{
	TCHAR szSysTmp[MAX_PATH] = {0};
	TCHAR szTmpDir[MAX_PATH] = {0};
	GetTempPath(MAX_PATH,szSysTmp);
	_sntprintf_s(szTmpDir,MAX_PATH,_T("%s~HntvC%llx.tmp"),szSysTmp,time(0));
	lstrcpyn(lpTempDirPath,szTmpDir,MAX_PATH);
	if(PathFileExists(lpTempDirPath))		 
	{
		DeleteFile(lpTempDirPath);	
	}
	CreateDirectory(lpTempDirPath,NULL);
}
BOOL CoreDump::WriteDump(struct _EXCEPTION_POINTERS* ExceptionInfo,LPCTSTR lpDumpPath)
{
	HANDLE hDumpFile = CreateFile(lpDumpPath,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if ( hDumpFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;
	bRet = MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hDumpFile, MiniDumpNormal,&loExceptionInfo,NULL,NULL);
	CloseHandle(hDumpFile);
	return bRet;
}
void CoreDump::RunRexport(LPCTSTR lpPara)
{
	//MessageBox(0,lpPara,0,0);
#ifdef _DEBUG_COREDUMP
	::ShellExecute(NULL ,_T("open") , m_reportExeFilePath , lpPara , NULL ,SW_SHOWNORMAL);
#else
	::ShellExecute(NULL ,_T("open") , m_reportExeFilePath , lpPara , NULL ,0);
#endif
}

void CoreDump::WriteLog(LPCTSTR lpLogPath)
{
#ifdef CRASH_LOG
	if(!lpLogPath) return;
	BYTE* pLogBuffer = NULL;
	int len = 0;
	g_playCtrl.GetCrashLog(&pLogBuffer,len);
	string data;
	CString tmp;

	//获取当前播放信息
	MixListItemData Info ;
	COnlineMediaMgr::Instance().GetPlayItem(&Info) ;
	MixListItemData *pPlayItem = &Info ;

	if (pPlayItem && pPlayItem->playlink)
	{
		tmp.Format(_T("PlayUrl ：%s \r\n"), pPlayItem->playlink);
		data += Convert::ToAString(tmp);
	}

	//tmp.Format(_T("HardAcc ：%d \r\n"),m_bStartHardAcc);
	//data += Convert::ToAString(tmp);

	tmp.Format(_T("CPU Name：%s \r\n"),MachineTool::GetProcessorName());
	data += Convert::ToAString(tmp);

	tmp.Format(_T("CPU Type：%d \r\n"),MachineTool::GetProcessorType());
	data += Convert::ToAString(tmp);

	CString videoCardName,videoCardVersion;
	MachineTool::GetVideoCardInfo(videoCardName,videoCardVersion);
	tmp.Format(_T("VideoCardName：%s version: %s\r\n"),videoCardName,videoCardVersion);
	data += Convert::ToAString(tmp);

	if(pLogBuffer)
	{
		data += string((char*)pLogBuffer,len);
	}
	QFile::Stream file(lpLogPath);
	file.WriteBlock(data);
#endif
}