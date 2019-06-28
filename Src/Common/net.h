#ifndef COMMON_NET_H
#define COMMON_NET_H

#include <string>
#include <atlstr.h>

#include <WinInet.h>
#pragma comment(lib,"Wininet.lib")

#include "stringtool.h"
#include "version.h"
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "..\include\INetUtil.h"

#ifdef _DEBUG
	#pragma comment(lib, "../../lib/NetUtil_d.lib")
#else
	#pragma comment(lib, "../../lib/NetUtil.lib")
#endif

#include <sstream>

namespace net{

enum PostType
{
	kDefaultPost,
	kJsonPost,
	kQrcodePost,
	kSaoTuPost,
};
struct HttpRequestInfo 
{
	static CString m_sDefaultUserAgent;
	HTTP_REQUEST_INFO reqInfo;
	HttpRequestInfo(CString url,
		            int timeout):m_url(url)
	{
		m_userAgent = m_sDefaultUserAgent;
		reqInfo.szURL = m_url;
		reqInfo.optional.szUserAgent = m_userAgent;
		reqInfo.optional.nTimeout = timeout;
	}
	HttpRequestInfo(CString url,
					CString lastModify,
					int timeout)
					:m_url(url), m_lastModify(lastModify)
	{
		m_userAgent = m_sDefaultUserAgent;
		reqInfo.szURL = m_url;
		reqInfo.optional.szUserAgent = m_userAgent;
		reqInfo.optional.nTimeout = timeout;
		if(m_lastModify.GetLength() > 0)
		{
			reqInfo.optional.szLastModify = m_lastModify;
		}
	}
	HttpRequestInfo(CString url,
					std::string postdata,
					int timeout,
					net::PostType pType = kDefaultPost) 
				    : m_url(url),m_postData(postdata)
	{
		m_userAgent = m_sDefaultUserAgent;
		reqInfo.szURL = m_url;
		reqInfo.optional.szUserAgent = m_userAgent;
		reqInfo.optional.nTimeout = timeout;
		reqInfo.optional.requestType = Request_Post_Sync;
		reqInfo.optional.pPostData = (char*)m_postData.c_str();
		
		switch (pType)
		{
		case kJsonPost:
			reqInfo.optional.pPostType = "application/json; charset=utf-8";
			break;
		case kSaoTuPost:
			reqInfo.optional.pPostType = "multipart/form-data; boundary=----WebKitFormBoundaryb7FreUw40w5\r\n";
			break;
		case kQrcodePost:
			reqInfo.optional.pPostType = "application/x-www-form-urlencoded; charset=UTF-8";
			break;
		default:
			break;
		}
		reqInfo.optional.dwPostSize = postdata.size();
	}
private:
	std::string m_postData;
	CString m_url;
	CString m_lastModify;
	CString m_userAgent;
};
__declspec(selectany) CString HttpRequestInfo::m_sDefaultUserAgent;
// 对IHttpUtil进行封装
class HttpDownloader
{
public:
	HttpDownloader(CString url,
				   int timeout = 10000
				   )
	{
		m_pHttpNet = NULL;
		NetUtil_CreateHttp(&m_pHttpNet);
		if(m_pHttpNet)
		{
			HttpRequestInfo reqInfo(url,timeout);
			m_pHttpNet->SetRequestInfo(&reqInfo.reqInfo);
		}
	}
	HttpDownloader(CString url, 
				   CString lastModify,
				   int timeout = 10000
				   )
	{
		m_pHttpNet = NULL;
		NetUtil_CreateHttp(&m_pHttpNet);
		if(m_pHttpNet)
		{
			HttpRequestInfo reqInfo(url,lastModify,timeout);
			m_pHttpNet->SetRequestInfo(&reqInfo.reqInfo);
		}
	}
	HttpDownloader(CString url,
				   std::string postdata,
				   int timeout = 10000,
				   net::PostType pType = net::kDefaultPost
		           )
	{
		m_pHttpNet = NULL;
		NetUtil_CreateHttp(&m_pHttpNet);
		if(m_pHttpNet)
		{
			HttpRequestInfo reqInfo(url, postdata,timeout, pType);
			m_pHttpNet->SetRequestInfo(&reqInfo.reqInfo);
		}
	}
	~HttpDownloader()
	{
		Abort();
		NetUtil_DestroyHttp(m_pHttpNet);
	}
	bool Request(std::string& outBuffer)
	{
		if(NULL == m_pHttpNet) return false;
		NetError error = m_pHttpNet->Request();
		if(error == NetError_OK)
		{
			LONGLONG len = m_pHttpNet->GetTotalSize();
			if(len > 0)
			{
				char* pBody = new char[len+1];
				m_pHttpNet->GetReceiveData(pBody, len);
				outBuffer = std::string(pBody,len);
				delete[]pBody;
				return true;
			}
		}
		return false;
	}
	bool Request()
	{
		if(NULL == m_pHttpNet) return false;
		return  NetError_OK == m_pHttpNet->Request();
	}
	bool DownloadFile(const CString& saveFile)
	{
		if(NULL == m_pHttpNet) return false;
		NetError error = m_pHttpNet->Download(Convert::ToWString(saveFile));
		if(error == NetError_OK)
		{
			return true;
		}
		return false;
	}
	CString GetLastModify()
	{
		CString lastModify;
		if(m_pHttpNet)
		{
			lastModify = m_pHttpNet->GetLastModify();
		}
		return lastModify;
	}
	void Abort()
	{
		if(m_pHttpNet)
		{
			m_pHttpNet->Abort();
		}
	}
	int GetErrorCode()
	{
		if(m_pHttpNet)
		{
			return m_pHttpNet->GetHttpErrorCode();
		}
		return 0;
	}
	int SetRefUrl(CString strRefUrl)
	{
		if(m_pHttpNet)
		{
			return m_pHttpNet->SetRefer(strRefUrl);
		}
		return 0;
	}
	int SetOtherHead(LPCSTR lpOtherHead)
	{
		if(m_pHttpNet)
		{
			return m_pHttpNet->SetOtherHead(lpOtherHead);
		}
		return 0;
	}
	void SetRequestInfo(LPCSTR lpStrHead, LPCSTR lpStrVal)
	{
		if(m_pHttpNet)
		{
			m_pHttpNet->SetRequestInfo(lpStrHead, lpStrVal);
		}
	}
private:
	IHttpUtil *m_pHttpNet;
};

// http 
class HttpTest  
{
public:
	HttpTest(CString url, CString userAgent, CString refer)
	{
		m_pHttpNet = NULL;
		NetUtil_CreateHttp(&m_pHttpNet);
		if(m_pHttpNet)
		{
			HTTP_REQUEST_INFO reqInfo;
			reqInfo.szURL = url;
			reqInfo.optional.requestType = Request_Get_Testing;
			reqInfo.optional.szUserAgent = userAgent;
			m_pHttpNet->SetRequestInfo(&reqInfo);
			m_pHttpNet->SetRefer(refer);
		}
	}
	~HttpTest()
	{
		Abort();
	}

