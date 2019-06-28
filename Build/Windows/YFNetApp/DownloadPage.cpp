// DownloadPage.cpp : 实现文件
//

#include "stdafx.h"
#include "DownloadPage.h"
#include "afxdialogex.h"
#include "AESData.h"
#include "YFNetAppDlg.h"
#include "include\function.h"
#include "json\json.h"
#include "include\AutoPtr.h"

using namespace std;
// CDownloadPage 对话框

IMPLEMENT_DYNAMIC(CDownloadPage, CDialog)

CDownloadPage::CDownloadPage(CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadPage::IDD, pParent)
{

}

CDownloadPage::~CDownloadPage()
{
}

void CDownloadPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOWNLIST, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CDownloadPage, CDialog)
	ON_BN_CLICKED(ID_TASK_CREATE, &CDownloadPage::OnBnClickedTaskCreate)
	ON_BN_CLICKED(ID_TASK_DOWN, &CDownloadPage::OnBnClickedTaskDown)
	ON_BN_CLICKED(ID_AES_ENCRYPT, &CDownloadPage::OnBnClickedAesEncrypt)
	ON_BN_CLICKED(ID_AES_DECRYPT, &CDownloadPage::OnBnClickedAesDecrypt)
	ON_BN_CLICKED(ID_TASK_DELETE, &CDownloadPage::OnBnClickedTaskDelete)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_TASK_DOWN2, &CDownloadPage::OnBnClickedTaskDown2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CDownloadPage::OnLvnItemchangedList2)
	ON_WM_WINDOWPOSCHANGING()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DOWNLIST, &CDownloadPage::OnNMRClickListDownlist)
	ON_BN_CLICKED(ID_BTN_CACHEPATH, &CDownloadPage::OnBnClickedBtnCachepath)
	ON_BN_CLICKED(ID_TASK_UPDATEURL, &CDownloadPage::OnBnClickedTaskUpdateurl)
	ON_EN_CHANGE(IDC_LIMIT_DOWNCOUNT, &CDownloadPage::OnEnChangeLimitDowncount)
END_MESSAGE_MAP()


// CDownloadPage 消息处理程序


void CDownloadPage::OnBnClickedTaskCreate()
{
	char szUrl[1024] = {0};
	GetDlgItemTextA(m_hWnd, IDC_TASK_URL, szUrl, 1024);

	if (m_pIYfnet)
	{
		char szProxyUrl[200] = {0};
		m_pIYfnet->CreateTask(szUrl, szUrl, "", m_szHash);
		m_pIYfnet->SetTaskDownload(m_szHash, true, NULL);
		m_pIYfnet->RunTask(m_szHash);

		m_curtime = 0;
		//SetTimer(1, 1000, NULL);
	}
	// TODO: 在此添加控件通知处理程序代码
}


void CDownloadPage::OnBnClickedTaskDown()
{
	// TODO: 在此添加控件通知处理程序代码
	char szUrl[1024] = {0};
	CString strUrl;
	GetDlgItemText(IDC_TASK_URL, strUrl);

	string url = Fun::UnicodeToANSI(strUrl);
	char *buf = new char[url.length()+1]();
	memcpy(buf, url.c_str(), url.length());
	
	vector<string> vUrls;
	char *p = strtok(buf, " ");
	while (p)
	{
		vUrls.push_back(p);
		p = strtok(NULL, " "); 
	}

	if (m_pIYfnet)
	{
		for (int i=0; i<vUrls.size(); i++)
		{
			string szUrl = vUrls[i];

			char szHash[41] = {0};
			char szProxyUrl[200] = {0};
			m_pIYfnet->CreateTask(szUrl.c_str(), szUrl.c_str(), NULL, szHash, NULL);
			m_pIYfnet->SetTaskDownload(szHash, true, NULL);
			m_pIYfnet->SetTaskPlaying(m_szHash, true);
			int ret = m_pIYfnet->RunTask(szHash);


			int j =0;
			for (j=0; j<m_vItems.size(); j++)
			{
				if (m_vItems[j]->strHash == szHash)
				{
					m_vItems[j]->dwTickStart = (ret>=0)? GetTickCount(): 0;
					break;
				}
			}
			if (j >= m_vItems.size())
			{
				ItemData *pData = new ItemData;
				pData->strHash.assign(szHash, 40);
				pData->iUpSize = 0;
				pData->iUpSpeed = 0;
				pData->dwTickStart =(ret>=0)? GetTickCount(): 0;
				pData->strFileName = szUrl;
				m_vItems.push_back(pData);

				wchar_t szTemp[10];
				int idx = m_listCtrl.InsertItem(0, L"");
				m_listCtrl.SetItemData(idx, (DWORD_PTR)pData);
			}
		}
		UpdateData();
	}
}


