#pragma once
#include "HttpUploadInterface.h"
#include "AgentInfo.h"
#include "qvod_string.h"
#include "qvod_file.h"
#include "qvod_time.h"
#include "qvod_sock.h"
#include "qvod_thread.h"
#include "qvod_error.h"
#include "HttpSock.h"
#ifdef SUPPORT_HTTPS
#include "HttpSSLSock.h"
#endif
#include <set>
using std::set;

#define MAX_AGENT_RANGE     (1<<25)	// 32MB
#define MAX_KEEPALIVE_TICKS 15*1000	// when there is no more range request, we close the connection
#define MAX_CONNECT_TIME	5*1000
typedef struct _HttpAgentSockData:public _HTTPSOCK_DATA
{
	HASH hash;		// sock关联的hash

	virtual ~_HttpAgentSockData()
	{
	}
}HttpAgentSockData, *PHttpAgentSockData;

struct taskinfo
{
	QVOD_INT64 start;
	QVOD_INT64 len;
	QVOD_INT64 downloaded;
	enum CAgentInfo::AgentStatus status;
	QVOD_INT64 filesize;
};

class CHttpUpload :public CHttpUploadInterface
{
public:
	enum AgentStatus{RUNNING,STOP};
	CHttpUpload(void);
public:
	virtual ~CHttpUpload(void);
	virtual void Run();
	virtual void Stop();
	virtual bool  DelTask(const HASH& hash);
	virtual bool RecvPacket(char* buf,int& len);
	virtual bool TerminalSock(const HASH& hash,bool initiative=false,bool ieclose=false,bool onConnectFailed=false);
	virtual bool TerminalSock(const qvod_sock_t& sock,bool initiative=false,bool ieclose=false,bool onConnectFailed=false);
	virtual int CopyErrBuf(const HASH& hash,char* buf);
	virtual unsigned int QueryFileSpeed(HASH& hash);

	// new httpagent interface
	virtual int AddTask(const HASH& hash, const HttpPostReq& req);
	virtual int AddPostTask(const HASH &hash, const string &url,  const int& id, const string& strHeadAdd, const char* pBuf, const unsigned int& len, const HttpPostReq& req);
	virtual int DeleteTask(const HASH &hash);
	virtual int GetTaskInfo(const HASH &hash, struct taskinfo *info);
	
	virtual bool UnGzip(const char* buf, const int &len, string& strBuf);
#ifdef WIN32
	WSAEVENT* GetEventArray(int index = 0);
#endif
	qvod_sock_t GetSockArray(int index = 0);
	//bool GetSockBindHash(const qvod_sock_t& s, HASH& hash)	;	// 获取socket索引绑定的hash			
	bool GetAgentInfo(const HASH& hash,CAgentInfo** agent);
	void DelAgentInfo(const HASH& hash);
	void ClearAgentInfoMap();
	void SetSavePath(char* url,char* savepath,QVOD_UINT64 ipos=0);
	int OnData(const qvod_sock_t& s);
	virtual int OnConnect(const qvod_sock_t& s, const int& i);
	int OnConnectFail(const qvod_sock_t& s);
	int OnClose(const qvod_sock_t& s);
	int OnClose(const HASH& hash);
	int ReConnect(const HASH& hash);
	void deleteUnusedSock();
	static QVOD_THREAD_RETURN QVOD_WINAPI Routine(void* param);

private:
	int GetIndexHttpSock(const qvod_sock_t& sock);
	int GetIndexHttpSock(const CHttpSock* pSock);
	bool GetHttpSock(const qvod_sock_t& s, CHttpSock** pSock );
	bool GetHttpSockByHash(const HASH& hash, CHttpSock** pSock );
	bool GetSockBindHash(const qvod_sock_t& s, HASH& hash);					// 获取socket索引绑定的hash
	bool RelaseSockHash(const HASH& hash);
	qvod_sock_t GetSocketByHash(const HASH& hash);					// 通过hash查找对应的sock


private:
	bool HasHttpSock(const int &i, const unsigned& sock);		// sock对象绑定的sock是否一样

protected:
	void AddOneTaskHash(const HASH& taskHash, const HASH& childHash);					// 添加子请求hash到任务hash中
	void DelOneTaskHash(const HASH& taskHash, const HASH& childHash);					// 从任务hashmap中删除子请求的hash
	void DelOneTaskSock(const HASH& taskHash);															// 删除任务hashmap中了所有请求
	bool FindOneTaskHash(const HASH& taskHash, const string& strTsName, HASH& outHash);	// 查找任务hashmap中的子请求hash,根据请求的url
	int AddRange(char* url,char* reqname,HASH& hash,QVOD_UINT64 beginpos,QVOD_UINT64 endpos,int type,char* referer);
	std::string GetHeadValue(const char* pStrInfo,  const char* pKey);
	bool IsChunkEnd(const std::string& strBuf, QVOD_UINT64 length, QVOD_UINT64 &knownLen/*已知的chunk长度, 用于内存申请*/);	//查找trunked结束标志
	char* GetChunkData(char const *pChunkData, QVOD_UINT64 datalen, char *pBuff, QVOD_UINT64 bufflen, QVOD_UINT64 &usedBuffLen, QVOD_UINT64 &handledChunkDataLen);
public:
	AgentStatus m_status;
	qvod_thread_t m_handle;
	CLock m_fileLock;

	int m_IfSockConnected[64]; /* silver, record if socket connected */

#ifdef WIN32
	WSAEVENT m_EventArray[64];
#else
	struct pollfd pfds[64];
#endif
	
	CHttpSock *m_SocketArray[64]; 
	//qvod_sock_t m_SocketArray[64];
	int m_nSocket;//套接字数目，最多只能64个

	CLock m_nSockLock;
	QVOD_DWORD m_timestamp;

	unsigned int m_iGlobalDownSpeed;

	map<HASH, qvod_sock_t> m_HashSockMap;
	map<HASH,CAgentInfo*> m_agentMap;
	CLock m_agentLock;

	queue<agentmsghead*> m_msgQueue;
	CLock m_msgQueueLock;

	std::map<HASH,HASH> m_hashMap;
	CLock m_hashmapLock;
	std::map<HASH,map<string, string> > m_mHttpErrors;
	CLock m_errorLock;
	std::map<HASH, set<HASH> > m_mOneTaskHashs;			// 主hash下的多个请求子hash
	CLock m_hashonetaskLock;
};