	bool IsHttpUsable()
	{
		if(NULL == m_pHttpNet) return false;
		NetError error = m_pHttpNet->Request();
		if(error == NetError_OK)
		{
			return true;
		}
		return false;
	}
	void Abort()
	{
		if(m_pHttpNet)
		{
			m_pHttpNet->Abort();
		}
	}
private:
	IHttpUtil *m_pHttpNet;
};

class JsonUtil
{
public:
	static std::string GetUnicodeStr(CString str)
	{
		std::stringstream ss;
		wchar_t *p = str.GetBuffer();
		for (int i = 0, t = str.GetLength();
			i < t;
			++i, ++p)
		{
			char buff[8] = {0};
			sprintf_s(buff, "\\u%04X", *p);
			ss.write(buff, 6);
		}
		return ss.str();
	}
};

/// 网络检测
class NetDetection
{
public:
	static NetDetection& Instance()
	{
		static NetDetection p ;
		return p;
	}
	NetDetection():m_bIsNetEnable(TRUE),m_socket(INVALID_SOCKET)
	{
		StartWorkerThread();
	}
	~NetDetection()
	{
		StopWorkerThread();
	}
	BOOL IsNetEnable()
	{
		return m_bIsNetEnable;
	}

	void SetStopEvent()
	{
		if ( m_hWorkerThreadExitEvent )
		{
			SetEvent(m_hWorkerThreadExitEvent) ;
		}
	}

	// 停止WorkerThread
	void StopWorkerThread()
	{
		if ( m_socket != INVALID_SOCKET )
		{
			WaitForSingleObject(m_hWorkerThread,10000);
			Close();
			CloseHandle(m_hWorkerThread);
			CloseHandle(m_hWorkerThreadExitEvent);
		}
	}
private:
	// 启动WorkerThread
	void StartWorkerThread()
	{
		m_hWorkerThread = ::CreateThread(NULL, 0, WorkerThread,this, 0, NULL);
		m_hWorkerThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	static DWORD WINAPI WorkerThread(LPVOID p)
	{
		NetDetection *pThis = (NetDetection*)p;
		if(pThis)
		{
			pThis->OnWorkerThread();
		}
		return true;
	}
	// 在线程中检测网络状态，8秒检测一次。
	void OnWorkerThread()
	{
		do 
		{
			m_bIsNetEnable = Check();
		}while(WaitForSingleObject(m_hWorkerThreadExitEvent,8000) != WAIT_OBJECT_0);
	}
	// 检测网络是否可用
	BOOL Check()
	{
		UINT ip = 0;
		hostent *phost = gethostbyname("www.baidu.com");
		if (!phost || (AF_INET != phost->h_addrtype) )
		{
			return FALSE;														
		}
		memcpy(&ip, phost->h_addr_list[0], min(sizeof(int),phost->h_length));
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(INVALID_SOCKET == m_socket)
		{
			return FALSE;
		}

		int nMs = 5000;
		setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nMs, sizeof(int));

		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = ip;
		addr.sin_port = htons(80);
		if(0 == connect(m_socket,(sockaddr*)&addr,sizeof(addr)))
		{
			Close();
			return TRUE;
		}
		Close();
		return FALSE;
	}
	// 关闭socket
	void Close()
	{
		if(m_socket != INVALID_SOCKET)
		{
			::closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
	}
private:
	SOCKET m_socket;                 // 网络检测时使用的socket
	BOOL m_bIsNetEnable;			 // 网络状态,TRUE表示网络可用，FALSE为不可用
	HANDLE m_hWorkerThread;	         // WorkerThread线程句柄		 
	HANDLE m_hWorkerThreadExitEvent; // WorkerThread线程退出事件
};
}  // namespace net

#endif