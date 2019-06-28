// 定时器
#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <map>
#include <vector>
#include "lock.h"

using std::map;
using std::vector;

namespace QTimer{ 

// 定时器事件接口
interface ITimerEvent
{
	// 定时器事件
	// uTimerID : 定时器ID
	virtual void OnTimer(UINT uTimerID)=0;
};

// 定时器对象
// 对全局定时器进行分装
class CTimer
{
private:
	typedef UINT_PTR SYSTIMERID;  // SetTimer生成的系统定时器ID的类型

	// 描述：CTimer对象映射对象
	// 功能：存放系统定时器ID（使用SetTimer生成）与CTimer对象的映射关系
	class CTimerMap
	{
	public:
		static CTimerMap& Instance()
		{
			static CTimerMap *p = new CTimerMap();
			return *p;
		}
		// 由系统定时器ID获取CTimer对象
		CTimer* Get(SYSTIMERID sysTimerID)
		{
			CTimer *pTimer = NULL;
			m_lock.Lock();
			pTimer = m_mTimer[sysTimerID];
			m_lock.Unlock();
			return pTimer;
		}
		// 添加映射关系
		void Add(SYSTIMERID sysTimerID, CTimer *pTimer)
		{
			m_lock.Lock();
			m_mTimer[sysTimerID] = pTimer;
			m_lock.Unlock();
		}
		// 删除映射关系
		void Del(SYSTIMERID sysTimerID)
		{
			m_lock.Lock();
			std::map<SYSTIMERID, CTimer*>::iterator it = m_mTimer.find(sysTimerID);
			if(it != m_mTimer.end())
			{
				m_mTimer.erase(it);
			}
			m_lock.Unlock();
		}
	private:
		std::map<SYSTIMERID, CTimer*> m_mTimer;
		QLock::CriticalSection m_lock;
	};

