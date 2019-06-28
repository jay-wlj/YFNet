
// YFNetAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YFNetApp.h"
#include "YFNetAppDlg.h"
#include "afxdialogex.h"

#include "function.h"
#include "sha1.h"
#include "share/StringCode.h"
#include "openssl/hmac.h"

#include <string>
using std::string;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CYFNetAppDlg 对话框



static CYFNetAppDlg *pThis = NULL;
CYFNetAppDlg::CYFNetAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYFNetAppDlg::IDD, pParent)
{
	pThis = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pIYfnet = NULL;
	m_lPointer = 0;
}

void CYFNetAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ID_INIT, m_initBtn);
	DDX_Control(pDX, ID_UNINIT, m_uninitBtn);
}

BEGIN_MESSAGE_MAP(CYFNetAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_INIT, &CYFNetAppDlg::OnBnClickedInit)
	ON_BN_CLICKED(ID_UNINIT, &CYFNetAppDlg::OnBnClickedUninit)

	ON_WM_TIMER()
	ON_BN_CLICKED(ID_START_UPLOAD, &CYFNetAppDlg::OnBnClickedStartUpload)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CYFNetAppDlg::OnBnClickedBtnLogin)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(ID_PAGE_UPLOAD, &CYFNetAppDlg::OnBnClickedPageUpload)
	ON_BN_CLICKED(ID_PAGE_DOWNLOAD, &CYFNetAppDlg::OnBnClickedPageDownload)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CYFNetAppDlg 消息处理程序
string Sign(const std::string& strHttpUrl, const DWORD& dwExpires, const string& strAccessKeyID, const string& strAccessSecret)
{
	string strSignUrl = strHttpUrl;

	string strMethod = "GET";
	string strHost;				// 域名
	string strUrl;					// 相对路径
	string strParams;			// 参数

	const char *p = strstr(strHttpUrl.c_str(), "://");
	if (NULL == p)
	{
		return strSignUrl;
	}
	p += strlen("://");
	const char *p2 = strstr(p, "/");
	if (NULL == p2)
	{
		strHost = p;
	}
	else
	{
		strHost.assign(p, p2-p);
		p = p2;
		p2 = strstr(p, "?");
		if (NULL != p2)
		{
			strParams = p2+1;
			strUrl.assign(p, p2-p);			
		}
		else
		{
			strUrl = p;
		}
	}

	p = strstr(strHost.c_str(), ".");
	if (NULL == p)
	{
		return strSignUrl;
	}
	string strCanoicalizedResource = "/";		
	strCanoicalizedResource += strHost.substr(0, p-strHost.c_str());
	strCanoicalizedResource += strUrl;

	char szExpires[50] = {0};
	sprintf(szExpires, "%d", dwExpires);

	string strSign = strMethod;
	strSign += "\n";
	strSign += "\n";	// content-md5
	strSign += "\n";	// content-type
	strSign += szExpires;
	strSign += "\n";
	strSign += strCanoicalizedResource;

	HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, strAccessSecret.c_str(), strAccessSecret.length(), EVP_sha1(), NULL);
	HMAC_Update(&ctx, (unsigned char*)strSign.c_str(), strSign.length());        // input is OK; &input is WRONG !!!

	unsigned char output[EVP_MAX_MD_SIZE] = {0};  
	unsigned int output_length = 0;
    HMAC_Final(&ctx, output, &output_length);
    HMAC_CTX_cleanup(&ctx);
	string strHmacSha1 = Base64::base64_encode((const char*)output, output_length);
	strHmacSha1 = Fun::URLEncode(strHmacSha1.c_str(), strHmacSha1.length());

	strSignUrl += strParams.empty()? "?": "&";
	strSignUrl += "OSSAccessKeyId=";
	strSignUrl += strAccessKeyID;
	strSignUrl += "&Signature=";
	strSignUrl += strHmacSha1;
	strSignUrl += "&Expires=";
	strSignUrl += szExpires;

	return strSignUrl;
}


typedef BOOL  (WINAPI *PChangeWindowMessageFilter)(__in UINT message, __in DWORD dwFlag);