void CDownloadPage::OnBnClickedAesEncrypt()
{
	// TODO: 在此添加控件通知处理程序代码
			char szFilePath[MAX_PATH] = {0};
	::GetDlgItemTextA(m_hWnd, IDC_AES_ENDECRYT, szFilePath, MAX_PATH);
	char szPath[MAX_PATH] = {0};
	strcpy(szPath, szFilePath);
	PathRemoveExtensionA(szPath);
	std::string strKeyFilePath = szPath;
	strKeyFilePath += ".key";

	strKeyFilePath = "F:/bin/5C8153EC59A92B8CA78DBA4D08341D8C1595274F.key";

	CAESData aes;

	string strKey = ReadAesKey(strKeyFilePath);
	aes.SetKey(strKey);

	string strDecpytPath = szPath;
	strDecpytPath += ".encrpyt";

	FILE *fin = fopen(strDecpytPath.c_str(), "wb");
	if (! fin)
	{
		return ;
	}
		
	FILE *fout = fopen(szFilePath, "rb");
	if (fout)
	{
		fseek(fout, 0, SEEK_END);
		long size = ftell(fout);
		unsigned nPicSize = 16384;//CalcPieceSize(size);

		unsigned char *buf = new unsigned char[nPicSize];

		fseek(fout, 0, SEEK_SET);
		int len = 0;
		while ((len=fread(buf, 1, nPicSize, fout)) == nPicSize)
		{
			aes.Encrpyt(buf, buf, nPicSize);

			fwrite(buf, 1, nPicSize, fin);
		}
		fwrite(buf, 1, len, fin);

		fclose(fin);
		fclose(fout);

		((CYFNetAppDlg*)GetParent())->PutString(CString(_T("数据加密完成")));

	}
}

void Char2Hash(const char* chars,unsigned char* hash, const unsigned int nLen)
{
	unsigned int i ,j=0;
	unsigned char c;

	const unsigned int len = nLen*2;
	if (chars && strlen(chars) < len)
	{
		return;
	}

	for (i=0;i<len;)
	{
		if('9'>= chars[i]&& chars[i]>= '0' )
			c = (chars[i] - '0')<< 4;
		else if('F'>= chars[i]&& chars[i]>= 'A' )
			c = (chars[i] - 'A'+10)<< 4;
		else if('f'>= chars[i]&& chars[i]>= 'a')
			c = (chars[i] - 'a'+10)<< 4;
		else
			return ;
		i++;
		if('9'>= chars[i]&& chars[i]>= '0' )
			c += (chars[i] - '0');
		else if('F'>= chars[i]&& chars[i]>= 'A' )
			c += (chars[i] - 'A'+10);
		else if('f'>= chars[i]&& chars[i]>= 'a')
			c += (chars[i] - 'a'+10);
		else
			return ;
		i++;
		hash[j]= c;
		j++;
	}
}


