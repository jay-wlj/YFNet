
/*
 * QVOD Event.
 */


#ifndef _QVOD_EVENT_H_
#define _QVOD_EVENT_H_

#include "qvod_basetypes.h"
#ifdef WIN32 /* WIN32 */

#include <windows.h>

typedef	HANDLE qvod_thread_event_t;
typedef void* pthread_mutex_t;

#else /* posix */

#include "qvod_string.h"
#include <semaphore.h>

#ifdef MACOSX
typedef pthread_cond_t qvod_thread_event_t;
#else
typedef sem_t qvod_thread_event_t;
#endif

#endif /* posix end */


#define QVOD_TIMEOUT_THRESHOLD 300


/* func */
int QvodCreateThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock);
int QvodCopyThreadEvent(qvod_thread_event_t **d_event, qvod_thread_event_t *s_event, pthread_mutex_t** d_lock, pthread_mutex_t* s_lock);
int QvodResetThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock);
int QvodDestroyThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock);
int QvodWaitSingleThreadEvent(qvod_thread_event_t *t_event,
							  QVOD_DWORD milliseconds, pthread_mutex_t* ptlock);
int QvodSetThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock);

#ifdef WIN32 /* WIN32 */

int QvodWaitMultiThreadEvent(int event_num,
							 qvod_thread_event_t t_event[], /* HANDLE */
							 int milliseconds,
							 int if_set[], /* return value */
							 bool if_all);

#else /* posix */

int QvodWaitMultiThreadEvent(int event_num,
							 qvod_thread_event_t* t_event[], /* sem_t */
							 int milliseconds,
							 int if_set[], /* return value */
							 bool if_all,
							 pthread_mutex_t* ptlock[]);

#endif /* posix end */






#endif /* _QVOD_EVENT_H_ */


