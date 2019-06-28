// UploadPage.cpp : 实现文件
//

#include "stdafx.h"
#include "UploadPage.h"
#include "afxdialogex.h"
#include "NetSign.h"
#include "sha1.h"
#include "function.h"
#include "json\json.h"

// CUploadPage 对话框

IMPLEMENT_DYNAMIC(CUploadPage, CDialog)

CUploadPage::CUploadPage(CWnd* pParent /*=NULL*/)
	: CDialog(CUploadPage::IDD, pParent)
{
	m_bPause = false;
	m_bExit = false;
	m_hThread = NULL;
}

CUploadPage::~CUploadPage()
{
	m_bExit = TRUE;
	WaitForSingleObject(m_hThread, INFINITE);
}

void CUploadPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_UPFILE, m_listCtrl);
	DDX_Control(pDX, IDC_COMBO_LIMIT_MODE, m_comboLimit);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPauseResume);
}


BEGIN_MESSAGE_MAP(CUploadPage, CDialog)
	ON_BN_CLICKED(ID_START_UPLOAD, &CUploadPage::OnBnClickedStartUpload)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CUploadPage::OnBnClickedBtnPause)
	ON_CBN_SELCHANGE(IDC_COMBO_LIMIT_MODE, &CUploadPage::OnCbnSelchangeComboLimitMode)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER+100, OnGetFileHash)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DROPFILES()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_UPFILE, &CUploadPage::OnLvnItemchangedListUpfile)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_UPFILE, &CUploadPage::OnNMRClickListUpfile)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CUploadPage 消息处理程序

DWORD ThreadFileHash(LPVOID lpvoid)
{
	CUploadPage *pThis = (CUploadPage*)lpvoid;
	if (pThis)
	{
		pThis->CaclFileHash();
	}
	return 0;
}

BOOL CUploadPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT);
	// TODO:  在此添加额外的初始化
	m_listCtrl.InsertColumn(0, _T("任务hash"), 0, 200);
	m_listCtrl.InsertColumn(1, _T("文件路径"), 0, 100);
	m_listCtrl.InsertColumn(2, _T("文件大小"), 0, 100);
	m_listCtrl.InsertColumn(3, _T("上传大小"), 0, 100);
	m_listCtrl.InsertColumn(4, _T("上传速度"), 0, 100);
	m_listCtrl.InsertColumn(5, _T("完成百分比"), 0, 100);
	m_listCtrl.InsertColumn(6, _T("状态"), 0, 100);

	m_comboLimit.AddString(_T("默认限速模式"));
	m_comboLimit.AddString(_T("上网模式"));
	m_comboLimit.AddString(_T("观影模式"));
	m_comboLimit.AddString(_T("游戏模式"));
	m_comboLimit.SetCurSel(0);

	m_btnPauseResume.EnableWindow(FALSE);

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFileHash, this, 0, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int QvodGetFilesInFolder(const char *pathname, vector<string>& vFileNames)
{
	std::string strfolder = std::string(pathname);
	if (strfolder[strfolder.length()-1] != '/')
	{
		strfolder += "/";  
	}
	strfolder += "*.*";  
	WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA(strfolder.c_str(), &findFileData);  
    if (hFind != INVALID_HANDLE_VALUE)  
    {  
        do   
        {  
            if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)  
            {  
                continue;  
            }  
            else if (! (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  
            {  
                vFileNames.push_back(findFileData.cFileName);
            }  

        } while (FindNextFileA(hFind, &findFileData));  
		::FindClose(hFind);
    }  
	return 0;
}

static BOOL GetFileHash(LPCSTR path, string& strHash)
{
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwHiSize;
		DWORD dwLoSize = GetFileSize(hFile, &dwHiSize);
		unsigned __int64 dwFileSize = dwHiSize;
		dwFileSize <<= 32;
		dwFileSize += dwLoSize;
		HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, dwHiSize, dwLoSize, NULL);
		if(hFileMapping)
		{
			BYTE *pByte = (BYTE*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
			if(pByte)
			{
				CSHA1 sha((void*)pByte, dwFileSize);
				strHash = sha.readHex();
				UnmapViewOfFile(pByte);
			}
			CloseHandle(hFileMapping);
		}
		CloseHandle(hFile);
		return TRUE;
	}
	return FALSE;
}



