// MsgPoolInterface.h: interface for the CMsgPoolInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGPOOLINTERFACE_H__B0A1486D_E7BB_4D65_A462_F18576263295__INCLUDED_)
#define AFX_MSGPOOLINTERFACE_H__B0A1486D_E7BB_4D65_A462_F18576263295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "include.h"

class CMsgPoolInterface
{
public:
	CMsgPoolInterface();
	virtual ~CMsgPoolInterface();
	virtual void Run(bool bOnlyPrior = false)=0;
	virtual void Stop()=0;

	virtual CMsgHandle* GetMsgHandle()=0;
	virtual void DelKeyHash(KEY& key)=0;
	virtual void DelOneKey(KEY &key)=0;
	virtual void DelOtherTaskPeers(HASH& excepthash)=0;
	virtual bool KeyHashExist(KEY& key)=0;
	virtual bool GetKeyHash(KEY& key,HASH& hash)=0; 
	virtual HASH& GetKeyHash(KEY& key)=0; 
	virtual bool InsertKeyHash(KEY& key,HASH& hash)=0;

	//开始暂停任务
	virtual void PauseOneTask(HASH& hash)=0;
	virtual void PauseOtherTask(HASH& excepthash)=0;
	virtual void StartOneTask(HASH& hash)=0;
	virtual void DelOneTask(HASH& hash)=0;
	virtual void PauseAllTask()=0;
	virtual void StartALLTask()=0;

	virtual	void InterDownSize(int size)=0;
	virtual void InterUpSize(int size) = 0;
	//限速部分
	virtual bool IsNeedCacheDownRequest(HASH& hash)=0;
	virtual bool IsNeedCacheDownRequest()=0;
	virtual bool IsOvertakeDownSpeed()=0;

	virtual unsigned int GetGlobalDownSpeed()=0;
	virtual QVOD_UINT64 GetGlobalDownSize()=0;
	virtual unsigned int GetGlobalUpSpeed()=0;
	virtual QVOD_UINT64 GetGlobalUpSize()=0;
	virtual void UpdateNextDownTime(int lastSize)=0;


	virtual bool IsLimitDownSpeed()=0;
	virtual int GetLimitDownSpeed()=0;
	virtual void SetLimitDownSpeed(int speed)=0;
	virtual bool IsLimitUpSpeed() = 0;
	virtual void SetLimitUpSpeed(int speed) = 0;
	virtual int GetLimitUpSpeed() = 0;

	//消息处理
	virtual void BroadCastHave(HASH& hash,int index)=0;
	virtual void SendTimeoutMsg(HASH& hash,KEY& key,request* reqData)=0;

	virtual bool InsertToRefuseSet(HASH& hash)=0;
	virtual bool IsRefuseHash(HASH& hash)=0;

	virtual bool IsBlockDownSpeed()=0;
	virtual void PushTimeoutReq(KEY& key,PROTOCOL protocol)=0;
	virtual void UpdateUpnp()=0;

	//云帆下载模式接口
	virtual void AddOneYfDownload(HASH& hash) = 0;


	static CMsgPoolInterface* Instance();
	static void FreeInstance();
private:
	static CMsgPoolInterface* m_instance;
};

#endif // !defined(AFX_MSGPOOLINTERFACE_H__B0A1486D_E7BB_4D65_A462_F18576263295__INCLUDED_)
