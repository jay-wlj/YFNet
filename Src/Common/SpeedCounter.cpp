#include "common-SpeedCounter.h"
#include "global.h"
#include "qvod_time.h"

CSpeedCounter* CSpeedCounter::m_instance = NULL;
CSpeedCounter::CSpeedCounter(void)
{
}

CSpeedCounter::~CSpeedCounter(void)
{
}

CSpeedCounter* CSpeedCounter::Instance()
{
	if(m_instance==NULL)
		m_instance=new CSpeedCounter();
	return m_instance;
}

void CSpeedCounter::FreeInstance()
{
	if(m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

void CSpeedCounter::PushSpeedData(const HASH* hash,const KEY* key,unsigned int size,CallBackFun callBack)
{
	SPEEDATA sdata;
	if(hash != NULL)
	{
		sdata.hash = *hash;
	}
	if(key != NULL)
	{
		sdata.key = *key;
	}
	sdata.size = size;
	sdata.timestamp = QvodGetTime();
	sdata.callback = (void*)callBack;

	m_lock.Lock();
	m_vector.push_back(sdata);
	m_lock.Unlock();

	PopSpeedData();
}

void CSpeedCounter::DelSpeedData(HASH& hash, CallBackFun callBack)
{
	CAutoLock lock(m_lock);
	vector<SPEEDATA>::iterator it = m_vector.begin();
	for(; it != m_vector.end();)
	{
		SPEEDATA& sData = *it;
		if (sData.hash == hash && sData.callback == (void*)callBack)
		{
			CallBackFun callback = (CallBackFun)sData.callback;
			callback(sData);
			it = m_vector.erase(it);
			continue;
		}
		else
		{
			it++;
		}
	}
}

void CSpeedCounter::DelSpeedData(HASH& hash,KEY &key,CallBackFun callBack)
{
	CAutoLock lock(m_lock);
	vector<SPEEDATA>::iterator it = m_vector.begin();
	for(; it != m_vector.end();)
	{
		SPEEDATA& sData = *it;
		if (sData.hash == hash && sData.key == key && sData.callback == (void*)callBack)
		{
			CallBackFun callback = (CallBackFun)sData.callback;
			callback(sData);
			it = m_vector.erase(it);
			continue;
		}
		else
		{
			it++;
		}
	}
}

void CSpeedCounter::PopSpeedData()
{
	m_lock.Lock();
	vector<SPEEDATA> t_vector;
	vector<SPEEDATA>::iterator it = m_vector.begin();
	QVOD_DWORD curTimeStamp = QvodGetTime();
	for(;it != m_vector.end();)
	{
		if (curTimeStamp - (*it).timestamp >= SPCINTERVALTIME*1000)
		{
			t_vector.push_back(*it);
			it = m_vector.erase(it);
			continue;
		}
		else
		{
			break;
		}
	}
	m_lock.Unlock();
	
	it = t_vector.begin();
	for(; it != t_vector.end();it++)
	{
		SPEEDATA& sData = *it;
		CallBackFun callback = (CallBackFun)sData.callback;
		callback(sData);
	}	
}