void CUploadPage::CaclFileHash()
{
	while (! m_bExit)
	{
		string strFilePath;
		{
			QLock::AutoCriticalSection lock(m_fileLock);
			if (m_vFilePaths.size() > 0)
			{
				strFilePath = m_vFilePaths[0];	
				m_vFilePaths.erase(m_vFilePaths.begin());
			}
		}

		if (! strFilePath.empty())
		{
			string strHash;
			if (GetFileHash(strFilePath.c_str(), strHash))
			{
				{
					QLock::AutoCriticalSection lock(m_fileLock);
					m_mFileHash[strFilePath] = strHash;
				}

				SendMessage(WM_USER+100, (LPARAM)strFilePath.c_str(), 0);
			}
		}
		Sleep(20);
	}
}
void CUploadPage::CreateUploadTask(string strfilePath)
{
	char szProxyUrl[200] = {0};
	char szHash[41] = {0};

	string strUrl="https://upload.nicefilm.com/upload/chunk/upload";
	int ret = m_pIYfnet->CreateUploadTask(1, strUrl.c_str(),  strfilePath.c_str(),  szHash);

	for (int i=0; i<m_vItems.size(); i++)
	{
		if (m_vItems[i]->strHash == szHash)		// 已在列表中
		{
			return;
		}
	}

	ItemData *pData = new ItemData;
	pData->strHash.assign(szHash, 40);
	pData->strFileName = strfilePath;
	pData->iUpSize = 0;
	pData->iUpSpeed = 0;
	m_vItems.push_back(pData);

	wchar_t szTemp[10];
	int idx = m_listCtrl.InsertItem(0, L"");
	m_listCtrl.SetItemData(idx, (DWORD_PTR)pData);
	UpdateData();
	m_vFilePaths.push_back(strfilePath);	// 获取文件hash
}

LRESULT CUploadPage::OnGetFileHash(WPARAM wParam, LPARAM lParam)
{
	string strFilePath = (const char*)wParam;
	//CreateUploadTask(strFilePath);
	{
		char szHash[41] = {0};
		string strUrl="https://upload.nicefilm.com/upload/chunk/upload";
		int ret = m_pIYfnet->CreateUploadTask(1, strUrl.c_str(),  strFilePath.c_str(),  szHash);

		std::map<std::string, std::string>	 mHeadAdd;
		net::CNetSign http(strUrl);

		http.Download();
		http.GetHeadAdd(mHeadAdd);
		//mHeadAdd["X-YF-Token"] = "T1tSstXueYA4rolSJt-_mgle-Cwd4mYf9eM22fpqT-fsE.eb3";
		mHeadAdd["Content-Type"] = "video/mp4";

		{
			QLock::AutoCriticalSection lock(m_fileLock);
			mHeadAdd["x-yf-hash"] = m_mFileHash[strFilePath];
		}
		for (std::map<std::string, std::string>::iterator itr=mHeadAdd.begin(); itr!= mHeadAdd.end(); itr++)
		{
			m_pIYfnet->SetHeadValue(szHash, itr->first.c_str(),  itr->second.c_str());
		}

		//m_pIYfnet->SetHeadValue(m_szUploadHash, "X-YF-hash", 
		m_pIYfnet->RunTask(szHash);

		for (int i=0; i<m_vItems.size(); i++)
		{
			if (m_vItems[i]->strHash == szHash)
			{
				m_vItems[i]->bHasCalHash = true;
				break;
			}
		}
	}
	return 0;
}

