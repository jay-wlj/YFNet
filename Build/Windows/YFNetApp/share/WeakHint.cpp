#include "stdafx.h"
#include "WeakHint.h"

#define _PRIVATE_WEAKHINT_TAG_  _T("ptetip")
static const TCHAR kUpdateTag[] = _T("update");	//格式 xxxx-xx-xx 例如2013-07-26

//////////////////////////////////////////////////////////////////////////
WearHint::WearHint()
{
	m_bEnable = TRUE;
	m_bInitialized = FALSE;
	m_pWeakHintWnd = NULL;
}

WearHint::~WearHint()
{

}

BOOL WearHint::Init_(CSkinWindow *pWeakHintWnd )
{
	if(pWeakHintWnd == NULL)
	{
		return FALSE;
	}

	if(m_bInitialized)
	{
		return TRUE;
	}
	
	m_bInitialized = TRUE;
	m_pWeakHintWnd = pWeakHintWnd;
	m_pWeakHintWnd->SetState(state_hide);
	return TRUE;
}

void WearHint::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;
}

void WearHint::Hide()
{
	if(m_pWeakHintWnd && m_pWeakHintWnd->IsVisible())
	{
		m_pWeakHintWnd->SetState(state_hide);
	}
}

BOOL WearHint::IsVisible()
{
	if(m_pWeakHintWnd)
	{
		return m_pWeakHintWnd->IsVisible();
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
LockTask::LockTask()
{
	m_bInitialize = FALSE;
	InitializeCriticalSection(&m_csLock);
}

LockTask::~LockTask()
{
	DeleteCriticalSection(&m_csLock);
}

LockTask& LockTask::Instance()
{
	static LockTask _instance;
	return _instance;
}

BOOL LockTask::Init_(CString strSaveDir)
{
	if(m_bInitialize)
	{
		return TRUE;
	}
	m_strSavePath = strSaveDir;
	m_bInitialize = TRUE;
	Load();
	return TRUE;
}

void LockTask::Clear_()
{
	m_bInitialize = FALSE;
	m_mHashStatus.clear();
	m_mUrlStatus.clear();
}

BOOL LockTask::IsLock(string hash ,CString strRefUrl)
{
	Parse(hash, strRefUrl);
	if(IsUrlLock(strRefUrl) || IsHasLock(hash))
	{
		return TRUE;
	}
	return FALSE;
}

void LockTask::QueryStatus(string hash ,CString strRefUrl)
{
	Parse(hash, strRefUrl);
	if(IsNeedQuery(hash, strRefUrl))
	{
		QueryPara *pPara = new QueryPara();
		pPara->pThis = this;
		pPara->hash = hash;
		pPara->strRefUrl = strRefUrl;
		HANDLE hHandle =  CreateThread(NULL , 0 ,Thread_QueryStatus ,pPara , 0 ,NULL);
		CloseHandle(hHandle);
	}
}

void LockTask::Load()
{
	CXMLDoc xmlDoc;
	if ( !xmlDoc.Init_())
	{
		return;
	}

	if ( INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_strSavePath) )
	{
		return;
	}

	//解密
	UINT newSize = En_DeFile::GetFileSize(m_strSavePath)+1;
	BYTE *bufOut = new BYTE[newSize];
	UINT nBufInSize;
	if (!En_DeFile::DecodeFile(m_strSavePath, bufOut, &nBufInSize) || !xmlDoc.LoadXml_((TCHAR*)bufOut))
	{
		xmlDoc.Clear_();
		delete[] bufOut;
		return ;
	}
	delete[] bufOut;

	EnterCriticalSection(&m_csLock);
	CXMLElement xmlRoot;
	xmlDoc.GetDocumentElement_(xmlRoot);
	if(IsNeedUpdate(xmlRoot.GetAttribute_(kUpdateTag)))
	{
		xmlDoc.Clear_();
		DeleteFile(m_strSavePath);
		LeaveCriticalSection(&m_csLock);
		return;
	}

	CXMLElement xmlHashs;
	CXMLNodeList xmlHashList;
	if(xmlRoot.SelectSingleNode_(_T("hashs"), xmlHashs)
		&& xmlHashs.GetChildNodes_(xmlHashList))
	{
		int nCount = xmlHashList.GetItemCount_();
		for (int i = 0; i < nCount ; i++)
		{
			CXMLElement xmlHashNode;
			xmlHashList.GetItemDisp_(i, xmlHashNode.m_Node);
			CString strHash = xmlHashNode.GetAttribute_(_T("hash"));
			int status = xmlHashNode.GetAttributeInt_(_T("status"));
			if(strHash.GetLength() <= 0 || status != fs_hash_lock)
			{
				continue;
			}

			char szHash[41] = {0};
			USES_CONVERSION;
			memcpy(szHash, W2A((LPCTSTR)strHash), 40);
			m_mHashStatus[string(szHash,40)] = status;
		}
	}

	CXMLElement xmlUrls;
	CXMLNodeList xmlUrlList;
	if(xmlRoot.SelectSingleNode_(_T("urls"), xmlUrls)
		&& xmlUrls.GetChildNodes_(xmlUrlList))
	{
		int nCount = xmlUrlList.GetItemCount_();
		for (int i = 0; i < nCount ; i++)
		{
			CXMLElement xmlUrlNode;
			xmlUrlList.GetItemDisp_(i, xmlUrlNode.m_Node);
			CString strUrl = xmlUrlNode.GetAttribute_(_T("url"));
			int status = xmlUrlNode.GetAttributeInt_(_T("status"));
			if(strUrl.GetLength() <= 0 || status != fs_url_lock)
			{
				continue;
			}
			m_mUrlStatus[strUrl] = status;
		}
	}
	xmlDoc.Clear_();
	LeaveCriticalSection(&m_csLock);
}

void LockTask::Save()
{
	CXMLDoc xmlDoc;
	if ( !xmlDoc.Init_() )
	{
		return;
	}		
	Load();
	EnterCriticalSection(&m_csLock);
	CString strVersion;
	strVersion.Format(_T("%d.%d.%d.%d"), QVOD_VERSION[0], QVOD_VERSION[1], QVOD_VERSION[2], QVOD_VERSION[3]);
	CString xmlTitle;
	xmlTitle.Format(_T("<?xml version='1.0' encoding='utf-8'?><%s version='%s'></%s>"), _PRIVATE_WEAKHINT_TAG_, strVersion, _PRIVATE_WEAKHINT_TAG_);
	if ( xmlDoc.LoadXml_(xmlTitle) )
	{
		CXMLElement xmlRoot;
		xmlDoc.GetDocumentElement_(xmlRoot);
		CXMLElement hashlistNode;
		xmlDoc.CreateElement_(_T("hashs"), hashlistNode);
		xmlRoot.AppendChild_(hashlistNode);
		xmlRoot.SetAttribute_(kUpdateTag,GetTodayString());

		map<string ,int>::iterator itHash = m_mHashStatus.begin();
		for( ;itHash != m_mHashStatus.end() ;itHash ++)
		{
			CXMLElement xmlHashNode;
			xmlDoc.CreateElement_(_T("hash"), xmlHashNode);
			char szHash[41] = {0};
			memcpy(szHash ,itHash->first.c_str() , itHash->first.size() > 40 ? 40 : itHash->first.size());
			CString strHash = CString(szHash) ;
			xmlHashNode.SetAttribute_(_T("hash"), strHash);
			xmlHashNode.SetAttribute_(_T("status") ,itHash->second);
			hashlistNode.AppendChild_(xmlHashNode);
		}

		CXMLElement urllistNode;
		xmlDoc.CreateElement_(_T("urls"), urllistNode);
		xmlRoot.AppendChild_(urllistNode);

		map<CString ,int>::iterator itUrl = m_mUrlStatus.begin();
		for( ;itUrl != m_mUrlStatus.end() ;itUrl ++)
		{
			CXMLElement xmlUrlNode;
			xmlDoc.CreateElement_(_T("url"), xmlUrlNode);
			xmlUrlNode.SetAttribute_(_T("url"), itUrl->first);
			xmlUrlNode.SetAttribute_(_T("status") ,itUrl->second);
			urllistNode.AppendChild_(xmlUrlNode);
		}

		// 加密
		CString strXml = xmlDoc.GetXml_();
		UINT nBufInSize = strXml.GetLength();
		TCHAR* bufIn = new TCHAR[nBufInSize*2+1];
		_tcsncpy(bufIn, strXml, nBufInSize*2);
		En_DeFile::EncodeFile(m_strSavePath, bufIn, nBufInSize*2);
		delete []bufIn;
	}
	xmlDoc.Clear_();
	LeaveCriticalSection(&m_csLock);
}

int LockTask::GetHashStatus(string hash)
{
	int status = 0;
	EnterCriticalSection(&m_csLock);
	map<string ,int>::iterator it = m_mHashStatus.find(hash);
	if(it != m_mHashStatus.end())
	{
		status = it->second;
	}
	LeaveCriticalSection(&m_csLock);
	return status;
}

int LockTask::GetUrlStatus(CString strUrl)
{
	int status = 0;
	EnterCriticalSection(&m_csLock);
	map<CString ,int>::iterator it = m_mUrlStatus.find(strUrl);
	if(it != m_mUrlStatus.end())
	{
		status = it->second;
	}
	LeaveCriticalSection(&m_csLock);
	return status;
}

void LockTask::SetHasStatus(string hash, int status)
{
	if(hash.size() > 0)
	{
		EnterCriticalSection(&m_csLock);
		if(fs_hash_lock == status)
		{
			m_mHashStatus[hash] = fs_hash_lock ;
			Save();
		}
		LeaveCriticalSection(&m_csLock);
	}
}

void LockTask::SetUrlStatus(CString strRefUrl, int status)
{
	if(strRefUrl.GetLength() > 0)
	{
		EnterCriticalSection(&m_csLock);
		if(fs_url_lock == status )
		{
			m_mUrlStatus[strRefUrl] = fs_url_lock;
			Save();
		}
		LeaveCriticalSection(&m_csLock);
	}
}

BOOL LockTask::IsHasLock(string hash)
{
	if(GetHashStatus(hash) == fs_hash_lock)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL LockTask::IsUrlLock(CString strUrl)
{
	if(GetUrlStatus(strUrl) == fs_url_lock)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL LockTask::IsNeedQuery(string hash, CString strRefUrl)
{
	if(IsHasLock(hash))
	{
		return FALSE;
	}
	if(IsUrlLock(strRefUrl))
	{
		return FALSE;
	}
	return TRUE;
}

void LockTask::Parse(string &hash, CString &strRefUrl)
{
	int nPos = strRefUrl.Find(_T("http://"));
	if(nPos != -1)
	{
		strRefUrl = strRefUrl.Right(strRefUrl.GetLength() - 7);
	}
	nPos = strRefUrl.Find(_T("/"));
	if(nPos != -1)
	{
		strRefUrl = strRefUrl.Left(nPos);
	}
}

DWORD WINAPI LockTask::Thread_QueryStatus(LPVOID p)
{
	::CoInitialize(0);
	QueryPara *pPara = (QueryPara*)p;
	if(pPara && pPara->pThis)
	{
		pPara->pThis->OnQueryStatus(pPara->hash ,pPara->strRefUrl);
	}
	::CoUninitialize();
	delete pPara;
	return 0;
}

// ********************************************************
// 备注：	向服务器查询文件的状态
// 接口	:	url？h=hash&s=site  eg:http://private.daddymami.net/?h=hash&s=site
// url		: 服务器的URL地址
// hash     : 文件hash
// site		: 文件引用页的域名
// 返回值 ：1	：url被锁	提示私人列表弱提示；且以后引用页为url的文件无需查询，直接提示私人列表弱提示
//			2	：url正常	暂时不会返回该值
//			3	：hash被锁	提示私人列表弱提示；且以后该hash的文件无需查询，直接提示私人列表弱提示
//			4	：找不到	不提示私人列表弱提示；下次还需到服务器查询，根据结果判断是否
//***********************************************************  
int LockTask::QueryInSrv(string hash ,CString strRefUrl, DWORD dwTimeOut /*= ATL_SOCK_TIMEOUT*/)
{
	int ret = fs_null;
	long nCopied = 0;
	HGLOBAL hBuffer = NULL;
	CString strUrl;
	strUrl.Format(_T("http://private.daddymami.net/?h=%s&s=%s"),Fun::ANSIToUnicode(hash) ,strRefUrl);
	string data;
	net::HttpDownloader downloader(strUrl, dwTimeOut);
	if(downloader.Request(data) && data.size() > 0)
	{
		ret = atoi(data.c_str());
	}
	return ret;
}

void LockTask::OnQueryStatus(string hash ,CString strRefUrl, DWORD dwTimeOut)
{
	int status = QueryInSrv(hash, strRefUrl, dwTimeOut);
	SetHasStatus(hash, status);
	SetUrlStatus(strRefUrl, status);
}

BOOL LockTask::IsNeedUpdate(CString updateTag)
{
	if(updateTag.GetLength() > 0 
		&& updateTag.Compare(GetTodayString()) == 0)
	{
		return FALSE;
	}
	return TRUE;
}

CString LockTask::GetTodayString()
{
	SYSTEMTIME nowTime;
	GetLocalTime(&nowTime);
	CString strToday;
	strToday.Format(_T("%04d-%02d-%02d"), nowTime.wYear, nowTime.wMonth, nowTime.wDay);
	return strToday;
}

#ifdef RDSP360
// 360版相关
BOOL LockTask::IsLockIn360Version(string hash ,CString strRefUrl)
{
	Parse(hash, strRefUrl);
	if(!IsHasLock360Version(hash))
	{
		OnQueryStatus(hash, strRefUrl, 3000);
	}
	return IsHasLock360Version(hash);
}

BOOL LockTask::IsHasLock360Version(string hash)
{
	if(GetHashStatus(hash) == fs_hash_lock)
	{
		return TRUE;
	}
	return FALSE;
}
#endif
//////////////////////////////////////////////////////////////////////////
/// 私人弱提示
PrivateWeakHint::PrivateWeakHint()
{
	m_bInitialize = FALSE;
	m_pWeakHintWnd = NULL;
	m_bEnable  = TRUE;
}

PrivateWeakHint::~PrivateWeakHint()
{
}

BOOL PrivateWeakHint::Init_(CSkinWindow *pWeakHintWnd ,CString strSaveDir)
{
	if(pWeakHintWnd == NULL)
	{
		return FALSE;
	}

	if(m_bInitialize)
	{
		return TRUE;
	}
	m_pWeakHintWnd = pWeakHintWnd;
	m_bInitialize = TRUE;
	if(m_pWeakHintWnd)
	{
		m_pWeakHintWnd->SetState(state_hide);
	}
	LockTask::Instance().Init_(strSaveDir);
	return TRUE;
}

void PrivateWeakHint::Clear_()
{
	m_bInitialize = FALSE;
	m_pWeakHintWnd = NULL;
	m_bEnable  = TRUE;	
}

BOOL PrivateWeakHint::Show(const string& hash)
{
	if(m_pWeakHintWnd)
	{
		CString strDescribe;
		int nPercent = 70;
		if(hash.size() > 1)
		{
			char szBuf[3] = {0};
			szBuf[0] = hash[0];
			szBuf[1] = hash[1];
			nPercent = TransPercent(strtol(szBuf , NULL ,16));
		}
		strDescribe.Format(ResString(STR_PTETIP_TXT, m_pWeakHintWnd), nPercent);

		int nTxtWidth = 0;
		CSkinCtrl* pBtn = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("pt_movepte")));
		if(pBtn)
		{
			pBtn->SetState(state_show);
			CString szText = ResString(IDS_PTETIP_MOVEPTE, m_pWeakHintWnd);
			int nBtnTxtWidth = pBtn->Method_(_T("text"), (long)(LPCTSTR)szText, 0);
			CSkinCtrl * pBtnTxt =dynamic_cast<CSkinCtrl*>(pBtn->FindChild(_T("txt")));
			if(pBtnTxt)
			{
				nBtnTxtWidth = nBtnTxtWidth + pBtnTxt->m_layout.x + pBtnTxt->m_layout.w;
			}
			pBtn->m_layout.w = nBtnTxtWidth;
			pBtn->m_oldSize.cx = 0;
		}

		CSkinCtrl* pTxt = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("describe")));
		CSkinCtrl * plock = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("lock")));
		if(pTxt)
		{
			pTxt->Method_(_T("text") ,(long)(LPCTSTR)strDescribe, 0);
			if ( pBtn && plock )
			{
				pTxt->m_layout.x = plock->m_layout.x + plock->m_layout.w + 2;
				pTxt->m_layout.w = pBtn->m_layout.x + pBtn->m_layout.w + 2;
			}
			pTxt->m_oldSize.cx = 0;
		}
		m_pWeakHintWnd->ReLayout();
		m_pWeakHintWnd->SetState(state_show);
		m_pWeakHintWnd->SetWindowPos(HWND_TOPMOST ,0 ,0 ,0 ,0 ,SWP_NOMOVE |SWP_NOSIZE|SWP_SHOWWINDOW);
		return TRUE;
	}
	return FALSE;
}
void PrivateWeakHint::Show(void)
{
	if ( m_strText.GetLength() )
	{
		Show(m_strText);
	}
}
void PrivateWeakHint::SetNormalText(LPCTSTR szNomalText)
{
	if ( szNomalText && szNomalText[0] )
	{
		m_strText = szNomalText;
	}
}
BOOL PrivateWeakHint::Show(LPCTSTR szNomalText)
{
	if ( !m_pWeakHintWnd || !szNomalText || !szNomalText[0] )
	{
		if ( m_strText.GetLength() )
		{
			m_strText.Empty();
			Hide();
		}
		return FALSE;
	}
	if ( CSkinCtrl * pBtn = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("pt_movepte"))) )
	{
		pBtn->SetState(state_hide);
	}
	if ( CSkinCtrl * pBtn = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("lock"))) )
	{
		pBtn->SetState(state_hide);
	}
	if ( CSkinCtrl* pTxt = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("describe"))) )
	{
		pTxt->Method_(_T("text") ,(long)szNomalText, 0);
		pTxt->m_layout.x = 5;
		pTxt->m_oldSize.cx = 0;
	}
	m_pWeakHintWnd->ReLayout();
	m_pWeakHintWnd->SetState(state_show);
	m_pWeakHintWnd->SetWindowPos(HWND_TOPMOST ,0 ,0 ,0 ,0 ,SWP_NOMOVE |SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE;
}

