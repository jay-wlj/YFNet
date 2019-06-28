
/*
 * QVOD Lock.
 */


#ifndef _QVOD_LOCK_H_
#define _QVOD_LOCK_H_



#include "qvod_string.h"


#ifdef WIN32  /* WIN32 */

typedef	CRITICAL_SECTION qvod_thread_lock_t;
typedef	volatile long qvod_atom_t;

#else /* posix */

#include <pthread.h>

typedef pthread_mutex_t qvod_thread_lock_t;
typedef volatile long qvod_atom_t;

#endif /* posix end */


/* qvod CCriticalSection */
class CCriticalSection
{
public:

	CCriticalSection(qvod_thread_lock_t *lock);
	~CCriticalSection();
	int Lock();
	int TryLock();
	int UnLock();
	bool IsLocked();

private:

	qvod_thread_lock_t *m_lock;
	bool m_blocked;
};


/* qvod CLock */
class CLock
{
public:
	CLock();
	~CLock();

	void Lock();
	void Unlock();

private:
	qvod_thread_lock_t  m_cs;
};

class CAutoLock
{
public:
	CAutoLock(CLock &lock);
	~CAutoLock();

private:
	CLock	&m_lock;
};


/* func */
int	QvodInitializeCS(qvod_thread_lock_t *lock);
int QvodDestroyCS(qvod_thread_lock_t *lock);
int QvodCSLock(qvod_thread_lock_t *lock);
int QvodCSTrylock(qvod_thread_lock_t *lock);
int QvodCSUnlock(qvod_thread_lock_t *lock);


/* qvod atom operation */
int QvodAtomAdd(qvod_atom_t *value);
int QvodAtomDec(qvod_atom_t *value);





#endif /* _QVOD_LOCK_H_ */


