#pragma once
#include <Windows.h>

class CLock
{
public:
	CLock(CRITICAL_SECTION &pcs): m_cs(pcs) 
	{ 
		EnterCriticalSection(&pcs);
	}
	~CLock(void) 
	{ 
		LeaveCriticalSection(&m_cs);
	}
private:
	CRITICAL_SECTION &m_cs;
};



namespace QLock{

class CriticalSection
{
public:
	CriticalSection()
	{
		InitializeCriticalSection(&m_csLock);
	}
	~CriticalSection()
	{
		DeleteCriticalSection(&m_csLock);
	}
	void Lock()
	{
		EnterCriticalSection(&m_csLock);
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_csLock);
	}
	BOOL IsLock()
	{
		return TryEnterCriticalSection(&m_csLock);
	}
public:
	CRITICAL_SECTION m_csLock;
};

class AutoCriticalSection 
{
public:
	explicit AutoCriticalSection(CriticalSection& lock) : m_lock(lock) 
	{
		m_lock.Lock();
	}

	~AutoCriticalSection() 
	{
		m_lock.Unlock();
	}
private:
	AutoCriticalSection(const AutoCriticalSection&);               
	void operator=(const AutoCriticalSection&);
private:
	CriticalSection& m_lock;

};

class Mutex
{
public:
	Mutex(LPTSTR lpName, BOOL bInitialOwner, LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL)
	{
		m_hMutex = ::CreateMutex(lpMutexAttributes, bInitialOwner, lpName);
	}
	~Mutex()
	{
		if(m_hMutex) ::CloseHandle(m_hMutex);
	}
	int WaitForSingleObject(DWORD dwMilliseconds = INFINITE)
	{
		if(m_hMutex)
			return ::WaitForSingleObject(m_hMutex, dwMilliseconds);
		return 0;
	}
	int Release()
	{
		if(m_hMutex) 
			return ::ReleaseMutex(m_hMutex);
		return 0;
	}
private:
	HANDLE  m_hMutex;
};

}  // namespace QLock
