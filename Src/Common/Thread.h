#pragma once
#include "..\Include\function.h"

#ifdef USE_THREAD_GBEXIT
extern BOOL g_bExit ;
#endif


namespace common{

class Thread
{
public:
	interface Delegate 
	{
		virtual void OnThread(common::Thread *pThread) = 0;
	};

	Thread(Delegate *pDelegate,DWORD dwThreadID=0):
		m_hThread(NULL),
		m_pDelegate(pDelegate),
		m_dwThreadID(dwThreadID),
		m_pAdditionalData(NULL)
	{

	}

	~Thread()
	{
		WaitThreadExit() ;
	}

private:
	void AddRef()
	{
		InterlockedIncrement(&m_lThreadCount) ;

		static DWORD dwID = 0 ;
		m_dwID = dwID++ ;
	}

	void Release()
	{
		InterlockedDecrement(&m_lThreadCount) ;
	}

public:
	long GetThreadCount()
	{
		return m_lThreadCount ;
	}

	int Run()
	{
#ifdef USE_THREAD_GBEXIT
		if ( g_bExit )
		{
			return 0 ;
		}
#endif

		if( IsRunning() )
		{
			return 2 ;
		}

		WaitThreadExit() ;

		DWORD dwID ;

		AddRef() ;
		m_hThread = CreateThread(NULL, 0, ThreadWorker, this, 0, &dwID);
		if ( !m_hThread )
		{
			Release() ;
			return 0 ;
		}

		return 1 ;
	}

	BOOL IsRunning()
	{
		DWORD dwValue = 0;
		if(m_hThread && GetExitCodeThread(m_hThread, &dwValue) && (dwValue == STILL_ACTIVE))
		{
			return TRUE;
		}
		
		return FALSE;
	}

	DWORD GetThreadID()
	{
		return m_dwThreadID;
	}

	DWORD GetID()
	{
		return m_dwID;
	}

	void AttachData(void *pData)
	{
		m_pAdditionalData = pData;
	}

	void* GetAdditionalData()
	{
		return m_pAdditionalData;
	}

	void WaitThreadExit()
	{
		if (m_hThread)
		{
			//Release() ;

// #ifdef USE_THREAD_GBEXIT
// 			if ( g_bExit )
// 			{
// 				Fun::WaitThreadMsgLoopExit(m_hThread, 10000);
// 			}
// 			else
// #endif
			{
				Fun::WaitThreadExit(m_hThread);
			}
			SafeDelHandle(m_hThread);
		}
	}

	BOOL SetThreadPriority(int nPriority)
	{
		if ( m_hThread )
		{
			return ::SetThreadPriority(m_hThread, nPriority) ;
		}

		return FALSE ;
	}

protected:
	static DWORD WINAPI ThreadWorker(LPVOID lp)
	{
		Thread *pThis = (Thread*)lp;
		if(pThis && pThis->m_pDelegate)
		{
			pThis->m_pDelegate->OnThread(pThis);
		}
		if (pThis)
		{
			pThis->Release();
		}
		
		return 0;
	}

private:
	Delegate		*m_pDelegate;			//�ص�����ָ��
	HANDLE			m_hThread;				//�߳̾��
	DWORD			m_dwThreadID;			//�̺߳��������߳�ID�������ߴ�ֵ
	void			*m_pAdditionalData;		//����
	DWORD			m_dwID;					//ΨһID���Զ�����
	static long		m_lThreadCount;			//���ܵ��߳���
};

} // namespace common