#pragma once

//错误码
typedef int STAT_ERROR;
#define STAT_ERROR_OK			0
#define STAT_ERROR_TIMEOUT		1
#define STAT_ERROR_UNVALID_NET		2
#define STAT_ERROR_UNVALID_FORMART	3
#define STAT_ERROR_GETUID_FAILED	4
#define STAT_ERROR_OTHER			5
#define STAT_ERROR_UNVALID_ACT	6

#define ST_ACT								"act"
//统计类型
#define ACT_UNINSTALL				"uninstall"
#define ACT_BUFFER					"buffer"



//字段类型
#define FT_CT 							"ct"
#define FT_LRES 						 "reason"
#define FT_URES 						 "uinput"
#define FT_TD	 						 "td"

#define STAT_TIMEOUT				5000

#pragma pack(push)
#pragma pack(1)
struct Value
{
	VARTYPE			vType;	//数据类型
	union
	{
		ULONGLONG	ullVal; //VT_UI8
		LONGLONG		llVal;	//VT_I8
		ULONG		ulVal;	//VT_UI4
		LONG			lVal;		//VT_I4
		const char*		pVal;	//VT_PTSTR
		const wchar_t*   wpVal;	
	};

	Value()
	{
		memset(this, 0, sizeof(Value));
	}
	Value(ULONGLONG val)
	{
		vType = VT_UI8;
		ullVal = val;
	}
	Value(LONGLONG val)
	{
		vType = VT_I8;
		llVal = val;
	}
	Value(ULONG val)
	{
		vType = VT_UI4;
		ulVal = val;
	}
	Value(LONG val)
	{
		vType = VT_I4;
		lVal = val;
	}
	Value(int val)
	{
		vType = VT_I4;
		lVal = val;
	}
	Value(UINT val)
	{
		vType = VT_UI4;
		ulVal = val;
	}
	Value(LPCSTR val)
	{
		vType = VT_LPSTR;
		pVal = val;
	}
	Value(char* val)
	{
		vType = VT_LPSTR;
		pVal = val;
	}
	Value(LPCWSTR val)
	{
		vType = VT_LPWSTR;
		wpVal = val;
	}
	Value(const Value& val)
	{
		memcpy(this, &val, sizeof(val));
	}


	Value& operator=(const Value& val)
	{
		memcpy(this, &val, sizeof(val));
		return *this;
	}
	Value& operator=(ULONGLONG val)
	{
		vType = VT_UI8;
		ullVal = val;
		return *this;
	}
	Value& operator=(LONGLONG val)
	{
		vType = VT_I8;
		llVal = val;
		return *this;
	}
	Value& operator=(ULONG val)
	{
		vType = VT_UI4;
		ulVal = val;
		return *this;
	}
	Value& operator=(int val)
	{
		vType = VT_I4;
		lVal = val;
		return *this;
	}
	Value& operator=(UINT val)
	{
		vType = VT_UI4;
		ulVal = val;
		return *this;
	}
	Value& operator=(char* val)
	{
		vType = VT_LPSTR;
		pVal = val;
		return *this;
	}
	Value& operator=(LPCSTR val)
	{
		vType = VT_LPSTR;
		pVal = val;
		return *this;
	}
	Value& operator=(LPCWSTR val)
	{
		vType = VT_LPWSTR;
		wpVal = val;
		return *this;
	}
};

//字段信息
struct STAT_FIELD
{
	LPCSTR  nID;				//字段ID
	Value  val;				//值
};
#pragma pack(pop)

#ifdef STAT_LIB
#define STATAPI 
#else
#ifdef STAT_EXPORTS
#define STATAPI extern "C" __declspec(dllexport) 
#else
#define STATAPI extern "C" __declspec(dllimport) 
#endif
#endif


STATAPI	void InitStat();

/************************************************************************
SendStat			发送统计
nStatType			[IN] 统计类型
arrStat				[IN] 包含统计字段信息的数组
nArrLen				[IN] 数组的长度
bAsy					[IN} 异步发送 
nTimeOut			[IN] 超时时间

备注："deid"，"机器码"，"bid"，"dch" "ver"这几种字段无需传入。
	   若除此之外再无其他字段，则arrStat与nArrLen应传空值。
************************************************************************/
STATAPI STAT_ERROR SendStat(LPCSTR lpStrAct, STAT_FIELD arrStat[] = NULL, DWORD nArrLen = 0, BOOL bAsy = TRUE, DWORD nTimeOut = STAT_TIMEOUT);

typedef STAT_ERROR (_cdecl  *PSendStat)(LPCSTR lpStrAct, STAT_FIELD arrStat[] , DWORD nArrLen , BOOL bAsy , DWORD nTimeOut );


/************************************************************************
SendStatEx		发送统计（可发送第三方统计)
lpstrStat			[IN} 发送内容(可为URL+内容),若lpstrStat以"http://"开头则发送到此服务器，否则发送到默认统计服务器
bAsy					[IN} 是否异步发送 TRUE:异步	FALSE:同步
nTimeOut			[IN] 超时时间
bAddComm		[IN] 是否需要添加公共字段（"deid"，"机器码"，"bid"，"dch" "ver"）
************************************************************************/
STATAPI STAT_ERROR SendStatEx(LPCSTR lpstrStat, BOOL bAsy = TRUE, DWORD nTimeOut = STAT_TIMEOUT, BOOL bAddComm = FALSE);

typedef STAT_ERROR (_cdecl  *PSendStatEx)(LPCSTR lpStrAct,  BOOL bAsy, DWORD nTimeOut, BOOL bAddComm);

STATAPI int MakeStatUrl(LPCSTR lpStrAct, STAT_FIELD arrStat[], DWORD nLen, BOOL bAddComm, LPSTR pOutStr, DWORD dwLen);
typedef int (_cdecl  *PMakeStatUrl)(LPCSTR lpStrAct,  STAT_FIELD arrStat[], DWORD nLen, BOOL bAddComm, LPSTR pOutStr, DWORD dwLen);


STATAPI STAT_ERROR PostStat(LPCSTR lpstrUrl, LPCSTR lpContent, int nLen, BOOL bAsy = TRUE, DWORD nTimeOut = STAT_TIMEOUT, BOOL bAddComm = FALSE, int nPostType=0);
typedef STAT_ERROR (_cdecl  *PPostStat)(LPCSTR lpstrUrl, LPCSTR lpContent, int nLen, BOOL bAsy, DWORD nTimeOut, BOOL bAddComm, int nPostType);

/************************************************************************
SendStat			发送过去的统计
nTimeOut			[IN] 超时时间

备注：此接口将重新发送过去由于网络原因发送失败的所有统计
************************************************************************/
STATAPI	STAT_ERROR SendLastStat(BOOL bAsy = TRUE, DWORD nTimeOut = STAT_TIMEOUT);

STATAPI	void UnInitStat();