BOOL CYFNetAppDlg::OnInitDialog()
{
	HMODULE hModule = LoadLibraryA("user32.dll");
	if (hModule)
	{
		PChangeWindowMessageFilter pChangeWindowMessageFilter = (PChangeWindowMessageFilter)GetProcAddress(hModule, "ChangeWindowMessageFilter");
		if (pChangeWindowMessageFilter)
		{
			pChangeWindowMessageFilter(WM_DROPFILES, 1);
			pChangeWindowMessageFilter(WM_COPYDATA, 1);
			pChangeWindowMessageFilter(0x0049, 1);			
		}
	}


	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	m_pageDownload.Create(IDD_DLG_DOWNLOAD, this);
	m_pageUpload.Create(IDD_DLG_UPLOAD, this);

	CRect rect;
	GetWindowRect(&rect);
	rect.top += 80;
	rect.bottom -= 50;
	m_pageUpload.MoveWindow(&rect);
	//rect.bottom = rect.top + 100;
	m_pageDownload.MoveWindow(&rect);

	m_pageDownload.ShowWindow(SW_SHOW);

	OnBnClickedInit();
	string str = Sign("http://vodpub.oss-cn-hangzhou.aliyuncs.com/360p/16_9/e40f/847e/e40f847e08aeacfe5b219945c41ee498.m3u8", GetTickCount()+3600*24*30, "LTAIv4nNqW4CB9ij", "0JEbPTrEQ0Vqw4sFyiYikunvWNkHIC");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CYFNetAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CYFNetAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CYFNetAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CYFNetAppDlg::PutString(CString& str)
{
}

void CYFNetAppDlg::EventCallBackFunc(SCallBackPara& para)
{
	switch (para.id)
	{
	case 1:
		{
			string strHash(para.buf, 40);
			CString strInfo;
			strInfo.Format(_T("任务 %s 下载完毕!"), Fun::ANSIToUnicode(strHash.c_str()));
			//m_listBox.InsertString(-1, strInfo);
		}
		break;
	case 3:
		{
			string strHash(para.buf, 40);
			CString strInfo;
			strInfo.Format(_T("任务 %s 下载失败，err=%d!"), Fun::ANSIToUnicode(strHash.c_str()), para.result);
			//m_listBox.InsertString(-1, strInfo);
		}
		break;
	case 4:
		{
			CString strInfo;
			strInfo.AppendFormat(_T("代理服务器端口:%d"), para.result);
			
			SetDlgItemText(IDC_STATIC_PORT, strInfo);
			//m_listBox.InsertString(-1, strInfo);
		}
		break;
	case 8:
		{
			string str(para.buf, para.buflen);

			if (0 == para.result)
			{
				m_pageUpload.UpdateUrlData(str);
/*				CString strInfo;
				strInfo.AppendFormat(_T("任务上传完成: %s"), Fun::ANSIToUnicode(str));
				m_listBox.InsertString(-1, strInfo);	*/		
			}
		}
		break;
	}
}
void EventCallBackFuncEx(SCallBackPara& para)
{
	if (pThis)
	{
		pThis->EventCallBackFunc(para);
	}
}

#include <vector>
using std::vector;

void CYFNetAppDlg::OnBnClickedInit()
{
	// TODO: 在此添加控件通知处理程序代码
	InitPara para;
	para.bUpgradeProc = false;
	para.callback = EventCallBackFuncEx;
	para.pConfigPath = "C:/NFConfig";
	para.pTempCachePath = "C:/NFConfig.cache";
	para.pDownloadPath = "C:/NFConfig.down";
	para.maxspace = 600;

	char szExePath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szExePath, MAX_PATH);
	PathRemoveFileSpecA(szExePath);
	strcat(szExePath, "/YfNetPlush.dll");

	m_hModule = ::LoadLibraryA(szExePath);
	PInitYfNet pInitYfNet = (PInitYfNet)GetProcAddress(m_hModule, "_InitYfNet");
	if (pInitYfNet)
	{
		if (E_NET_OK != pInitYfNet(&m_pIYfnet, para, m_lPointer))
		{
			MessageBox(_T("初始化失败"));
		}
		m_initBtn.EnableWindow(FALSE);
		m_uninitBtn.EnableWindow(TRUE);
	}
	m_pIYfnet->SetConfig("{\"data\":{\"maxdowncount\":1,\"commonstat\":{\"pid\":\"lp\",\"mi\":\"mi\",\"did\":\"did\",\"pv\":\"1.6.19\"},\"alisign\" : {\"LTAIv4nNqW4CB9ij\" : \"0JEbPTrEQ0Vqw4sFyiYikunvWNkHIC\"}}}");
}


void CYFNetAppDlg::OnBnClickedUninit()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL == m_hModule)
	{
		return;
	}
	m_pIYfnet = NULL;
	PClearYfNet pClear = (PClearYfNet)GetProcAddress(m_hModule, "_ClearYfNet");
	if (pClear)
	{
		pClear(m_lPointer);
	}
	FreeLibrary(m_hModule);
	m_hModule = NULL;

	m_initBtn.EnableWindow(TRUE);
	m_uninitBtn.EnableWindow(FALSE);
}


void CYFNetAppDlg::OnLbnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码

}



void CYFNetAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnTimer(nIDEvent);


}

//文件分块大小范围  picsize>=16KB && picsize<= 4MB  picsize=2^n
unsigned int CalcPieceSize(INT64 filesize)
{
	if (filesize <= 2097152)		 // 大小小于2M 默认分块大小为16KB
		return 16384;
	float f = sqrt((float)filesize/4096);			// 除以4K的平方根,以下算法为 2^n <= sqrt(filesize/4096), 其中2^n为最大块大小
	unsigned short n = (unsigned short)f;
	int i = 3;
	while(n && i<16)										
	{
		if( n & (0x8000>>i))
			return (0x8000 >> i)*1024;
		i++;
	}
	return 16384;											// (文件大小大于256GB?),块大小超过4M,则默认分块大小为16KB
}







void CYFNetAppDlg::OnBnClickedStartUpload()
{
	// TODO: 在此添加控件通知处理程序代码

}



void CYFNetAppDlg::OnBnClickedBtnLogin()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CYFNetAppDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnDropFiles(hDropInfo);
}



void CYFNetAppDlg::OnBnClickedPageUpload()
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_pageUpload.ShowWindow(SW_SHOWNORMAL);
	m_pageDownload.ShowWindow(SW_HIDE);
}


void CYFNetAppDlg::OnBnClickedPageDownload()
{
	m_pageUpload.ShowWindow(SW_HIDE);
	m_pageDownload.ShowWindow(SW_SHOWNORMAL);
	m_pageDownload.Invalidate();
	m_pageDownload.UpdateWindow();
	// TODO: 在此添加控件通知处理程序代码
}


void CYFNetAppDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_pageDownload.m_hWnd && m_pageDownload.m_hWnd)
	{
		m_pageUpload.SetWindowPos(NULL, 0, 0, cx-1, cy-100, SWP_NOMOVE|SWP_NOZORDER);
		m_pageDownload.SetWindowPos(NULL, 0, 0, cx-1, cy-100, SWP_NOMOVE|SWP_NOZORDER);
	}

	// TODO: 在此处添加消息处理程序代码
}


void CYFNetAppDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedUninit();
	CDialogEx::OnClose();
}
