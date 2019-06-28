
/*
 * QVOD Thread.
 */

#ifndef _QVOD_THREAD_H_
#define _QVOD_THREAD_H_



#ifdef WIN32 /* WIN32 */

#include <process.h>

#define QVOD_THREAD_RETURN void
#define QVOD_THREAD_RETURN_NULL
typedef	unsigned long qvod_thread_t;
typedef void(__cdecl *qvod_routine_pt)(void*);

#else /* posix */

#include <pthread.h>

#define QVOD_THREAD_RETURN void*
#define QVOD_THREAD_RETURN_NULL NULL
typedef pthread_t qvod_thread_t;
typedef void *(*qvod_routine_pt)(void*);

#endif /* posix end */

#include "qvod_basetypes.h"


/* func */
int QvodCreateThread(qvod_thread_t *tid, qvod_routine_pt routine, void *arg);
int QvodWaitForThread(qvod_thread_t tid, void **value_ptr, QVOD_DWORD dwMilliseconds=0xFFFFFFFF);
int QvodWaitForMultiThreads(int nCount, const qvod_thread_t *handles);



#endif /* _QVOD_THREAD_H_ */


