#include "stdafx.h"
#include "Filter.h"
#include "..\Common\XmlConfig.h"
#include "..\Common\url.h"

#define _DEBUG_FILTER
#undef OutString(str)
#if (defined _DEBUG) && (defined _DEBUG_FILTER)
#include "..\include\Console.h"
static ZConsole con2;
#define OutString(str) do{con2 << str;}while(0)
#else
#define OutString(str) 
#endif

const TCHAR kWhiteListUrl[] = _T("http://s.yunfan.com/client/xml/Text5.xml");

namespace filter{

CWhiteList::CWhiteList()
{
	
}
// static
CWhiteList& CWhiteList::Instance()
{
	static CWhiteList n;
	return n;
}
void CWhiteList::Init(const CString& filePath)
{
	m_saveFilePath = filePath;
	Load();
	HANDLE hHandle = CreateThread(0, 0, Thread_Update, this, 0, 0);
	CloseHandle(hHandle);
}
BOOL CWhiteList::IsContainUrl(const CString& url)
{
	BOOL bExist = FALSE;
	CString domain = GetDomain(url);
	m_lock.Lock();
	std::set<CString>::iterator it = m_urls.find(domain);
	if(it != m_urls.end())
	{
		bExist = TRUE;
	}
	m_lock.Unlock();
	return bExist;
}
// static
DWORD WINAPI CWhiteList::Thread_Update(LPVOID p)
{
	CWhiteList *pThis = (CWhiteList*) p;
	CoInitialize(0);
	pThis->OnThreadUpdate();
	CoUninitialize();
	return 0;
}
void CWhiteList::OnThreadUpdate()
{
	
	//œ¬‘ÿkeywords.xml
	XMLConfig::Updater updater(m_saveFilePath,
								kWhiteListUrl,
								0,
								TRUE);
	if(updater.Update())
	{
		Load();
	}
}
void CWhiteList::Load()
{
	CXMLDoc doc;
	if(!doc.Init_() || !XMLConfig::Tool::LoadXml(doc,m_saveFilePath,XMLConfig::Cipher_Common))
	{
		doc.Clear_();
		return ;
	}
	CXMLElement xmlRoot;
	doc.GetDocumentElement_(xmlRoot);
	CXMLNodeList nodelist;
	xmlRoot.GetChildNodes_(nodelist);
	int count = nodelist.GetItemCount_();
	CString url;

	OutString("===============CWhiteList::Load========================\r\n");

	m_lock.Lock();
	m_urls.clear();
	for(long i=0; i<count; i++)
	{
		CXMLElement node;
		nodelist.GetItemDisp_(i, node.m_Node);
		if(node.GetText_(url))
		{
			CString str = GetDomain(url);
			m_urls.insert(str);
			OutString("url:");OutString(str);OutString("\r\n");
		}
	}
	m_lock.Unlock();
}
CString  CWhiteList::GetDomain(const CString& url)
{
	CString str = url;
	str.MakeLower();
	return Convert::ToString(qurl::Tool::GetDomain((LPCSTR)Convert::ToAString(str)));
}

}