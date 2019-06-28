//netagentdll.h
#ifndef _NETAGENTDLL_H__
#define _NETAGENTDLL_H__

#ifndef _DATARANGE_
#define _DATARANGE_
struct DataRange
{
	UINT from;
	UINT to;
};
#endif
typedef BOOL (*NOTIFYFUNC)(std::vector<DataRange>& pdatarange);
//typedef BOOL (*NOTIFYFUNC)(std::vector<DataRange>* pdatarange);
#ifndef _NETAGENT_DLL_API_
#define _NETAGENT_DLL_API_
typedef int  ( *pfnCreateAndRunAgent ) (char *, wchar_t *, NOTIFYFUNC pfunc);
typedef bool ( *pfnSeek ) (int , UINT);
typedef int ( *pfnGetFileLen )(int);
typedef UINT ( *pfnGetDownloadLen )(int);
typedef void ( *pfnTerminal )(int);
#endif

#define DLLNAME				_T("NetAgent.dll")
class CNetagentdll
{
public:
	CNetagentdll();
	~CNetagentdll();
	int CreateAgentAndRun(TCHAR * url, TCHAR *savepath, NOTIFYFUNC pfunc);
	bool Seek(UINT pos);
	int  GetFileLen();
	UINT GetDownloadLen();
	int GetAgentID();
	void TerminateAgent();
protected:

private:
	void Init();
private:
	pfnCreateAndRunAgent	m_pfnCreateAndRunAgent_;
	pfnSeek					m_pfnSeek_;
	pfnGetFileLen			m_pfnGetFileLen_;
	pfnGetDownloadLen		m_pfnGetDownloadLen_;
	pfnTerminal				m_pfnTerminal_;
private:
	int		m_nid;
	HMODULE	m_dllhandle;
	TCHAR   *m_savepath;
};


#endif