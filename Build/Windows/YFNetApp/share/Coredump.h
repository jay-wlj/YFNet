#ifndef _COREDUMP_H_H__
#define _COREDUMP_H_H__

#include <string>
#include <atlstr.h>
#include <tchar.h>
using std::string;

class CoreDump
{
public:
	static BOOL SetExceptionHandle();
private:
	// 自定义异常过滤程序
	static LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo); 
	static void SuspendOthersThread();
	static BOOL GetExceptionInfo(IN PVOID pExceptionAddr,OUT HMODULE &hModule,OUT LPTSTR lpMouduleName, UINT nSize);
	static void GetTempDirPath(OUT LPTSTR lpTempDirPath);
	static BOOL WriteDump(struct _EXCEPTION_POINTERS* ExceptionInfo,LPCTSTR lpDumpPath);
	static void RunReportExe(LPCTSTR lpDumpPath,LPCTSTR lpExceptionModuleName,PVOID offsetAddr);
	static void RunRexport(LPCTSTR lpPara);
	static void WriteLog(LPCTSTR lpLogPath);

	static CString GetProcessName();
	static CString GetProcessVersion();
private:
	static TCHAR m_processVersion[20];
	static TCHAR m_processName[60];
	static TCHAR m_reportExeFilePath[MAX_PATH];
};

#endif