	// 描述：定时器ID映射对象
	// 功能：存放用户自定义的定时器ID与系统定时器ID的映射关系
	class TimerIDMap
	{
	public:
		// 添加映射关系
		void Add(UINT userTimerID, SYSTIMERID sysTimerID)
		{
			m_lock.Lock();
			m_mTimerID[userTimerID] = sysTimerID;
			m_lock.Unlock();
		}
		// 删除映射关系
		void Del(UINT userTimerID)
		{
			m_lock.Lock();
			std::map<UINT,SYSTIMERID>::iterator it = m_mTimerID.find(userTimerID);
			if(it != m_mTimerID.end())
			{
				m_mTimerID.erase(it);
			}
			m_lock.Unlock();
		}
		// 由系统定时器ID获取用户自定义定时器ID
		UINT GetUserTimerID(SYSTIMERID sysTimerID)
		{
			UINT userTimerID = 0;
			m_lock.Lock();
			std::map<UINT,SYSTIMERID>::iterator it = m_mTimerID.begin();
			for(;it != m_mTimerID.end();it++)
			{
				if(it->second == sysTimerID)
				{
					userTimerID = it->first;
					break;
				}
			}
			m_lock.Unlock();
			return userTimerID;
		}
		// 由用户自定义定时器ID获取系统定时器ID
		SYSTIMERID GetSysTimerID(UINT userTimerID)
		{
			SYSTIMERID sysTimerID = 0;
			m_lock.Lock();
			std::map<UINT,SYSTIMERID>::iterator it = m_mTimerID.find(userTimerID);
			if(it != m_mTimerID.end())
			{
				sysTimerID = it->second;
			}
			m_lock.Unlock();
			return sysTimerID;
		}
		// 获取系统定时器ID数组
		void GetSysTimerIDArray(std::vector<SYSTIMERID> &sysTimerIDArray)
		{
			m_lock.Lock();
			std::map<UINT,SYSTIMERID>::iterator it = m_mTimerID.begin();
			for(;it != m_mTimerID.end();it++)
			{
				sysTimerIDArray.push_back(it->second);
			}
			m_lock.Unlock();
		}
		void Clear()
		{
			m_lock.Lock();
			m_mTimerID.clear();
			m_lock.Unlock();
		}
		// 用户自定义定时器ID是否存在
		BOOL IsExist(UINT userTimerID)
		{
			BOOL bExist = FALSE;
			m_lock.Lock();
			std::map<UINT,SYSTIMERID>::iterator it = m_mTimerID.find(userTimerID);
			if(it != m_mTimerID.end())
			{
				bExist = TRUE;
			}
			m_lock.Unlock();
			return bExist;
		}
	private:
		std::map<UINT,SYSTIMERID> m_mTimerID;  // 定时器ID mapping.First:用户定义的ID；Second：系统定时器ID
		QLock::CriticalSection m_lock;
	};
public:
	CTimer(ITimerEvent *pEvent):m_pEvent(pEvent){}
	// 设置定时器
	// 参数：
	void Set(UINT uUserTimer,UINT uElapse)
	{
		Kill(uUserTimer);
		SYSTIMERID sysTimerID = ::SetTimer(NULL,0,uElapse,(TIMERPROC)OnTimer);
		m_timerIDMap.Add(uUserTimer,sysTimerID);
		CTimerMap::Instance().Add(sysTimerID,this);
	}
	void Kill(UINT uUserTimerID)
	{
		SYSTIMERID sysTimerID = m_timerIDMap.GetSysTimerID(uUserTimerID);
		KillTimer(NULL, sysTimerID);
		CTimerMap::Instance().Del(sysTimerID);
		m_timerIDMap.Del(uUserTimerID);
	}
	BOOL IsHasSet(UINT uUserTimerID)
	{
		return m_timerIDMap.IsExist(uUserTimerID);
	}
	void KillAll()
	{
		std::vector<SYSTIMERID> sysTimerIDArray;
		m_timerIDMap.GetSysTimerIDArray(sysTimerIDArray);
		for (size_t i = 0; i < sysTimerIDArray.size();i++)
		{
			SYSTIMERID sysTimerID = sysTimerIDArray[i];
			KillTimer(NULL, sysTimerID);
			CTimerMap::Instance().Del(sysTimerID);
		}
		m_timerIDMap.Clear();
	}
private:		
	static void OnTimer(HWND hWnd, UINT uMsg, UINT_PTR uSysTimer, DWORD dwTime)
	{
		CTimer *pTimer = CTimerMap::Instance().Get(uSysTimer);
		if(pTimer)
		{
			pTimer->DispatchTimer(uSysTimer);
		}
		else
		{
			KillTimer(NULL,uSysTimer);
		}
	}
	void DispatchTimer(SYSTIMERID uSysTimer)
	{
		if(m_pEvent)
		{
			m_pEvent->OnTimer(m_timerIDMap.GetUserTimerID(uSysTimer));
		}
	}
private:
	TimerIDMap m_timerIDMap;
	ITimerEvent *m_pEvent;
};

class DelayMessage : public ITimerEvent
{
private:
	struct UserData
	{
		HWND hTargetWnd;
		UINT uMsg;
		WPARAM wparam;
		LPARAM lparam;
	};
	enum TimerID
	{
		kTimerIDDelete = -1,
	};
public:
	DelayMessage():m_Timer(this), m_nID(0) { }
	void PostMessage(HWND hTargetWnd, int nDelay, UINT uMsg, WPARAM wp, LPARAM lp )
	{
		UserData data = {0};
		data.hTargetWnd = hTargetWnd;
		data.uMsg = uMsg;
		data.wparam = wp;
		data.lparam = lp;
		m_mapData.insert(std::map<int, UserData>::value_type(++m_nID, data));
		m_Timer.Set(m_nID, nDelay);
	}
	void Delete(int nDelay)
	{
		m_Timer.Set(kTimerIDDelete, nDelay);
	}
	virtual void OnTimer(UINT uTimerID)
	{
		switch(uTimerID)
		{
		case kTimerIDDelete:
			delete this;
			break;
		default:
			{
				std::map<UINT, UserData>::iterator it = m_mapData.find(uTimerID);
				if (it != m_mapData.end())
				{
					::PostMessage(it->second.hTargetWnd, it->second.uMsg, it->second.wparam, it->second.lparam);
				}
				m_Timer.Kill(uTimerID);
			}
			break;
		}
	}
private:
	CTimer m_Timer;
	int m_nID;
	std::map<UINT , UserData> m_mapData;
};


}// namespace QTimer
#endif