string CDownloadPage::ReadAesKey(string strKeyPath)
{
	bool bRet = false;
	string strKeyFile = strKeyPath;
	FILE *fp = fopen(strKeyFile.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0L, SEEK_END);
		long nFileLen = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		if (nFileLen > 0)
		{
			string strKey;
			int nVer = 0;

			bool bExistKey = false;
			QPtr::AutoArrayPtr<char> buf = new char[nFileLen];
			if (1 == fread(buf,nFileLen, 1, fp))
			{
				std::string strInfo(buf, nFileLen);
				Json::Value root;
				Json::Reader reader;
				if (reader.parse(strInfo, root) &&
					root.isObject() &&
					root.isMember("key") &&
					root["key"].isString())
				{
					string sKey = root["key"].asString();
					if (sKey.length() >= 32)	// key存在且合法
					{
						bExistKey = true;
						unsigned char key[16] = {0};
						Char2Hash(sKey.c_str(), key, 16);
						strKey.assign((const char*)key, 16);
					}
					if (root.isMember("ver") && root["ver"].isInt())
					{
						nVer = root["ver"].asInt();
					}
				}
			}
			return strKey;
		}

		fclose(fp);
	}
	return "";
}
void CDownloadPage::OnBnClickedAesDecrypt()
{
	// TODO: 在此添加控件通知处理程序代码
		char szFilePath[MAX_PATH] = {0};
	::GetDlgItemTextA(m_hWnd, IDC_AES_ENDECRYT, szFilePath, MAX_PATH);
	char szPath[MAX_PATH] = {0};
	strcpy(szPath, szFilePath);
	PathRemoveExtensionA(szPath);
	std::string strKeyFilePath = szPath;
	strKeyFilePath += ".key";

	strKeyFilePath = "F:/bin/5C8153EC59A92B8CA78DBA4D08341D8C1595274F.key";

	CAESData aes;

	string strKey = ReadAesKey(strKeyFilePath);
	aes.SetKey(strKey);

	string strDecpytPath = szPath;
	strDecpytPath += ".decrpyt";

	FILE *fin = fopen(strDecpytPath.c_str(), "wb");
	if (! fin)
	{
		return ;
	}
		
	FILE *fout = fopen(szFilePath, "rb");
	if (fout)
	{
		fseek(fout, 0, SEEK_END);
		long size = ftell(fout);
		unsigned nPicSize = 16384;//CalcPieceSize(size);

		unsigned char *buf = new unsigned char[nPicSize];

		fseek(fout, 0, SEEK_SET);
		int len = 0;
		while ((len=fread(buf, 1, nPicSize, fout)) == nPicSize)
		{
			aes.Decrpyt(buf, buf, nPicSize);

			fwrite(buf, 1, nPicSize, fin);
		}
		fwrite(buf, 1, len, fin);

		fclose(fin);
		fclose(fout);

		((CYFNetAppDlg*)GetParent())->PutString(CString(_T("数据解密完成")));

	}
}


void CDownloadPage::OnBnClickedTaskDelete()
{
	char szPath[500] = {0};
	ItemData *pData = (ItemData*)m_listCtrl.GetItemData(0);
	if (pData && m_pIYfnet)
	{
		int len = 500;
		m_pIYfnet->GetTaskInfo(pData->strHash.c_str(), szPath, len);
	}
	//if (m_pIYfnet)
	//{
	//	m_pIYfnet->DeleteTask(m_szHash);
	//}
	// TODO: 在此添加控件通知处理程序代码
}


void CDownloadPage::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		{
			if (m_pIYfnet)
			{
				m_curtime ++;
				int nCacheTime = 0;
				m_pIYfnet->SetPlayingTimepoint(m_szHash, m_curtime*1000, false, nCacheTime);
			}
		}
		break;
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
				m_vItems[i]->iDownSize = info.iDownloadLen;
				m_vItems[i]->iDownSpeed = info.iDownloadSpeed;
				m_vItems[i]->iUpSpeed = info.iUploadSpeed;
				m_vItems[i]->strFilePath = info.szCopyPath;
			}
			UpdateData();
		}
		break;
		}
	CDialog::OnTimer(nIDEvent);
}


