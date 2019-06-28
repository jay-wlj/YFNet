#ifndef _TRACKERINTERFACE_H
#define _TRACKERINTERFACE_H

#include "qvod_event.h"

struct msgheadreq;
struct msghead;
class CTrackerInterface 
{
private:
	static CTrackerInterface* m_instance;
public:
	virtual ~CTrackerInterface(){};
	virtual bool Run(const char* peerid = NULL)=0;
	virtual void Stop()=0;
	virtual void AddMsg(msgheadreq* pmsg,bool ahead)=0;
	virtual bool GetMsg(char* buf,int& len)=0;
	virtual bool CopyWaitingEvent(qvod_thread_event_t **handle, pthread_mutex_t** lock)=0;
	virtual void Login()=0;
	virtual unsigned int GetPublicIP()=0;
	virtual unsigned int GetTrackerIP()=0;
	virtual unsigned int GetLoginStatus()=0;//1为登录成功
	virtual unsigned int GetAgentStatus()=0;//1为启用代理
	virtual char* GetPeerID()=0;
	virtual unsigned int GetSocket()=0;

	virtual bool ConnectPeer(unsigned int ip,unsigned short port,unsigned int nattype,char* peerid,unsigned char* hash)=0;

	static CTrackerInterface* Instance();
	static void FreeInstance();
};

#endif
