#ifndef MONGOTVCDN_H_
#define MONGOTVCDN_H_
#include <vector>
#include <string>
#include "qvod_lock.h"
#include "qvod_event.h"
#include "qvod_thread.h"
using namespace std;
struct SMongoTvCdnReq
{
	string strUrl;
	int type;
	bool bIsOtherCdn;
};
class CMongoTvCdn
{
public:
	enum MONGOTV_CDN_TYPE
	{
		KEEPALIVE,
		LOGOUT,
		START
	};
	static CMongoTvCdn* Instance();
	static void FreeInstance();
	CMongoTvCdn();
	~CMongoTvCdn();
	void Stop();
	bool AddRequest(const char* url,bool bIsOtherCdn,int type);
private:
	static QVOD_THREAD_RETURN QVOD_WINAPI Routine(void* para);
	void DoRoutine();

private:
	static CMongoTvCdn* m_instance;
	bool m_bRunning;
	vector<SMongoTvCdnReq> m_reqVec;
	CLock	m_lock;
	qvod_thread_t	m_hThread;
	qvod_thread_event_t m_event;
	pthread_mutex_t m_eventLock;
};
#endif