void PrivateWeakHint::Hide()
{
	if(m_pWeakHintWnd)
	{
		m_pWeakHintWnd->SetState(state_hide);
	}
}

BOOL PrivateWeakHint::IsVisible()
{
	if(m_pWeakHintWnd)
	{
		return m_pWeakHintWnd->IsVisible();
	}
	return FALSE;
}

BOOL PrivateWeakHint::IsEnable()
{
	return m_bEnable;
}

void PrivateWeakHint::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;
}

void PrivateWeakHint::ResetSkinElement( void )
{
	if(m_pWeakHintWnd)
	{
		return m_pWeakHintWnd->ResetSkinElement();
	}
}

void PrivateWeakHint::QueryStatus(string hash ,CString strRefUrl)
{
	LockTask::Instance().QueryStatus(hash, strRefUrl);
}

BOOL PrivateWeakHint::IsLock(string hash ,CString strRefUrl)
{
	return LockTask::Instance().IsLock(hash, strRefUrl);
}

#ifdef RDSP360
BOOL PrivateWeakHint::IsLockIn360Version(string hash ,CString strRefUrl)
{
	return LockTask::Instance().IsLockIn360Version(hash, strRefUrl);
}
#endif

int	PrivateWeakHint::TransPercent(unsigned char c)
{
	float f = float(c)/255;
	if(f < 0.01)
	{
		f = 0.7;
	}
	if(f < 0.7)
	{
		f= f/7 + 0.7;
	}
	return f*100;
}