void CDownloadPage::OnBnClickedTaskDown2()
{
	char szUrl[1024] = {0};
	CString strUrl;
	GetDlgItemText(IDC_TASK_URL, strUrl);

	string url = Fun::UnicodeToANSI(strUrl);
	char *buf = new char[url.length()+1]();
	memcpy(buf, url.c_str(), url.length());
	
	vector<string> vUrls;
	char *p = strtok(buf, " ");
	while (p)
	{
		vUrls.push_back(p);
		p = strtok(NULL, " "); 
	}

	if (m_pIYfnet)
	{
		for (int i=0; i<vUrls.size(); i++)
		{
			string szUrl = vUrls[i];

			char szHash[41] = {0};
			char szProxyUrl[200] = {0};
			m_pIYfnet->CreateTask(szUrl.c_str(), szUrl.c_str(), NULL, szHash, NULL, true);
			m_pIYfnet->SetTaskDownload(szHash, true, NULL);
			m_pIYfnet->SetTaskPlaying(m_szHash, true);
			int ret = m_pIYfnet->RunTask(szHash);


			int j =0;
			for (j=0; j<m_vItems.size(); j++)
			{
				if (m_vItems[j]->strHash == szHash)
				{
					m_vItems[j]->dwTickStart =(ret>=0)? GetTickCount(): 0;
					break;
				}
			}
			if (j >= m_vItems.size())
			{
				ItemData *pData = new ItemData;
				pData->strHash.assign(szHash, 40);
				pData->strFileName = szUrl;
				pData->iUpSize = 0;
				pData->iUpSpeed = 0;
				pData->dwTickStart =(ret>=0)? GetTickCount(): 0;
				m_vItems.push_back(pData);

				wchar_t szTemp[10];
				int idx = m_listCtrl.InsertItem(0, L"");
				m_listCtrl.SetItemData(idx, (DWORD_PTR)pData);
			}
		}
		UpdateData();
	}
}


void CDownloadPage::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


BOOL CDownloadPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT);

	// TODO:  在此添加额外的初始化
	m_listCtrl.InsertColumn(0, _T("任务hash"), 0, 100);
	m_listCtrl.InsertColumn(1, _T("文件名"), 0, 100);
	m_listCtrl.InsertColumn(2, _T("文件大小"), 0, 100);
	m_listCtrl.InsertColumn(3, _T("下载大小"), 0, 100);
	m_listCtrl.InsertColumn(4, _T("下载速度"), 0, 100);
	m_listCtrl.InsertColumn(5, _T("完成百分比"), 0, 100);
	m_listCtrl.InsertColumn(6, _T("用时"), 0, 100);

	// TODO:  在此添加额外的初始化
	SetTimer(2, 1000, NULL);
	SetDlgItemText(IDC_LIMIT_DOWNCOUNT, _T("1"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDownloadPage::UpdateData()
{
	m_listCtrl.SetRedraw(FALSE);
	
	wchar_t szTemp[20];
	for (int i=0; i<m_listCtrl.GetItemCount(); i++)
	{
		PItemData pInfo = (PItemData)m_listCtrl.GetItemData(i);
		m_listCtrl.SetItemText(i, 0,  Fun::ANSIToUnicode(pInfo->strHash));
		m_listCtrl.SetItemText(i, 1,  Fun::ANSIToUnicode(pInfo->strFileName));

		_i64tot(pInfo->iFileSize, szTemp, 10);
		CString strInfo = szTemp;
		m_listCtrl.SetItemText(i, 2,  strInfo);
		_i64tot(pInfo->iDownSize, szTemp, 10);
		strInfo = szTemp;
		m_listCtrl.SetItemText(i, 3,  strInfo);

		_itot(pInfo->iDownSpeed/1024, szTemp, 10);
		strInfo = szTemp;
		strInfo += _T("KB/S");
		m_listCtrl.SetItemText(i, 4,  strInfo);

		strInfo = _T("0%");
		if (pInfo->iFileSize)
		{
			_itot(pInfo->iDownSize*100.0/pInfo->iFileSize, szTemp, 10);
			strInfo = szTemp;
			strInfo += _T("%");
		}
		m_listCtrl.SetItemText(i, 5,  strInfo);

		DWORD dwTick = pInfo->nTick;
		if (pInfo->dwTickStart>0)
		{
			dwTick += (GetTickCount()-pInfo->dwTickStart)/1000;
			if (pInfo->iDownSize>0 && pInfo->iFileSize>0 && pInfo->iDownSize >= pInfo->iFileSize)	// 完成下载
			{
				pInfo->nTick = dwTick;
				pInfo->dwTickStart = 0;
			}
		}
		strInfo.Format(_T("%02d:%02d:%02d"), (dwTick/3600)%60, (dwTick/60)%60, dwTick%60);
		m_listCtrl.SetItemText(i, 6,  strInfo);
	}
	m_listCtrl.SetRedraw(TRUE);
}

void CDownloadPage::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);
	if(lpwndpos && !(lpwndpos->flags & SWP_NOSIZE))
	{
		m_listCtrl.MoveWindow(0, 110,  lpwndpos->cx, lpwndpos->cy);
	}
	// TODO: 在此处添加消息处理程序代码
}


