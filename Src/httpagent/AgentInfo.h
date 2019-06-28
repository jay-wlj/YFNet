#pragma once
#include <stdio.h>
#include "common-RefCountedObject.h"
#include "common-commonstruct.h"

class CAgentInfo : public RefCountedObject
{
public:
	CAgentInfo();
public:
	virtual ~CAgentInfo();
	CAgentInfo& operator = (CAgentInfo const& _A);
	enum AgentType{_NONE,_RANGE,_START,_M3U8,_M3U8TS,_POST};
	enum AgentStatus{_INIT,_INVALID,_STOP,_RUNING};
public:
	AgentType type;
	AgentStatus status;
	bool bHaveRange;//原始请求是否带有range字段
	unsigned long ip;
	unsigned short port;
	unsigned int sock;
	int id;						// 标识ID
	QVOD_UINT64 pos;
	QVOD_UINT64 endpos;
	char acchash[20];
	char md5[20];
	char rawbuf[2*1024];
	char referer[256];
	FILE* fp;
	QVOD_UINT64 wholeFilelen;
	QVOD_UINT64 filelen;
	QVOD_UINT64 contentlen;	//Content-Length
	QVOD_UINT64 downloadlen;
	QVOD_UINT64 xsecdownlen;
    QVOD_UINT64 uFilePos;                 // 相对于m3u8文件的位置(仅m3u8有签名大小的ts片时有效)
	unsigned int sendlen;				// 已发送长度
	unsigned int senddatalen;			// 已发送数据长度,仅对post的数据有效
	char* pdatabuf;
	QVOD_UINT64 piecelen;
	char* metabuf;
	unsigned int metalen;
	unsigned int metadownlen;
	bool bshare;
	bool bGzip;							// 是否压缩数据
	bool bThunked;						// 是否thunked分块下载
	unsigned char connectimes;			// 重试连接次数
	bool connected;
	unsigned int connecttime;
	QVOD_DWORD downloadtime;
	unsigned int errcode;				//前两字节存放由于API执行出错的错误码，后两字节为断开的原因
	unsigned short httpcode;			// http请求错误码
	KEY cachekey;
	unsigned int iDataBufLen;           //当前数据的长度
	bool reuse;	// when the current range has been downloaded, we hold the connection for a possible coming request
	QVOD_DWORD last_recv_tick;	// used for timeout check
	QVOD_DWORD initime;			// 初始时间
	unsigned int iMaxDownloadTime;      //最长下载时间，超过该时间未下载完则断开连接，单位为秒
	bool bRemoveInvalidHead;            //芒果tv使用，标识是否已去掉多余的头

	string strHash;						// 该请求所属的hash
	string strHost;                      //原始请求host字段
	string strErrBuf;
	string strOrgUrl;
	string strUrl;				
	string strSavePath;
	string strXml;
	string strDataBuf;
	string strSendDataBuf;
};

