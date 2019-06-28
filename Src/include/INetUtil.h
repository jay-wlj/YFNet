// NetUtil.h file
//
// Author: 关玮
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

// 请求类型
enum RequestType
{
	Request_Trace = 0,	// 只是连接测试
	Request_Get_Sync,	// 同步Get请求
	Request_Get_Async,	// 异步Get请求
	Request_Post_Sync,	// 同步Post请求
	Request_Post_Async,	// 异步Post请求
	Request_Get_Testing	// 同步Get请求测试
};

// 返回错误码
enum NetError
{
	NetError_OK = 0,	// 成功
	NetError_DNS,		// 域名解析失败
	NetError_Socket,	// socket创建失败
	NetError_Connect,	// 连接失败
	NetError_Send,		// 发送失败
	NetError_Recv,		// 接收失败
	NetError_Hijack,	// 内容劫持
	NetError_Parameter,	// 参数错误
	NetError_State,		// 当前状态的非法操作
	NetError_Memory,	// 内存错误
	NetError_Abort		// 手动停止
};

// 连接状态
enum ConnectState
{
	Connect_Ready = 0,	// 就绪
	Connect_GetHost,	// 解析域名中
	Connect_Connecting,	// 连接中
	Connect_Sending,	// 发送数据中
	Connect_Receiving,	// 接收中
	Connect_Finish,		// 接收完成
	Connect_Redirecting,	//重定向中
	//Connect_Error		// 发生异常
};

// 代理信息
typedef struct _PROXYDATA
{
	LPCTSTR szProxyURL;		// 代理域名
	LPCTSTR szProxyIP;		// 代理默认IP
	LPCTSTR szProxyAccount;	// 代理帐号
	LPCTSTR szProxyPswd;	// 代理密码
	_PROXYDATA()
	{
		ZeroMemory(this, sizeof(_PROXYDATA));
	}
}PROXYDATA;

#define POST_CONTENT_FORM "application/x-www-form-urlencoded"
#define POST_CONTENT_JSON "application/json"

// http 可选参数
typedef struct _HTTP_OPTIONAL
{
	RequestType requestType;	// 请求类型
	LPCWSTR szLastModify;		// 最后修改
	LPCWSTR szUserAgent;		// user agent
	int nTimeout;				// 超时
	BOOL bZip;					// 是否启用压缩
	char* pPostData;			// Post数据
	DWORD dwPostSize;			// Post大小
	char* pPostType;			// Post类型  POST_CONTENT_FORM POST_CONTENT_JSON
	PROXYDATA proxy;			// 代理
	_HTTP_OPTIONAL()
	{
		ZeroMemory(this, sizeof(_HTTP_OPTIONAL));
		requestType = Request_Get_Sync;	// 默认 get请求，同步
		nTimeout = 10000; // 10秒超时
		bZip = TRUE; // 启用压缩
		pPostData = NULL;
		dwPostSize = 0;
		pPostType = NULL;
	}
}HTTP_OPTIONAL;

typedef struct _HTTP_REQUEST_INFO
{
	LPCWSTR szURL;			// 请求地址链接
	LPCWSTR szIP[4];		// 请求默认IP
	DWORD dwSupposeSize;	// 预估返回大小
	HTTP_OPTIONAL optional;	// 可选项
	_HTTP_REQUEST_INFO()
	{
		// 设定默认值
		szURL = NULL;
		ZeroMemory(szIP, sizeof(LPCTSTR)*4);
		dwSupposeSize = -1;
	}
}HTTP_REQUEST_INFO, *LPHTTP_REQUEST_INFO;

// http工具接口
interface IHttpUtil
{
	virtual ~IHttpUtil() {};
	// 设置请求信息
	virtual BOOL SetRequestInfo(LPHTTP_REQUEST_INFO pRequestInfo) = 0;

	virtual void SetRequestInfo(LPCSTR pStrHead, LPCSTR pStrVal) = 0;
	// http请求，结果保存到内存，当下载数据超过10MB会返回失败
	virtual NetError Request(void) = 0;

	// http请求，结果保存到文件
	virtual NetError Download(LPCWSTR lpPath) = 0;

	// http终止
	virtual void Abort(void) = 0;

	// 获取下载百分比 -1表示未知，1000表示100%
	virtual LONGLONG GetPercent(void) = 0;

	// 获取总数据的大小 -1表示未知
	virtual	LONGLONG GetTotalSize() = 0;

	// 获取已接收的数据大小
	virtual LONGLONG GetReceiveSize() = 0;

	// 获取已接收的数据
	virtual NetError GetReceiveData(char* pBuff, LONGLONG cbSize) = 0;

	// 获取当前连接状态
	virtual ConnectState GetConnectState(void) = 0;

	// 获取http错误码
	virtual int GetHttpErrorCode(void) = 0;

	// 获取时间戳
	virtual CString GetLastModify(void) = 0;

	// 设置Refer
	virtual BOOL SetRefer(LPCWSTR szRefer) = 0;

	virtual BOOL SetOtherHead(LPCSTR lpOtherHead) = 0 ;

	// 设置Cookie
	//virtual BOOL SetCookie(LPCWSTR szCookie)
};

// 创建 http 实例
NETUTIL_API BOOL NetUtil_CreateHttp(IHttpUtil **ppHttp);

// 创建 http 实例
NETUTIL_API BOOL NetUtil_CreateHttps(IHttpUtil **ppHttp);

// 销毁 http 实例
NETUTIL_API void NetUtil_DestroyHttp(IHttpUtil *pHttp);


typedef BOOL (*PNetUtil_CreateHttp)(IHttpUtil **ppHttp);
typedef BOOL (*PNetUtil_DestroyHttp)(IHttpUtil *ppHttp);

// 测试URL有效性
// NETUTIL_API BOOL NetUtil_HttpGet_Testing(LPCWSTR pURL, LPCWSTR pUserAgent, LPCWSTR pRef);
#endif