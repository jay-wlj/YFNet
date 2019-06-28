// MsgLimitLayer.cpp: implementation of the CMsgLimitLayer class.
//
//////////////////////////////////////////////////////////////////////
#include "include.h"
#include "MsgLimitLayer.h"
#include "MsgPoolInterface.h"
#include "TaskMgrInterface.h"
#include "qvod_time.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMsgLimitLayer* CMsgLimitLayer::m_instance = NULL;
CMsgLimitLayer::CMsgLimitLayer()
{
}

CMsgLimitLayer::~CMsgLimitLayer()
{
	ClearAllQueue();
}

CMsgLimitLayer* CMsgLimitLayer::Instance()
{
	if(m_instance == NULL)
		m_instance = new CMsgLimitLayer();
	return m_instance;
}

void CMsgLimitLayer::FreeInstance()
{
	if(m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

bool CMsgLimitLayer::AddMsgToUpQueue(HASH& hash,DATA& data,PROTOCOL protocol,bool optimize,bool priority)
{
	//优先处理模式，不判断包队列限制
	if(priority)
	{
		CAutoLock lock(m_upLock);
		HASHDATA hashdata;
		hashdata.hash = hash;
		hashdata.data = data;
		hashdata.protocol = protocol;
		hashdata.timestamp = QvodGetTime();
		m_upList.push_front(hashdata);
		return true;
	}

	return true;
}

bool CMsgLimitLayer::AddMsgToDownQueue(HASH& hash,DATA& data,PROTOCOL protocol,bool priority)
{
	//优先处理模式，不判断包队列限制
	if(priority)
	{
		CAutoLock lock(m_downLock);
		HASHDATA hashdata;
		hashdata.hash = hash;
		hashdata.data = data;
		hashdata.protocol = protocol;
		hashdata.timestamp = QvodGetTime();
		m_downList.push_front(hashdata);
		return true;
	}
	//限速模式对超时包发送优化
	int iDownRateLimit = CMsgPoolInterface::Instance()->GetLimitDownSpeed();
	int iPacketSize = iDownRateLimit*4/16;
	if(iDownRateLimit*4%16 != 0)
	{
		iPacketSize++;
	}
	if((int)m_downList.size() > iPacketSize)//g_speedlimit != -2 && 
	{
		//5秒加速时,取消限制
		return false;
	}
	else
	{
		CAutoLock lock(m_downLock);
		HASHDATA hashdata;
		hashdata.hash = hash;
		hashdata.data = data;
		hashdata.protocol = protocol;
		hashdata.timestamp = QvodGetTime();
		m_downList.push_back(hashdata);
		return true;
	}
}

int CMsgLimitLayer::GetMsgFromDownQueue(HASHDATA& hashdata)
{
	CAutoLock lock(m_downLock);
	list<HASHDATA>::iterator itor = m_downList.begin();
	for(;itor != m_downList.end();++itor)
	{
		//不符合条件跳过
		if(CTaskMgrInterFace::Instance()->IsOvertakeDownSpeed(itor->hash))
		{			
			continue;
		}
		else
		{
			hashdata = *itor;
			m_downList.erase(itor);
			return 0;
		}		
	}	
	return -1;
}

int CMsgLimitLayer::GetMsgFromUpQueue( HASHDATA& hashdata )
{
	return 1;
}

bool CMsgLimitLayer::ClearAllQueue()
{
	m_downLock.Lock();
	if(m_downList.empty())
	{
		m_downLock.Unlock();
		return false;
	}
	else
	{
		list<HASHDATA>::iterator itor=m_downList.begin();
		for(;itor!=m_downList.end();)
		{
			delete[] itor->data.packet;
			itor=m_downList.erase(itor);
		}
	}
	m_downLock.Unlock();

	m_upLock.Lock();
	if(m_upList.empty())
	{
		m_upLock.Unlock();
		return false;
	}
	else
	{
		list<HASHDATA>::iterator itor=m_upList.begin();
		for(;itor!=m_upList.end();)
		{
			delete[] itor->data.packet;
			itor=m_upList.erase(itor);
		}
	}
	m_upLock.Unlock();
	return true;
}