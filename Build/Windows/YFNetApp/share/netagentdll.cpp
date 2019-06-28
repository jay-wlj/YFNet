
#include <windows.h>
#include <TCHAR.H>
#include <vector>
#include "Registry.h"
using namespace std;
//using namespace QVODRE::registry;
#include "netagentdll.h"

CNetagentdll::CNetagentdll()
{

	m_pfnCreateAndRunAgent_ = NULL;
	m_pfnSeek_ = NULL;
	m_pfnGetFileLen_ = NULL;
	m_pfnGetDownloadLen_ = NULL;
	m_pfnTerminal_ = NULL;
	m_nid  = -1 ;
	m_dllhandle = NULL;
	m_savepath = NULL;
}

CNetagentdll::~CNetagentdll()
{
	Init();
}

void CNetagentdll::Init()
{
	if(m_nid >= 0 && m_dllhandle != NULL && m_pfnTerminal_ != NULL)
	{
		m_pfnTerminal_(m_nid);
		::FreeLibrary(m_dllhandle);
	}
	m_pfnCreateAndRunAgent_ = NULL;
	m_pfnSeek_ = NULL;
	m_pfnGetFileLen_ = NULL;
	m_pfnGetDownloadLen_ = NULL;
	m_pfnTerminal_ = NULL;
	m_nid  = -1 ;
	m_dllhandle = NULL ;
	if(m_savepath)
	{
		::DeleteFile(m_savepath);
		delete []m_savepath;
		m_savepath = NULL;
	}

}

int CNetagentdll::CreateAgentAndRun(TCHAR * url, TCHAR *savepath, NOTIFYFUNC pfunc)
{
	if(m_dllhandle != NULL)
	{
		Init();
	}
	TCHAR pathdll[MAX_PATH*2];
	memset(pathdll, 0, MAX_PATH*2*sizeof(TCHAR));
	TCHAR exepath[MAX_PATH*2];
	memset(exepath, 0, MAX_PATH*2*sizeof(TCHAR));
	::GetModuleFileName(0, exepath, MAX_PATH*2);
	TCHAR *pline = _tcsrchr(exepath, '\\');
	*(pline+1) = 0;
	_stprintf(pathdll, _T("%s%s"), exepath, DLLNAME);
	m_dllhandle = ::LoadLibrary( pathdll);
	if(!m_dllhandle)
	{//query 

		TCHAR Insertpath[MAX_PATH*2];
		memset(Insertpath, 0, MAX_PATH*2*sizeof(TCHAR));
		QVODRE::registry::CRegistry regpath;
		regpath.GetString(QVODRE::registry::lpInsert, QVODRE::registry::lpInsert_path, Insertpath, MAX_PATH*2);
		_stprintf(pathdll, _T("%s%s"), Insertpath, DLLNAME);
		m_dllhandle = ::LoadLibrary( pathdll);
	}
	if(m_dllhandle)
	{
		m_pfnCreateAndRunAgent_ = (pfnCreateAndRunAgent) GetProcAddress(m_dllhandle, "CreateAndRunAgent");
		m_pfnSeek_ = (pfnSeek) GetProcAddress(m_dllhandle, "Seek");
		m_pfnGetFileLen_ = (pfnGetFileLen) GetProcAddress(m_dllhandle, "GetFileLen");
		m_pfnGetDownloadLen_ = (pfnGetDownloadLen) GetProcAddress(m_dllhandle, "GetDownloadLen");
		m_pfnTerminal_ = (pfnTerminal) GetProcAddress(m_dllhandle, "Terminate");
		if(!m_pfnTerminal_)
			m_pfnTerminal_ = (pfnTerminal) GetProcAddress(m_dllhandle, "Terminal");
		if(m_pfnCreateAndRunAgent_)
		{
			int nlen1 = _tcslen(savepath) + 1;
			m_savepath = new TCHAR[nlen1];
			memset(m_savepath, 0, nlen1*sizeof(TCHAR));
			_tcscpy(m_savepath, savepath);
			char *psurl = NULL;
#ifdef _UNICODE
			nlen1 = (_tcslen(url) + 1) * 2;
			psurl = new char [nlen1];
			memset(psurl, 0, nlen1);
			WideCharToMultiByte(CP_ACP, 0, url, -1, psurl, nlen1, 0, 0);
#else
			nlen1 = strlen(url) + 1;
			psurl = new char [nlen1];
			memset(psurl, 0, nlen1);
			strcpy(psurl, url);
#endif
			m_nid = m_pfnCreateAndRunAgent_(psurl, savepath, pfunc);
			if(psurl)
			{
				delete []psurl;
				psurl = NULL;
			}
			if(m_nid < 0)
			{
				m_nid = -1;
				::FreeLibrary(m_dllhandle);
				m_pfnCreateAndRunAgent_ = NULL;
				m_pfnSeek_ = NULL;
				m_pfnGetFileLen_ = NULL;
				m_pfnGetDownloadLen_ = NULL;
				m_pfnTerminal_ = NULL;
				m_dllhandle = NULL ;
				return -1;
			}
		}
	}
	return 0;
}

bool CNetagentdll::Seek(UINT pos)
{
	if(m_dllhandle && m_nid >= 0  && m_pfnSeek_)
	{
		return m_pfnSeek_(m_nid, pos);
	}
	return false;
}

int CNetagentdll::GetFileLen()
{
	if(m_dllhandle && m_nid >= 0  && m_pfnGetFileLen_)
	{
		return m_pfnGetFileLen_(m_nid);
	}
	return 0;
}

UINT CNetagentdll::GetDownloadLen()
{
	if(m_dllhandle && m_nid >= 0  && m_pfnGetDownloadLen_)
	{
		return m_pfnGetDownloadLen_(m_nid);
	}
	return 0;
}

int CNetagentdll::GetAgentID()
{
	return m_nid;
}

void CNetagentdll::TerminateAgent()
{
	m_pfnTerminal_(m_nid);
	m_nid = -1;
}