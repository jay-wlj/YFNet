#pragma once

#include "stdafx.h"
#include "../FLLog/FLLog.h"
#include "../Common/AutoPtr.h"
#include "../include/function.h"

#define FL_LOG (CFLLog::Instance().WriteLog)
#define FL_LOG_RELEASE (CFLLog::Instance().UnInit())
#define FL_INIT(X) (CFLLog::Instance().Init(X))

#define BUF_MAX_SIZE (10*1024)
#define BUF_SIZE (1024)

class CFLLog
{
private:
	CFLLog()
	{
		Init();
	}
	~CFLLog()
	{
		UnInit();
	}
public:
	static CFLLog& Instance()
	{
		static CFLLog s_instance;
		return s_instance;
	}
	void Init(HMODULE hModule = NULL)
	{
		TCHAR szFilePath[MAX_PATH] = {0};
		GetModuleFileName(hModule, szFilePath, MAX_PATH);
		PathRemoveFileSpec(szFilePath);
		CString strLogDllPath = szFilePath;
		strLogDllPath += _T("\\FLLog.dll");
		m_hLogDll = LoadLibrary(strLogDllPath);
		if (m_hLogDll)
		{
			PGetFLLog pFLLog = (PGetFLLog)GetProcAddress(m_hLogDll, "GetFLLog");

			if (pFLLog)
			{
				m_pFLLog = pFLLog();
			}
		}
	}

	void UnInit()
	{
		if (m_hLogDll)
		{
			m_pFLLog = NULL;
			PReleaseFLLog pReleaseFLLog = (PReleaseFLLog)GetProcAddress(m_hLogDll, "ReleaseFLLog");
			if (pReleaseFLLog)
			{
				pReleaseFLLog();
			}
			FreeLibrary(m_hLogDll);
			m_hLogDll = NULL;
		}
	}

	void WriteLog(const char * fmt, ...)
	{
		if (m_pFLLog && fmt)
		{

			try
			{
				if (NULL != strstr(fmt, "%"))			//根据格式化串判断有无参
				{
				va_list args;
				va_start(args, fmt);

					std::string strRet = GetVarBuffer(fmt, args);
				va_end(args);

					m_pFLLog->WriteLog(strRet.c_str());
				}
				else
				{
					m_pFLLog->WriteLog(fmt);
				}
			}
			catch (...)
			{
				OutputDebugStringA("catch WriteLog");
			}
		}
	}

	void WriteLog(const wchar_t * fmt, ...)
	{
		if (m_pFLLog && fmt)
		{

			try
			{
				if (NULL != wcsstr(fmt, L"%"))
				{
				va_list args;
				va_start(args, fmt);
					std::string strRet = GetVarBuffer(fmt, args);
				va_end(args);

					m_pFLLog->WriteLog(strRet.c_str());
				}
				else
				{
					m_pFLLog->WriteLog(fmt);
				}
			}
			catch (...)
			{
				OutputDebugStringA("catch WriteLog");
			}

		}
	}

	void WriteLog(BOOL bTrace, const char * fmt, ...)
	{
		if (NULL == fmt)
		{
			return;
		}

		try
		{
			if (NULL != strstr(fmt, "%"))
			{
			va_list args;
			va_start(args, fmt);
				std::string strRet = GetVarBuffer(fmt, args);
			va_end(args);

			if (bTrace)
				{
					OutputDebugStringA(strRet.c_str());
				}
				if (m_pFLLog)
				{
					m_pFLLog->WriteLog(strRet.c_str());
				}
			}
			else
			{
				if (bTrace)
				{
					OutputDebugStringA(fmt);
			}

			if (m_pFLLog)
			{
					m_pFLLog->WriteLog(fmt);
				}
			}
		}
		catch (...)
		{
			OutputDebugStringA("catch WriteLog");
		}
	}

	void WriteLog(BOOL bTrace, const wchar_t * fmt, ...)
	{
		if (NULL == fmt)
		{
			return;
		}
		wchar_t sprint_buf[BUF_MAX_SIZE] = {0};

		try
		{
			if (NULL != wcsstr(fmt, L"%"))
			{
			va_list args;
			va_start(args, fmt);
				std::string strRet = GetVarBuffer(fmt, args);
			va_end(args);

			if (bTrace)
				{
					OutputDebugStringA(strRet.c_str());
				}
				if (m_pFLLog)
				{
					m_pFLLog->WriteLog(strRet.c_str());
				}
			}
			else
			{
				if (bTrace)
				{
					OutputDebugString(fmt);
			}

			if (m_pFLLog)
			{
					m_pFLLog->WriteLog(fmt);
				}
			}
		}
		catch (...)
		{
			OutputDebugStringA("catch WriteLog");
		}
	}
	std::string GetVarBuffer(const char *fmt, va_list& args)
	{
		std::string strRet;
		int len = strlen(fmt);
		DWORD dwBufLen = BUF_SIZE;
		dwBufLen = dwBufLen <= len? len+BUF_SIZE: dwBufLen;
		try
		{
			QPtr::AutoArrayPtr<char> buf =  new char[dwBufLen]();
			while (-1== _vsnprintf(buf, dwBufLen, fmt, args))
			{
				dwBufLen *= 2;
				buf = new char[dwBufLen]();
			}
			strRet = buf;
		}
		catch(...)
		{
			DWORD dwBufLen = len+100;
			QPtr::AutoArrayPtr<char> buf = new char[dwBufLen]();
			sprintf_s(buf, dwBufLen, "catch LOG GetVarBuffer: %s", fmt);
			strRet = buf;
		}
		return strRet;
	}
	std::string GetVarBuffer(const wchar_t *fmt, va_list& args)
	{
		std::string strRet;
		int len = wcslen(fmt);
		DWORD dwBufLen = BUF_SIZE;
		dwBufLen = dwBufLen <= len? len+BUF_SIZE: dwBufLen;
		try
		{
			QPtr::AutoArrayPtr<wchar_t> buf =  new wchar_t[dwBufLen]();
			while (-1== _vsnwprintf(buf, dwBufLen, fmt, args))
			{
				dwBufLen *= 2;
				buf = new wchar_t[dwBufLen]();
			}
			strRet = Fun::UnicodeToANSI(buf);
		}
		catch(...)
		{
			DWORD dwBufLen = len+100;
			QPtr::AutoArrayPtr<wchar_t> buf = new wchar_t[dwBufLen]();
			swprintf_s(buf, dwBufLen, L"catch LOG GetVarBuffer: %s", fmt);
			strRet = Fun::UnicodeToANSI(buf);
		}
		return strRet;
	}
private:
	HMODULE m_hLogDll;					//日志模块
	IFLLog			*m_pFLLog;
};