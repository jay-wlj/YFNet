#pragma once

#include <set>
#include "..\Common\lock.h"

namespace filter{

// °×Ãûµ¥
class CWhiteList
{
private:
	CWhiteList();
public:
	static CWhiteList& Instance();
	void Init(const CString& filePath);
	BOOL IsContainUrl(const CString& url);

	static DWORD WINAPI Thread_Update(LPVOID p);
	void OnThreadUpdate();
protected:
	CString GetDomain(const CString &url);
	void Load();
private:
	std::set<CString> m_urls;
	CString m_saveFilePath;
	QLock::CriticalSection m_lock;
};

}