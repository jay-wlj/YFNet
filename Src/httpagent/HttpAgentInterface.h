#pragma once
#include "Include.h"
#include "AgentInfo.h"

class CHttpAgentInterface
{
public:
	CHttpAgentInterface(void);
	virtual void Run()=0;
	virtual void Stop()=0;

	virtual bool DelTask(const HASH& hash)=0;
	virtual int  ReadPiece(const HASH& hash, char *buf, QVOD_UINT64 len, QVOD_UINT64 pos)=0;
	virtual bool RecvPacket(char* buf,int& len)=0;
	virtual unsigned int QueryFileSpeed(const HASH& hash)=0;
	virtual QVOD_UINT64 QueryFileDownLoadLen(const HASH& hash)=0;
	virtual QVOD_UINT64 QueryFileFileLen(const HASH& hash)=0;
	virtual int CopyErrBuf(const HASH& hash,char* buf)=0;
	virtual bool TerminalSock(const HASH& hash,bool initiative=false,bool ieclose=false,bool onConnectFailed=false)=0;
	virtual void InterDownSize(int size)=0;
	virtual void InterDownSize(const HASH& hash,int size)=0;
	virtual int GetIndexByHash(const HASH& hash)=0;
	virtual unsigned int GetGlobalDownSpeed(unsigned char *pHash=NULL)=0;
	virtual bool GetAgentInfo(const HASH& hash,CAgentInfo** agent)=0;
	virtual bool GetAgentDownload(const HASH& hash,int& downloadlen, int& totallen)=0;
	// new httpagent interface
	virtual int AddTask(const HASH &hash, const string &url, const string &refer,  QVOD_INT64 start, QVOD_INT64 len) = 0;
	virtual int AddTask(const HASH &hash, const HttpPostReq& req) = 0;
	virtual int DeleteTask(const HASH &hash) = 0;
	virtual QVOD_INT64 GetFileSize(const HASH &hash) = 0;
	virtual int GetTaskInfo(const HASH &hash, struct taskinfo *info) = 0;
	virtual int Read(const HASH &hash, char *buf, int len, QVOD_INT64 pos) = 0;	// return bytes copied
	virtual int AddM3u8TsTask(const HASH& hash,SM3u8TsReq& req) = 0;
	virtual int ReadM3u8Ts(const HASH &hash,string& filename, char *buf, int len, QVOD_INT64 pos) = 0;
	virtual QVOD_INT64 GetM3u8TsSize(const HASH &hash,const string& filename) = 0;
	virtual bool GetM3u8DownError(const HASH& hash, const string& filename, string& strErrorInfo) = 0;		// 获取m3u8下载出错信息，接口返回响应码
public:
	virtual ~CHttpAgentInterface(void);
	static CHttpAgentInterface* Instance();
	static void FreeInstance();
private:
	static CHttpAgentInterface* m_instance;
};

