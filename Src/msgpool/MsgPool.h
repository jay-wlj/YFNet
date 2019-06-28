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
	//�߳�ģ��
	static QVOD_THREAD_RETURN QVOD_WINAPI CommonRoutine(void* param);
	static QVOD_THREAD_RETURN QVOD_WINAPI Routine(void* param);
	static QVOD_THREAD_RETURN QVOD_WINAPI UpRoutine(void* param);
	static bool SendPacket(KEY& key,char* packet,int length);

	// ������������
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
	virtual bool SendPieceRequest();//�ڴ�涨ʱ����Ƭ����
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

	//��Ϣ����
	virtual void BroadCastHave(HASH& hash,int index);
	virtual void SendTimeoutMsg(HASH& hash,KEY& key,request* reqData);

	bool IsValidData(KEY& key);
	void SetGlobalUpSpeed();
	void SetGlobalDownSpeed();

	//��ѯ�ٶȼ���
	virtual	void InterDownSize(int size);
	virtual void InterUpSize(int size);
	virtual bool InsertToRefuseSet(HASH& hash);
	virtual bool IsRefuseHash(HASH& hash);

	virtual bool IsBlockUpSpeed();
	virtual bool IsBlockDownSpeed();
	virtual void PushTimeoutReq(KEY& key,PROTOCOL protocol);
	virtual void PopTimeoutReq();
	virtual void UpdateUpnp();

	//�Ʒ�����ģʽ�ӿ�
	virtual void AddOneYfDownload(HASH& hash);

private:
#ifndef SUPERNODE
	void DoDownLimitAction(HASHDATA& hashdata);
	void DoUpLimitAction();
#endif
	void DoCommonAction();
	void ReportAverageSpeed();

	//������̺���
	void ProcessHttpAgentPacket(char* packet);

#if 0
	//
	void CheckSpeedLimit();
#endif

private:
	MsgPoolStatus m_status;
	MsgPoolStatus m_priorRoutineStatus;	//�����߳�:�����̺߳�Common�߳�
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
	//���ٲ��ֱ���
	int m_limitUpSpeed,m_limitDownSpeed;
	QVOD_DWORD m_nextDownTime,m_nextUpTime;
	unsigned int m_xSecUpBufsize,m_xSecDownBufSize;
	unsigned int m_downloadSpeed,m_uploadSpeed;
	int m_downQueueTime,m_upQueueTime;
	QVOD_UINT64 m_upTotalSize,m_downTotalSize;
	int m_lastUpSize,m_lastDownSize;

#ifndef FOREIGNER
	//����ͨ��������(Hi-Speed Server)�ٶ����
	CLock m_lockVipAccServerDown;
	unsigned int m_xSecVipAccServerDownSize;		//10���ڴӸ���ͨ�����������ص����ݳ���
	unsigned int m_iVipAccServerDownSpeed;			//ȫ�ָ���ͨ�����������ٶ�
	QVOD_UINT64 m_iVipAccServerTotalDownSize;	//�Ӹ���ͨ�����������ص������ܳ���
#endif

	//ȫ��ƽ���ٶ�ͳ�����
#if 0
	CLock m_lockSpeedStc;						//64λ����,���̲߳���,Ӧ������
	unsigned long		m_dwLastReportTime;
	QVOD_UINT64	m_iStcSpeedDownSize;
	QVOD_UINT64	m_iStcSpeedUpSize;		//ͳ��ʱ���ڵ��ϴ�/�����ܴ�С
#endif

	//����ͳ��
	QVOD_UINT64                  m_downlentotal;
	QVOD_UINT64                  m_uplentotal;
	RedundancyData          m_redundancyData;
	CLock		            m_redundancydataLock;

	QVOD_DWORD m_secPos[10];
	//�߳�������
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