//////////////////////////////////////////////////////////////////////////
// Vip弱提示
BufferWeakHint::BufferWeakHint()
{

	m_bEnable = TRUE;
	m_beginBufferingTime = 0;
	m_fristBufferingTime = time(NULL);
	m_nBufferingCount = 0;
	m_nValidInterval = 120;
	m_pWeakHintWnd = NULL;

}

BufferWeakHint::~BufferWeakHint()
{

}

BOOL BufferWeakHint::Init_(CSkinWindow *pWeakHintWnd )
{
	if(pWeakHintWnd == NULL)
	{
		return FALSE;
	}
	m_pWeakHintWnd = pWeakHintWnd;
	return TRUE;
}


BOOL BufferWeakHint::BeginBuffer()
{
	m_beginBufferingTime = time(NULL);
	return TRUE;
}

BOOL BufferWeakHint::EndBuffer()
{
	if(!m_bEnable)
	{
		return FALSE;
	}

	BOOL bTrigger = FALSE;
	time_t curTime = time(NULL);
	if(curTime - m_beginBufferingTime >= 3)
	{
		if(m_nBufferingCount == 0)
		{
			m_fristBufferingTime = m_beginBufferingTime;
		}
		m_nBufferingCount ++;
	}

	if(m_nBufferingCount >= 2)
	{
		if(curTime - m_fristBufferingTime > m_nValidInterval)
		{
			m_nBufferingCount = 0;
		}
		else 
		{
			bTrigger = TRUE;
		}
	}

	if(bTrigger)
	{
		m_nBufferingCount = 0;
	}
	return bTrigger;
}

