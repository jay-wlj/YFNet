// NetUtil.h file
//
// Author: ����
// Date:   2013-03-19
//////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_NETUTIL_H
#define INCLUDE_NETUTIL_H

#ifdef NETUTIL_LIB
#define NETUTIL_API 
#else
#ifdef NETUTIL_EXPORTS
#define NETUTIL_API extern "C" __declspec(dllexport)
#else
#define NETUTIL_API extern "C" __declspec(dllimport)
#endif
#endif

// ��������
enum RequestType
{
	Request_Trace = 0,	// ֻ�����Ӳ���
	Request_Get_Sync,	// ͬ��Get����
	Request_Get_Async,	// �첽Get����
	Request_Post_Sync,	// ͬ��Post����
	Request_Post_Async,	// �첽Post����
	Request_Get_Testing	// ͬ��Get�������
};

// ���ش�����
enum NetError
{
	NetError_OK = 0,	// �ɹ�
	NetError_DNS,		// ��������ʧ��
	NetError_Socket,	// socket����ʧ��
	NetError_Connect,	// ����ʧ��
	NetError_Send,		// ����ʧ��
	NetError_Recv,		// ����ʧ��
	NetError_Hijack,	// ���ݽٳ�
	NetError_Parameter,	// ��������
	NetError_State,		// ��ǰ״̬�ķǷ�����
	NetError_Memory,	// �ڴ����
	NetError_Abort		// �ֶ�ֹͣ
};

// ����״̬
enum ConnectState
{
	Connect_Ready = 0,	// ����
	Connect_GetHost,	// ����������
	Connect_Connecting,	// ������
	Connect_Sending,	// ����������
	Connect_Receiving,	// ������
	Connect_Finish,		// �������
	Connect_Redirecting,	//�ض�����
	//Connect_Error		// �����쳣
};

// ������Ϣ
typedef struct _PROXYDATA
{
	LPCTSTR szProxyURL;		// ��������
	LPCTSTR szProxyIP;		// ����Ĭ��IP
	LPCTSTR szProxyAccount;	// �����ʺ�
	LPCTSTR szProxyPswd;	// ��������
	_PROXYDATA()
	{
		ZeroMemory(this, sizeof(_PROXYDATA));
	}
}PROXYDATA;

#define POST_CONTENT_FORM "application/x-www-form-urlencoded"
#define POST_CONTENT_JSON "application/json"

// http ��ѡ����
typedef struct _HTTP_OPTIONAL
{
	RequestType requestType;	// ��������
	LPCWSTR szLastModify;		// ����޸�
	LPCWSTR szUserAgent;		// user agent
	int nTimeout;				// ��ʱ
	BOOL bZip;					// �Ƿ�����ѹ��
	char* pPostData;			// Post����
	DWORD dwPostSize;			// Post��С
	char* pPostType;			// Post����  POST_CONTENT_FORM POST_CONTENT_JSON
	PROXYDATA proxy;			// ����
	_HTTP_OPTIONAL()
	{
		ZeroMemory(this, sizeof(_HTTP_OPTIONAL));
		requestType = Request_Get_Sync;	// Ĭ�� get����ͬ��
		nTimeout = 10000; // 10�볬ʱ
		bZip = TRUE; // ����ѹ��
		pPostData = NULL;
		dwPostSize = 0;
		pPostType = NULL;
	}
}HTTP_OPTIONAL;

typedef struct _HTTP_REQUEST_INFO
{
	LPCWSTR szURL;			// �����ַ����
	LPCWSTR szIP[4];		// ����Ĭ��IP
	DWORD dwSupposeSize;	// Ԥ�����ش�С
	HTTP_OPTIONAL optional;	// ��ѡ��
	_HTTP_REQUEST_INFO()
	{
		// �趨Ĭ��ֵ
		szURL = NULL;
		ZeroMemory(szIP, sizeof(LPCTSTR)*4);
		dwSupposeSize = -1;
	}
}HTTP_REQUEST_INFO, *LPHTTP_REQUEST_INFO;

// http���߽ӿ�
interface IHttpUtil
{
	virtual ~IHttpUtil() {};
	// ����������Ϣ
	virtual BOOL SetRequestInfo(LPHTTP_REQUEST_INFO pRequestInfo) = 0;

	virtual void SetRequestInfo(LPCSTR pStrHead, LPCSTR pStrVal) = 0;
	// http���󣬽�����浽�ڴ棬���������ݳ���10MB�᷵��ʧ��
	virtual NetError Request(void) = 0;

	// http���󣬽�����浽�ļ�
	virtual NetError Download(LPCWSTR lpPath) = 0;

	// http��ֹ
	virtual void Abort(void) = 0;

	// ��ȡ���ذٷֱ� -1��ʾδ֪��1000��ʾ100%
	virtual LONGLONG GetPercent(void) = 0;

	// ��ȡ�����ݵĴ�С -1��ʾδ֪
	virtual	LONGLONG GetTotalSize() = 0;

	// ��ȡ�ѽ��յ����ݴ�С
	virtual LONGLONG GetReceiveSize() = 0;

	// ��ȡ�ѽ��յ�����
	virtual NetError GetReceiveData(char* pBuff, LONGLONG cbSize) = 0;

	// ��ȡ��ǰ����״̬
	virtual ConnectState GetConnectState(void) = 0;

	// ��ȡhttp������
	virtual int GetHttpErrorCode(void) = 0;

	// ��ȡʱ���
	virtual CString GetLastModify(void) = 0;

	// ����Refer
	virtual BOOL SetRefer(LPCWSTR szRefer) = 0;

	virtual BOOL SetOtherHead(LPCSTR lpOtherHead) = 0 ;

	// ����Cookie
	//virtual BOOL SetCookie(LPCWSTR szCookie)
};

// ���� http ʵ��
NETUTIL_API BOOL NetUtil_CreateHttp(IHttpUtil **ppHttp);

// ���� http ʵ��
NETUTIL_API BOOL NetUtil_CreateHttps(IHttpUtil **ppHttp);

// ���� http ʵ��
NETUTIL_API void NetUtil_DestroyHttp(IHttpUtil *pHttp);


typedef BOOL (*PNetUtil_CreateHttp)(IHttpUtil **ppHttp);
typedef BOOL (*PNetUtil_DestroyHttp)(IHttpUtil *ppHttp);

// ����URL��Ч��
// NETUTIL_API BOOL NetUtil_HttpGet_Testing(LPCWSTR pURL, LPCWSTR pUserAgent, LPCWSTR pRef);
#endif