void CUploadPage::OnBnClickedStartUpload()
{
	char path[1024] = {0};
	GetDlgItemTextA(m_hWnd, IDC_UPLOAD_PATH, path, 1024);

	if (m_pIYfnet && strlen(path)>0)
	{
		vector<string> vFilePaths;
		QvodGetFilesInFolder(path, vFilePaths);

		if (vFilePaths.size() > 0)
		{
			QLock::AutoCriticalSection lock(m_fileLock);
			
			strcat(path, "/");
			for (int i=0; i<vFilePaths.size(); i++)
			{
				CreateUploadTask(path+vFilePaths[i]);
			}
		}
		else
		{
			QLock::AutoCriticalSection lock(m_fileLock);
			CreateUploadTask(path);
		}
		m_btnPauseResume.EnableWindow(TRUE);
		SetTimer(2, 1000, NULL);
	}
}

void CUploadPage::UpdateUrlData(const string strInfo)
{
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(strInfo, root) &&
		root.isObject() && root.isMember("hash") &&
		root["hash"].isString())
	{
		string strHash = root["hash"].asString();
		if (root.isMember("data") && root["data"].isObject())
		{
			Json::Value data = root["data"];
			if (data.isMember("ok") && data["ok"].isBool())
			{
				if (data["ok"].asBool())
				{
					data = data["data"];
					if (data.isObject() && data.isMember("url") && data["url"].isString())
					{
						string strUrl =  data["url"].asString();
						for (int i=0; i<m_vItems.size(); i++)
						{
							if (m_vItems[i]->strHash == strHash)
							{
								m_vItems[i]->strUrl = strUrl;
								m_vItems[i]->strRet = strInfo;
							}
						}
					}
				}
			}
		}
	}
}

void CUploadPage::UpdateData()
{
	m_listCtrl.SetRedraw(FALSE);
	
	wchar_t szTemp[20];
	for (int i=0; i<m_listCtrl.GetItemCount(); i++)
	{
		PItemData pInfo = (PItemData)m_listCtrl.GetItemData(i);
		m_listCtrl.SetItemText(i, 0,  Fun::ANSIToUnicode(pInfo->strHash));
		m_listCtrl.SetItemText(i, 1,  Fun::ANSIToUnicode(pInfo->strFileName));

		_itot(pInfo->iFileSize, szTemp, 10);
		CString strInfo = szTemp;
		m_listCtrl.SetItemText(i, 2,  strInfo);
		_itot(pInfo->iUpSize, szTemp, 10);
		strInfo = szTemp;
		m_listCtrl.SetItemText(i, 3,  strInfo);

		_itot(pInfo->iUpSpeed/1024, szTemp, 10);
		strInfo = szTemp;
		strInfo += _T("KB/S");
		m_listCtrl.SetItemText(i, 4,  strInfo);

		strInfo = _T("0%");
		if (pInfo->iFileSize)
		{
			_itot(pInfo->iUpSize*100.0/pInfo->iFileSize, szTemp, 10);
			strInfo = szTemp;
			strInfo += _T("%");
		}
		m_listCtrl.SetItemText(i, 5,  strInfo);

		strInfo = 1==pInfo->nStatus? _T("正在上传"): (3==pInfo->nStatus)?_T("上传成功"): _T("暂停");
		if (! pInfo->bHasCalHash)
		{
			strInfo = _T("正在计算文件hash");
		}
		m_listCtrl.SetItemText(i, 6,  strInfo);
	}
	m_listCtrl.SetRedraw(TRUE);
}
void CUploadPage::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnTimer(nIDEvent);

	switch (nIDEvent)
	{
	case 2:
		{
			if (NULL == m_pIYfnet)
			{
				return ;
			}
			for (int i=0; i<m_vItems.size(); i++)
			{
				STaskInfo info;
				m_pIYfnet->QueryTaskInfo(m_vItems[i]->strHash.c_str(), &info);
				float fPercent = info.iUploadLen*1.0/info.iFileLen;
				m_vItems[i]->iUpSize = info.iUploadLen;
				m_vItems[i]->iFileSize = info.iFileLen;
				m_vItems[i]->iUpSpeed = info.iUploadSpeed;
				m_vItems[i]->nStatus = info.status;
			}
			UpdateData();
		}
		break;
	}
}