void BufferWeakHint::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;
}

void BufferWeakHint::Show(const CString& txt,const CString& btnTxt)
{
	if(m_bEnable && m_pWeakHintWnd)
	{
		m_pWeakHintWnd->SetChildText(_T("describe"), txt);
		m_pWeakHintWnd->SetChildText(_T("buffer_btn"),btnTxt);
		m_pWeakHintWnd->SetState(state_show);
	}
}

void BufferWeakHint::Hide()
{
	if(m_pWeakHintWnd && m_pWeakHintWnd->IsVisible())
	{
		m_pWeakHintWnd->SetState(state_hide);
	}
}

BOOL BufferWeakHint::IsVisible()
{
	if(m_pWeakHintWnd)
	{
		return m_pWeakHintWnd->IsVisible();
	}
	return FALSE;
}
void BufferWeakHint::Reset()
{
	m_nBufferingCount = 0;
	m_bEnable = TRUE;
}

//////////////////////////////////////////////////////////////////////////
#ifdef RDSP360
STDVerWeakhint::STDVerWeakhint()
{
	m_bInitialize = FALSE;
	m_bEnable = TRUE;
	m_bEnabTimeTick = TRUE;
	m_pWeakHintWnd = NULL;
	m_rtTriggerTime = 0;
}

STDVerWeakhint::~STDVerWeakhint()
{

}

