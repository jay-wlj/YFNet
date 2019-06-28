
// YFNetAppDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CYFNetAppDlg �Ի���



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


// CYFNetAppDlg ��Ϣ�������
string Sign(const std::string& strHttpUrl, const DWORD& dwExpires, const string& strAccessKeyID, const string& strAccessSecret)
{
	string strSignUrl = strHttpUrl;

	string strMethod = "GET";
	string strHost;				// ����
	string strUrl;					// ���·��
	string strParams;			// ����

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

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������


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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CYFNetAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
			strInfo.Format(_T("���� %s �������!"), Fun::ANSIToUnicode(strHash.c_str()));
			//m_listBox.InsertString(-1, strInfo);
		}
		break;
	case 3:
		{
			string strHash(para.buf, 40);
			CString strInfo;
			strInfo.Format(_T("���� %s ����ʧ�ܣ�err=%d!"), Fun::ANSIToUnicode(strHash.c_str()), para.result);
			//m_listBox.InsertString(-1, strInfo);
		}
		break;
	case 4:
		{
			CString strInfo;
			strInfo.AppendFormat(_T("����������˿�:%d"), para.result);
			
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
				strInfo.AppendFormat(_T("�����ϴ����: %s"), Fun::ANSIToUnicode(str));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			MessageBox(_T("��ʼ��ʧ��"));
		}
		m_initBtn.EnableWindow(FALSE);
		m_uninitBtn.EnableWindow(TRUE);
	}
	m_pIYfnet->SetConfig("{\"data\":{\"maxdowncount\":1,\"commonstat\":{\"pid\":\"lp\",\"mi\":\"mi\",\"did\":\"did\",\"pv\":\"1.6.19\"},\"alisign\" : {\"LTAIv4nNqW4CB9ij\" : \"0JEbPTrEQ0Vqw4sFyiYikunvWNkHIC\"}}}");
}


void CYFNetAppDlg::OnBnClickedUninit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}



void CYFNetAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialogEx::OnTimer(nIDEvent);


}

//�ļ��ֿ��С��Χ  picsize>=16KB && picsize<= 4MB  picsize=2^n
unsigned int CalcPieceSize(INT64 filesize)
{
	if (filesize <= 2097152)		 // ��СС��2M Ĭ�Ϸֿ��СΪ16KB
		return 16384;
	float f = sqrt((float)filesize/4096);			// ����4K��ƽ����,�����㷨Ϊ 2^n <= sqrt(filesize/4096), ����2^nΪ�����С
	unsigned short n = (unsigned short)f;
	int i = 3;
	while(n && i<16)										
	{
		if( n & (0x8000>>i))
			return (0x8000 >> i)*1024;
		i++;
	}
	return 16384;											// (�ļ���С����256GB?),���С����4M,��Ĭ�Ϸֿ��СΪ16KB
}







void CYFNetAppDlg::OnBnClickedStartUpload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}



void CYFNetAppDlg::OnBnClickedBtnLogin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CYFNetAppDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnDropFiles(hDropInfo);
}



void CYFNetAppDlg::OnBnClickedPageUpload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	m_pageUpload.ShowWindow(SW_SHOWNORMAL);
	m_pageDownload.ShowWindow(SW_HIDE);
}


void CYFNetAppDlg::OnBnClickedPageDownload()
{
	m_pageUpload.ShowWindow(SW_HIDE);
	m_pageDownload.ShowWindow(SW_SHOWNORMAL);
	m_pageDownload.Invalidate();
	m_pageDownload.UpdateWindow();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CYFNetAppDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_pageDownload.m_hWnd && m_pageDownload.m_hWnd)
	{
		m_pageUpload.SetWindowPos(NULL, 0, 0, cx-1, cy-100, SWP_NOMOVE|SWP_NOZORDER);
		m_pageDownload.SetWindowPos(NULL, 0, 0, cx-1, cy-100, SWP_NOMOVE|SWP_NOZORDER);
	}

	// TODO: �ڴ˴������Ϣ����������
}


void CYFNetAppDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	OnBnClickedUninit();
	CDialogEx::OnClose();
}