BOOL CUploadPage::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}


void CUploadPage::OnBnClickedBtnPause()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_pIYfnet)
	{
		for (int i=0; i<m_vItems.size(); i++)
		{
			if (m_bPause)
			{
				m_pIYfnet->PauseTask(m_vItems[i]->strHash.c_str());
			}
			else
			{
				m_pIYfnet->RunTask(m_vItems[i]->strHash.c_str());	
			}
		}
		m_btnPauseResume.SetWindowText(m_bPause? _T("恢复"): _T("暂停"));
		m_bPause = !m_bPause;
	}
}


void CUploadPage::OnCbnSelchangeComboLimitMode()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_LIMIT_MODE);
	if (pBox && m_pIYfnet)
	{
		int nSel = pBox->GetCurSel();
		m_pIYfnet->SetLimitUploadSpeedMode((ELimitUploadSpeedMode)nSel);
	}
}


void CUploadPage::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bExit = TRUE;
	WaitForSingleObject(m_hThread, INFINITE);
	CDialog::OnClose();
}


void CUploadPage::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::vector<string> filelist;
	int DropCount=DragQueryFile(hDropInfo,-1,NULL,0);//取得被拖动文件的数目
	for(int i=0;i< DropCount;i++)
	{
		char wcStr[MAX_PATH];
		DragQueryFileA(hDropInfo,i,wcStr,MAX_PATH);//获得拖曳的第i个文件的文件名
		filelist.push_back(wcStr);
	} 
	DragFinish(hDropInfo);  //拖放结束后,释放内存

	if (filelist.size() > 0)
	{
		QLock::AutoCriticalSection lock(m_fileLock);
		for (int i=0; i<filelist.size(); i++)
		{
			CreateUploadTask(filelist[i]);
		}

		m_btnPauseResume.EnableWindow(TRUE);
		SetTimer(2, 1000, NULL);
	}
	CDialog::OnDropFiles(hDropInfo);
}


void CUploadPage::OnLvnItemchangedListUpfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CUploadPage::CopyToClipBoard(string url)
{
	if(OpenClipboard())
	{
		if (EmptyClipboard())
		{
			HGLOBAL clipbuffer   =   GlobalAlloc(GMEM_FIXED,   url.length()+1);
			char * buffer   =   (char*)GlobalLock(clipbuffer);
			memset(buffer, 0x0, url.length()+1);
			strcpy(buffer,   url.c_str());

			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT,clipbuffer);
		}
		CloseClipboard();
	}
}

void CUploadPage::OnNMRClickListUpfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (pNMItemActivate->iItem >=0)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CMenu popMenu;
		popMenu.LoadMenu(IDR_MENU1);
		CMenu *pMenu = popMenu.GetSubMenu(0);
		int cmd = pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,pt.x+10,pt.y+10,this);

		switch (cmd)
		{
		case ID_UPPAGE_COPY_URL:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				if (pData)
				{
					CopyToClipBoard(pData->strUrl);
				}
			}
			break;
		case ID_UPPAGE_COPYALL:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				if (pData)
				{
					CopyToClipBoard(pData->strRet);
				}
			}
			break;
		case ID_UPPAGE_DEL:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				m_listCtrl.DeleteItem(pNMItemActivate->iItem);

				for (int i=0; i<m_vItems.size(); i++)
				{
					if (m_vItems[i] == pData && m_pIYfnet)
					{
						m_pIYfnet->DeleteTask(pData->strHash.c_str());
						m_vItems.erase(m_vItems.begin()+i);
						break;
					}
				}
			}
			break;
		default:
			break;
		}
	}
	*pResult = 0;
}


void CUploadPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_listCtrl.m_hWnd)
	{
		m_listCtrl.SetWindowPos(this, 0, 0, cx, cy-10, SWP_NOZORDER|SWP_NOMOVE);
	}
	// TODO: 在此处添加消息处理程序代码
}
