#pragma once
#include "Include.h"

class CHttpUploadInterface
{
public:
	CHttpUploadInterface(void);
	virtual void Run()=0;
	virtual void Stop()=0;
	virtual bool DelTask(const HASH& hash)=0;
	virtual bool RecvPacket(char* buf,int& len)=0;
	virtual unsigned int QueryFileSpeed(HASH& hash)=0;

	virtual int CopyErrBuf(const HASH& hash,char* buf)=0;
	virtual bool TerminalSock(const HASH& hash,bool initiative=false,bool ieclose=false,bool onConnectFailed=false)=0;

	
	// new httpagent interface
	virtual int AddTask(const HASH& hash, const HttpPostReq& req) = 0;
	virtual int AddPostTask(const HASH &hash, const string &url, const int& nId, const string& strHeadAdd, const char* pBuf, const unsigned int& len, const HttpPostReq& req) = 0;
	virtual int DeleteTask(const HASH &hash) = 0;
	virtual int GetTaskInfo(const HASH &hash, struct taskinfo *info) = 0;
	
public:
	virtual ~CHttpUploadInterface(void);
	static CHttpUploadInterface* Instance();
	static void FreeInstance();
private:
	static CHttpUploadInterface* m_instance;
};