BOOL STDVerWeakhint::Init_(CSkinWindow *pWeakHintWnd )
{
	if(pWeakHintWnd == NULL)
	{
		return FALSE;
	}

	if(m_bInitialize)
	{
		return TRUE;
	}
	m_pWeakHintWnd = pWeakHintWnd;
	m_bInitialize = TRUE;
	if(m_pWeakHintWnd)
	{
		m_pWeakHintWnd->SetState(state_hide);
	}
	return TRUE;
}

void STDVerWeakhint::Clear_()
{
	m_bInitialize = FALSE;
	m_bEnable = TRUE;
	m_bEnabTimeTick = TRUE;
	m_pWeakHintWnd = NULL;
	m_rtTriggerTime = 0;
}

void STDVerWeakhint::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;
}

BOOL STDVerWeakhint::IsEnable()
{
	return m_bEnable;
}

CSkinWindow* STDVerWeakhint::GetWindow()
{
	return m_pWeakHintWnd;
}

BOOL STDVerWeakhint::Show()
{
	if(m_pWeakHintWnd)
	{
		CString strDescribe = ResString(STR_UPGRADE_STDVER, m_pWeakHintWnd);
		int nTxtWidth = 0;
		CSkinCtrl* pBtn = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("upgradeSTD")));
		if(pBtn)
		{
			CString szText = ResString(IDS_REMIND_STDVER_NOWINST, m_pWeakHintWnd);
			int nBtnTxtWidth = pBtn->Method_(_T("text"), (long)(LPCTSTR)szText, 0);
			CSkinCtrl * pBtnTxt =dynamic_cast<CSkinCtrl*>(pBtn->FindChild(_T("txt")));
			if(pBtnTxt)
			{
				nBtnTxtWidth = nBtnTxtWidth + pBtnTxt->m_layout.x + pBtnTxt->m_layout.w;
			}
			pBtn->m_layout.w = nBtnTxtWidth;
			pBtn->m_oldSize.cx = 0;
		}

		CSkinCtrl* pTxt = dynamic_cast<CSkinCtrl*>(m_pWeakHintWnd->FindChild(_T("describe")));
		if(pTxt)
		{
			pTxt->Method_(_T("text") ,(long)(LPCTSTR)strDescribe, 0);
			if(pBtn)
			{
				pTxt->m_layout.w = pBtn->m_layout.x + pBtn->m_layout.w + 2;
			}
			pTxt->m_oldSize.cx = 0;
		}
		m_pWeakHintWnd->ReLayout();
		m_pWeakHintWnd->SetState(state_show);
		m_pWeakHintWnd->SetWindowPos(HWND_TOPMOST ,0 ,0 ,0 ,0 ,SWP_NOMOVE |SWP_NOSIZE|SWP_SHOWWINDOW);
		return TRUE;
	}
	return FALSE;
}

