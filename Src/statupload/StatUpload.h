#pragma once
#include "statmgr.h"
#include "qvod_lock.h"
#include "qvod_thread.h"
#include <string>
#include <vector>
#include <queue>
#include <map>
using std::string;
using std::vector;
using std::queue;
using std::map;


typedef struct _StatInfo{
	_StatInfo()	{
		retrys = 0;
	}
	std::string strUrl;					// 上报地址，空为默认上报地址
	std::string strPostData;		// post参数
	int				retrys;					// 重新发送次数
}StatInfo, *PStatInfo;

class StatUpload :
	public CStatMgr
{
public:
	StatUpload(void);
	virtual ~StatUpload(void);

	virtual bool Run();
	virtual bool Stop();
	virtual bool AddStat(STAT_FIELD *stats, bool bAsy=true, QVOD_DWORD nTimeout=STAT_TIMEOUT);
	virtual bool AddStat(Json::Value &data);
	virtual void AddStat(const StatInfo& info);
	virtual void SetCommonStat(Json::Value &data);
	static QVOD_THREAD_RETURN Routine(void* param);
	void OnRoutine();
	enum StatStatus{RUNNING,STOP};
protected:
	bool RecvStatInfo(StatInfo &info);
	bool SendStat(const StatInfo &info);
private:
	queue<StatInfo> m_vStatDatas;
	
	CLock							m_vStatLock;

	qvod_thread_t m_handle;						// 上报线程句柄

	int								m_nMaxRetrys;		// 上报失败重试次数

	Json::Value m_comVal;							// 通用字段
public:
	StatStatus					m_status;				// 运行状态
};

