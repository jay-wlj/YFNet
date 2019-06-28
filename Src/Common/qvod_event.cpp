
/*
 * QVOD Event.
 */

#include "qvod_basetypes.h"
#include "qvod_event.h"
#include "qvod_error.h"


#ifdef WIN32 /* WIN32 */

#include <windows.h>

#else /* posix */

#include "qvod_error.h"
#include "qvod_time.h"
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
//#include <time.h>
//#include <sys/time.h>
#include "common-commonfun.h"

#endif /* posix end */



int QvodCreateThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock)
{
#ifdef WIN32 /* WIN32 */

	*t_event = CreateEvent(NULL, false, false, NULL);
	if(*t_event == NULL) {
		/* failed */
		printf("%s(%d)-%s: CreateEvent() failed\n", __FILE__, __LINE__, __FUNCTION__);
		return QVOD_ERROR;
	}

#else /* posix */

	int res;
#ifdef MACOSX
	res = pthread_cond_init(t_event, NULL);
	if(res != 0)
	{
		printf("%s(%d)-%s: pthread_cond_init() failed %d: %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return QVOD_ERROR;
	}
	res = pthread_mutex_init(ptlock, NULL);
	if(res != 0)
	{
		printf("%s(%d)-%s: pthread_mutex_init() failed %d: %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return QVOD_ERROR;
	}
#else
	res = sem_init(t_event, 0, 0);
	if(res != 0) {
		/* error */
		printf("%s(%d)-%s: sem_init() failed %d: %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return QVOD_ERROR;
	}
#endif

#endif /* posix end */

	return QVOD_OK;
}

/*
 * First you should define "qvod_thread_event_t *d_event" and "qvod_thread_event_t s_event",
 * then call this function like this:
 * 							QvodCopyThreadEvent(&d_event, &s_event);
 */
int QvodCopyThreadEvent(qvod_thread_event_t **d_event, qvod_thread_event_t *s_event, pthread_mutex_t** d_lock, pthread_mutex_t* s_lock)
{
	if(d_event == NULL || s_event == NULL) {
		/* error */
		return QVOD_ERROR;
	}

#ifdef WIN32 /* WIN32 */

	/* copy handle(int) */
	if(*s_event == NULL || *d_event == NULL/* null-pointer */) {
		return QVOD_ERROR;

	} else {

		**d_event = *s_event;
		return QVOD_OK;
	}
	
#else /* posix */
	
#ifdef MACOSX
	*d_lock = s_lock;
#endif

	/* only copy pointer */
	*d_event = s_event;
	return QVOD_OK;

#endif /* posix end */
}


int QvodResetThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock)
{
#ifdef WIN32 /* WIN32 */

	bool res = ResetEvent(*t_event);
	if(res == 0) {
		/* failed */
		printf("%s(%d)-%s: ResetEvent() failed\n", __FILE__, __LINE__, __FUNCTION__);
		return QVOD_ERROR;
	}

#else /* posix */

	int res;
#ifdef MACOSX
	res = pthread_cond_destroy(t_event);
	if(EBUSY == res)
	{
		printf("%s(%d)-%s: event reset failed %d: %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return QVOD_ERROR;
	}
	res = pthread_mutex_destroy(ptlock);
	if(EBUSY == res)
	{
		printf("%s(%d)-%s: lock reset failed %d: %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
	}
	
	res = pthread_cond_init(t_event, NULL);
	if(res != 0)
	{
		printf("%s(%d)-%s: pthread_cond_init() failed %d: %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return QVOD_ERROR;
	}
	res = pthread_mutex_init(ptlock, NULL);
	if(res != 0)
	{
		printf("%s(%d)-%s: pthread_mutex_init() failed %d: %s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return QVOD_ERROR;
	}
	
#else
	res = sem_destroy(t_event);
	if(res != 0) {
		/* error */
		printf("%s(%d)-%s: sem_destroy() failed\n", __FILE__, __LINE__, __FUNCTION__);
		return QVOD_ERROR;
	}

	/* Initialising a semaphore that has already been initialised results in undefined behaviour */
	res = sem_init(t_event, 0, 0);
	if(res != 0) {
		/* error */
		printf("%s(%d)-%s: sem_init() failed\n", __FILE__, __LINE__, __FUNCTION__);
		return QVOD_ERROR;
	}
#endif

#endif /* posix end */

	return QVOD_OK;
}


int QvodDestroyThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock)
{
#ifdef WIN32

	bool res;

	res = CloseHandle(*t_event);
	if(res == 0) {
		/* failed */
		printf("%s(%d)-%s: CloseHandle() failed\n", __FILE__, __LINE__, __FUNCTION__);
		return QVOD_ERROR;
	}

#else /* posix */

	int res;
#ifdef MACOSX
	pthread_cond_destroy(t_event);
	pthread_mutex_destroy(ptlock);
#else
	res = sem_destroy(t_event);
	if(res != 0) {
		/* error */
		printf("%s(%d)-%s: sem_destroy() failed\n", __FILE__, __LINE__, __FUNCTION__);
		return QVOD_ERROR;
	}
#endif
	
#endif

	return QVOD_OK;
}

int QvodWaitSingleThreadEvent(qvod_thread_event_t *t_event, QVOD_DWORD milliseconds, pthread_mutex_t* ptlock)
{
	int res;

#ifdef WIN32 /* WIN32 */

	res = WaitForSingleObject(*t_event, milliseconds);
	if(res == WAIT_FAILED) {
		return QVOD_WAIT_FAILED;
	}
	if(res == WAIT_TIMEOUT) {
		return QVOD_WAIT_TIMEOUT;
	}

#else /* posix */

#ifdef MACOSX
	if(QVOD_INFINITE == milliseconds)
	{
		pthread_mutex_lock(ptlock);
		res = pthread_cond_wait(t_event, ptlock);
		pthread_mutex_unlock(ptlock);
	}
	else 
	{
        struct timeval now;
        gettimeofday(&now, NULL);
        struct timespec ts = {0};
        ts.tv_sec = now.tv_sec + milliseconds/1000;
        ts.tv_nsec = now.tv_usec*1000 + (milliseconds%1000)*1000000;
        if (ts.tv_nsec >= 1000000000)   //tv_nsec must be less than 1000000000,otherwise EINVAL happens
        {
            ts.tv_sec += ts.tv_nsec/1000000000;
            ts.tv_nsec %= 1000000000;
        }
		pthread_mutex_lock(ptlock);
		res = pthread_cond_timedwait(t_event, ptlock, &ts);
		pthread_mutex_unlock(ptlock);
	}
	if(0 != res)
	{
		if(ETIMEDOUT == res || ETIMEDOUT == errno)
		{
			return QVOD_WAIT_TIMEOUT;
		}
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "%s(%d)-%s: QvodWaitSingleThreadEvent failed %s, errno %d, res %d\n", __FILE__, __LINE__, __FUNCTION__, strerror(errno), errno, res);
#endif // QVODDEBUG
		return QVOD_WAIT_FAILED;
	}

#else

	if(milliseconds == QVOD_INFINITE) {
		/* infinite wait */

		while(1) {

			res = sem_wait(t_event);
			if(res != 0) {
				/* error */
				if(errno == EINTR) {
					QVOD_SLEEP(10);
					continue;
				}

#ifdef QVODDEBUG
				_OUTPUT(LOG_DEBUG,"%s(%d)-%s: sem_wait() failed, errno = %d\n", __FILE__, __LINE__, __FUNCTION__, errno);
#endif
				return QVOD_WAIT_FAILED;

			} else {

				/* ok, return */
				return QVOD_OK;
			}

		} /* while */

	} else {

		/* get time */
		struct timeval from;
		res = gettimeofday(&from, NULL);
		if(res != 0) {
#ifdef QVODDEBUG
			_OUTPUT(LOG_DEBUG,"%s(%d)-%s: gettimeofday() failed\n", __FILE__, __LINE__, __FUNCTION__);
#endif
			return QVOD_WAIT_FAILED;
		}

		while(1) {

			res = sem_trywait(t_event);
			if(res == 0) {
				/* ok, return */
				return QVOD_OK;

			} else {

				/* abnormal */
				if(errno != EAGAIN) {

					if(errno == EINTR) {

						QVOD_SLEEP(10);
						continue;
					}

					/* error */
#ifdef QVODDEBUG
					_OUTPUT(LOG_DEBUG,"%s(%d)-%s: sem_trywait() failed, error: %s\n", __FILE__, __LINE__, __FUNCTION__, strerror(errno));
#endif
					return QVOD_WAIT_FAILED;
				}
			}

			/* EAGAIN, judge if timeout */
			struct timeval now;
			res = gettimeofday(&now, NULL);
			if(res != 0) {
#ifdef QVODDEBUG
				_OUTPUT(LOG_DEBUG,"%s(%d)-%s: gettimeofday() failed\n", __FILE__, __LINE__, __FUNCTION__);
#endif
				return QVOD_WAIT_FAILED;
			}

			if((now.tv_sec * 1000 + now.tv_usec / 1000) > (from.tv_sec * 1000 + from.tv_usec / 1000 + (long)milliseconds))
			{
				/* timeout, return */
				return QVOD_WAIT_TIMEOUT;
			}

			/* sleep for decrease cpu time */
			QVOD_SLEEP(10);

		} /* while */
	}
#endif

#endif /* posix end */

	return QVOD_OK;
}


#ifdef WIN32 /* WIN32 */

int QvodWaitMultiThreadEvent(int event_num,
							 qvod_thread_event_t t_event[], /* HANDLE */
							 int milliseconds,
							 int if_set[], /* return value */
							 bool if_all)

#else /* posix */

int QvodWaitMultiThreadEvent(int event_num,
							 qvod_thread_event_t* t_event[], /* sem_t */
							 int milliseconds,
							 int if_set[], /* return value */
							 bool if_all,
							 pthread_mutex_t* t_lock[])

#endif /* posix end */

{
	int i;
	int res;
	(void)(&if_all);

	/* reset if_set */
	for(i = 0; i < event_num; ++i) {
		if_set[i] =	0;
	}

#ifdef WIN32 /* WIN32 */

	/* ignore "lock[]" param */
	res = WaitForMultipleObjects(event_num, t_event, if_all, milliseconds);
	switch(res) {
	case WAIT_FAILED:
		{
			return QVOD_WAIT_FAILED;
		}
		break;

	case WAIT_TIMEOUT:
		{
			return QVOD_WAIT_TIMEOUT;
		}
		break;
	}

	/* ok */
	res = res - WAIT_OBJECT_0;

	/* check which event is seted, only this turn */
	for(i = res; i < event_num; ++i) {

		res = WaitForSingleObject(t_event[i], 0); /* not timeout */
		if(res == WAIT_FAILED) {
			/* check next event */
			continue;

		} else {
			/* set if_set[] */
			if_set[i] = 1;
		}
	}

#else /* posix */

	/* get now time */
	bool if_have_event = false;
	int if_wait = event_num;
	struct timeval now;
	unsigned int now_i; /* milliseconds */
	unsigned int to_i;	/* milliseconds */


	if(milliseconds != QVOD_INFINITE) {

		res = gettimeofday(&now, NULL);
		if(res != 0) {
			/* error */
#ifdef QVODDEBUG
			printf("%s(%d)-%s: gettimeofday() failed\n", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
			return QVOD_WAIT_FAILED;
		}

		to_i =	now.tv_sec * 1000 +	now.tv_usec / 1000 + milliseconds;
	}


	while(if_wait > 0) {

		/* check event */
		for(i = 0; i < event_num; ++i) {
			//QVOD_DEBUG("QvodWaitMultiThreadEvent i = %d\n", i);

			if(if_set[i] == 0) {
				/* need wait */
#ifdef MACOSX
				struct timespec tm;
				tm.tv_sec = time(NULL)+5;
				tm.tv_nsec = 0;
				pthread_mutex_lock(t_lock[i]);
				res = pthread_cond_timedwait(t_event[i], t_lock[i], &tm);
				pthread_mutex_unlock(t_lock[i]);
#else
				res = sem_trywait(t_event[i]);
#endif
				if(res == 0) 
				{
					/* ok */
					if_have_event = true;
					if_set[i] = 1;
					--if_wait;

				} 
				else if(errno == EAGAIN || ETIMEDOUT == res) 
				{
					/* need try again */
					//QVOD_DEBUG("sem_trywait return EAGAIN\n");

				} else {
					/* error */
#ifdef QVODDEBUG
					_OUTPUT(FAIL, "%s(%d)-%s: QvodWaitMultiThreadEvent failed %s\n", __FILE__, __LINE__, __FUNCTION__, strerror(errno));
#endif
					return QVOD_WAIT_FAILED;
				}


			} /* if */

		} /* for */

		if(if_all == false && if_have_event == true || if_wait <= 0) {
			/* no need to wait the event */
			return QVOD_OK;
		}

		/* check if timeout if needed */
		if(milliseconds != QVOD_INFINITE) {

			res = gettimeofday(&now, NULL);
			if(res != 0) {
#ifdef QVODDEBUG
				_OUTPUT(LOG_DEBUG,"%s(%d)-%s: gettimeofday() failed\n", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
				return QVOD_WAIT_FAILED;
			}

			now_i =	now.tv_sec * 1000 + now.tv_usec / 1000;
			if(now_i >= to_i) {
				/* timeout, quit */
				return QVOD_WAIT_TIMEOUT;
			}
		}

		/* sleep for decrease cpu time */
		QVOD_SLEEP(1);

	} /* while */

#endif /* posix end */


	return QVOD_OK;
}

int QvodSetThreadEvent(qvod_thread_event_t *t_event, pthread_mutex_t* ptlock)
{
#ifdef WIN32 /* WIN32 */

	bool res;

	res = SetEvent(*t_event);
	if(res == 0) {
		/* failed */
		return QVOD_ERROR;
	}

#else /* posix */

	int res;
#ifdef MACOSX
	pthread_mutex_lock(ptlock);
	res = pthread_cond_signal(t_event);
	pthread_mutex_unlock(ptlock);
#else
	res = sem_post(t_event);
#endif
	if(res != 0) 
	{
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "QvodSetThreadEvent failed %s\n", strerror(errno));
#endif
		return QVOD_ERROR;
	}

#endif /* posix end */

	return QVOD_OK;
}