void STDVerWeakhint::Hide()
{
	if(m_pWeakHintWnd)
	{
		m_pWeakHintWnd->SetState(state_hide);
	}
}

BOOL STDVerWeakhint::IsVisible()
{
	if(m_pWeakHintWnd)
	{
		return m_pWeakHintWnd->IsVisible();
	}
	return FALSE;
}

void STDVerWeakhint::EnableTimeTick(BOOL bEnable)
{
	m_bEnabTimeTick = bEnable;
}

BOOL STDVerWeakhint::IsEnableTimeTick()
{
	return m_bEnabTimeTick;
}

// 影片长度需30分钟以上
// 播放器结束前3分钟时，显示提示。
BOOL STDVerWeakhint::IsEnableTimeTick(REFERENCE_TIME totalTime)
{
	REFERENCE_TIME minTotalTime = 30*60;
	minTotalTime *= 1000*10000;
	if(m_bEnabTimeTick && totalTime > minTotalTime)
	{
		m_rtTriggerTime = totalTime - minTotalTime/10;
		return TRUE;
	}
	return FALSE;
}


BOOL STDVerWeakhint::TimeTick(REFERENCE_TIME curTime)
{
	if(m_rtTriggerTime < curTime)
	{
		return TRUE;
	}
	return FALSE;
}
#endif