#ifndef _IOINTERFACE_H
#define _IOINTERFACE_H
#include "../include/stdafx.h"

enum FLAG
{
	WRITE = 1,
	READ
};
struct IO
{
	long threadid;
	FLAG flag;
	QVOD_UINT64 len;
	QVOD_UINT64 pos;
	char* buf;
	HASH hash;
	KEY key;
	int result;//0成功，其它为错误码
};
class IIoInterface
{
private:
	static IIoInterface* m_instance;
public:
	virtual ~IIoInterface(){};
	virtual void Run()=0;
	virtual bool AddIo(IO* pIO,char disk)=0;
	virtual bool GetIo(IO* &pIO)=0;
	virtual bool CopyWaitingEvent(qvod_thread_event_t **handle, pthread_mutex_t** lock)=0;
	virtual void Stop()=0;
	static IIoInterface* Instance();
	static void FreeInstance();
};
#endif