void CDownloadPage::OnNMRClickListDownlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (pNMItemActivate->iItem >=0)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CMenu popMenu;
		popMenu.LoadMenu(IDR_MENU1);
		CMenu *pMenu = popMenu.GetSubMenu(1);
		int cmd = pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,pt.x+10,pt.y+10,this);

		switch (cmd)
		{
		case ID_DOWNPAGE_RUN:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				if (pData)
				{
					if (m_pIYfnet)
					{
						if (m_pIYfnet->RunTask(pData->strHash.c_str())>=0)
						{
							pData->dwTickStart = GetTickCount();
						}
					}
				}
			}
			break;
		case ID_DOWNPAGE_PAUSE:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				if (pData)
				{
					if (m_pIYfnet)
					{
						if (m_pIYfnet->PauseTask(pData->strHash.c_str()) >=0)
						{
							pData->nTick = pData->nTick+(GetTickCount()-pData->dwTickStart)/1000;
							pData->dwTickStart = 0;
						}
					}
				}
			}
			break;
		case ID_DOWNPAGE_DOWN:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				if (pData && m_pIYfnet)
				{
					m_pIYfnet->SetTaskDownload(pData->strHash.c_str(), true, "");
				}
			}
			break;
		case ID_DOWNPAGE_UNSETDOWN:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				if (pData && m_pIYfnet)
				{
					m_pIYfnet->SetTaskDownload(pData->strHash.c_str(), false, "");
				}
			}
			break;
		case ID_DOWNPAGE_DELETE:
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
		case ID_DOWNPAGE_OPENFILE:
			{
				ItemData *pData = (ItemData*)m_listCtrl.GetItemData(pNMItemActivate->iItem);
				if (pData && m_pIYfnet && !pData->strFilePath.empty())
				{
					string path = pData->strFilePath;
					int i = 0;
					while ((i = path.find('/')) != string::npos)
					{
						path[i] = '\\';
					}

					char szPath[MAX_PATH] = {0};
					strcpy(szPath,  path.c_str());
					PathRemoveFileSpecA(szPath);
					path = szPath;
					CoInitialize(NULL);
					ITEMIDLIST* pidl = ILCreateFromPathA(path.c_str());
					if(pidl)
						SHOpenFolderAndSelectItems(pidl,0,0,0);    
					CoUninitialize();

					//m_pIYfnet->SetTaskDownload(pData->strHash.c_str(), true, "");
				}
			}
			break;
		default:
			break;
		}
	}
	*pResult = 0;
}


void CDownloadPage::OnBnClickedBtnCachepath()
{
	// TODO: 在此添加控件通知处理程序代码
	int err = 0;
	char szTemp[1024] = {0};
	GetDlgItemTextA(m_hWnd, IDC_CACHEPATH_EDIT, szTemp, 1024);
	if (m_pIYfnet)
	{
		//err = m_pIYfnet->SetTempCachePath(szTempPath);
		m_pIYfnet->SetConfig(szTemp);
	}
}


void CDownloadPage::OnBnClickedTaskUpdateurl()
{
	// TODO: 在此添加控件通知处理程序代码
	char szUpdateUrl[MAX_PATH] = {0};
	GetDlgItemTextA(m_hWnd, IDC_TASK_URL, szUpdateUrl, MAX_PATH);
	int nItem = m_listCtrl.GetSelectionMark();
	nItem = nItem < 0 ? 0: nItem;

	ItemData *pData = (ItemData*)m_listCtrl.GetItemData(nItem);
	if (pData && m_pIYfnet)
	{
		m_pIYfnet->UpdateClarityUrl(pData->strHash.c_str(), szUpdateUrl, szUpdateUrl, 2);
	}
}


void CDownloadPage::OnEnChangeLimitDowncount()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strText;
	GetDlgItemText(IDC_LIMIT_DOWNCOUNT, strText);
	int nCount = _ttoi(strText);

	if (nCount>64)
	{
		MessageBox(_T("已超同时连接上限64"));
		return;
	}
	char szChar[2000] = {0};
	sprintf(szChar, "{\"data\": {\"concurrentnum\": %d}}", nCount);
	if (m_pIYfnet)
	{
		m_pIYfnet->SetConfig(szChar);
	}
}
