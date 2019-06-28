// MsgPool.h: interface for the CMsgPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGPOOL_H__8B342913_A540_43BA_A880_B1A3AE1C17BD__INCLUDED_)
#define AFX_MSGPOOL_H__8B342913_A540_43BA_A880_B1A3AE1C17BD__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif
#include "include.h"
#include "MsgPoolInterface.h"
#include "MsgHandle.h"
#include "MsgLimitLayer.h"
#include "qvod_lock.h"
#include "Iointerface.h"
#include <map>
#include <set>
using namespace std;

struct RedundancyData
{
	unsigned int downlenfromserver;
	unsigned int downlenfromcache;
	unsigned int downlenfromclient;
	unsigned int repeatdownlen;
	unsigned int errordatalen;
	unsigned int writefaillen;
	unsigned int notaskdownlen;
	unsigned int invaliddownlen;
	unsigned int uplentoserver;
	unsigned int uplentoclient;
	unsigned int checksum;
};

class CMsgPool :public CMsgPoolInterface
{
public:
	enum MsgPoolStatus{RUNNING,STOP};
	CMsgPool();
    ~CMsgPool();
	virtual void Run(bool bOnlyPrior);
	virtual void Stop();
	
	virtual CMsgHandle* GetMsgHandle();
	//线程模块
	static QVOD_THREAD_RETURN QVOD_WINAPI CommonRoutine(void* param);
	static QVOD_THREAD_RETURN QVOD_WINAPI Routine(void* param);
	static QVOD_THREAD_RETURN QVOD_WINAPI UpRoutine(void* param);
	static bool SendPacket(KEY& key,char* packet,int length);

	// 任务操作代码段
	virtual void PauseAllTask();
	virtual void PauseOtherTask(HASH& excepthash);
	virtual void StartALLTask();
	virtual void PauseOneTask(HASH& hash);
	virtual void StartOneTask(HASH& hash);
	virtual void DelOneTask(HASH& hash);

	virtual void DelKeyHash(KEY& key);
	virtual void DelOneKey(KEY &key);
	virtual void DelOtherTaskPeers(HASH& excepthash);
	virtual bool KeyHashExist(KEY& key);
	virtual bool GetKeyHash(KEY& key,HASH& hash); 
	virtual HASH& GetKeyHash(KEY& key); 
	virtual bool InsertKeyHash(KEY& key,HASH& hash);

#ifdef NETBAR
	virtual bool SendPieceRequest();//内存版定时发送片请求
#endif

	virtual bool IsNeedCacheDownRequest(HASH& hash);
	virtual bool IsNeedCacheDownRequest();
	virtual bool IsOvertakeDownSpeed();
	virtual bool IsOvertakeUpSpeed();
	virtual unsigned int GetGlobalDownSpeed();
	virtual QVOD_UINT64 GetGlobalDownSize();
	virtual unsigned int GetGlobalUpSpeed();
	virtual QVOD_UINT64 GetGlobalUpSize();	

	virtual void UpdateNextDownTime(int lastSize);
	virtual void UpdateNextUpTime(int lastSize);

	virtual bool IsLimitDownSpeed();
	virtual int GetLimitDownSpeed();
	virtual void SetLimitDownSpeed(int speed);
	virtual bool IsLimitUpSpeed();
	virtual void SetLimitUpSpeed(int speed);
	virtual int GetLimitUpSpeed();

	//消息处理
	virtual void BroadCastHave(HASH& hash,int index);
	virtual void SendTimeoutMsg(HASH& hash,KEY& key,request* reqData);

	bool IsValidData(KEY& key);
	void SetGlobalUpSpeed();
	void SetGlobalDownSpeed();

	//查询速度计算
	virtual	void InterDownSize(int size);
	virtual void InterUpSize(int size);
	virtual bool InsertToRefuseSet(HASH& hash);
	virtual bool IsRefuseHash(HASH& hash);

	virtual bool IsBlockUpSpeed();
	virtual bool IsBlockDownSpeed();
	virtual void PushTimeoutReq(KEY& key,PROTOCOL protocol);
	virtual void PopTimeoutReq();
	virtual void UpdateUpnp();

	//云帆下载模式接口
	virtual void AddOneYfDownload(HASH& hash);

private:
#ifndef SUPERNODE
	void DoDownLimitAction(HASHDATA& hashdata);
	void DoUpLimitAction();
#endif
	void DoCommonAction();
	void ReportAverageSpeed();

	//处理过程函数
	void ProcessHttpAgentPacket(char* packet);

#if 0
	//
	void CheckSpeedLimit();
#endif

private:
	MsgPoolStatus m_status;
	MsgPoolStatus m_priorRoutineStatus;	//优先线程:限速线程和Common线程
	set<HASH> m_refuseHash;
	map<KEY,HASH> m_keyHashMap;
	map<HASH,set<KEY> > m_hashKeyMap;
	list<TRACKDATA> m_trackList;
	queue<TIMEOUTREQ> m_timeoutReqQue;
	CLock m_timeoutQueLock;
	CLock m_keyHashMapLock;
	CLock m_trackListLock;
	CLock m_hashKeyMapLock;
	CLock m_refuseHashLock;
	CLock m_downTotalSizeLock;
	CLock m_upTotalSizeLock;
	//限速部分变量
	int m_limitUpSpeed,m_limitDownSpeed;
	QVOD_DWORD m_nextDownTime,m_nextUpTime;
	unsigned int m_xSecUpBufsize,m_xSecDownBufSize;
	unsigned int m_downloadSpeed,m_uploadSpeed;
	int m_downQueueTime,m_upQueueTime;
	QVOD_UINT64 m_upTotalSize,m_downTotalSize;
	int m_lastUpSize,m_lastDownSize;

#ifndef FOREIGNER
	//高速通道服务器(Hi-Speed Server)速度相关
	CLock m_lockVipAccServerDown;
	unsigned int m_xSecVipAccServerDownSize;		//10秒内从高速通道服务器下载的数据长度
	unsigned int m_iVipAccServerDownSpeed;			//全局高速通道服务器的速度
	QVOD_UINT64 m_iVipAccServerTotalDownSize;	//从高速通道服务器下载的数据总长度
#endif

	//全网平均速度统计相关
#if 0
	CLock m_lockSpeedStc;						//64位整数,多线程操作,应当加锁
	unsigned long		m_dwLastReportTime;
	QVOD_UINT64	m_iStcSpeedDownSize;
	QVOD_UINT64	m_iStcSpeedUpSize;		//统计时间内的上传/下载总大小
#endif

	//冗余统计
	QVOD_UINT64                  m_downlentotal;
	QVOD_UINT64                  m_uplentotal;
	RedundancyData          m_redundancyData;
	CLock		            m_redundancydataLock;

	QVOD_DWORD m_secPos[10];
	//线程数组句柄
	qvod_thread_t m_handles[5];
	//HANDLE m_handles[5];
	int m_initMaxdownTask;

	//queue<PEER> m_peerQue;
	//CLock m_peerQueLock;

	//set<KEYTIME> m_keytimeSet;
	//CLock m_keytimeSetLock;
	
public:
	CMsgHandle* m_msgHandle;
};
#endif
