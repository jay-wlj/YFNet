
/*
 * QVOD Thread.
 */

#include "qvod_string.h"
#include "qvod_thread.h"
#include "qvod_error.h"
#include "common-commonfun.h"



/* tid and arg may be NULL */
int QvodCreateThread(qvod_thread_t *tid, qvod_routine_pt routine, void *arg)
{
#ifdef WIN32 /* WIN32 */

	qvod_thread_t res;

	/* ignore tid */
	res = _beginthread(routine, 0, arg);
	if(res == -1) {
		/* error */
		return QVOD_ERROR;
	}

	/* ok */
	if(tid != NULL) {
		*tid = res;
	}

#else /* posix */

	int res;
	qvod_thread_t tid_tmp;
	pthread_attr_t tattr;

	res = pthread_attr_init(&tattr);
	if(res != 0) {
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"pthread_attr_init error\n");
#endif
		return QVOD_ERROR;
	}

//	res = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
//	if(res != 0) {
//		/* error */
//		QVOD_DEBUG("pthread_attr_setdetachstate error\n");
//		return QVOD_ERROR;
//	}

	res = pthread_create(&tid_tmp, &tattr, routine, arg);
	if(res != 0) {
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"QvodCreateThread return %d, error: %d\n", res, errno);
#endif
		return QVOD_ERROR;
	}

	res = pthread_attr_destroy(&tattr);
	if(res != 0) {
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"pthread_attr_destroy error\n");
#endif
		return QVOD_ERROR;
	}

	/* ok */
	if(tid != NULL) {
		*tid = tid_tmp;
	}

#endif /* posix end */

	return QVOD_OK;
}

int QvodWaitForThread(qvod_thread_t tid, void **value_ptr, QVOD_DWORD dwMillSeconds)
{
	int res;

#ifdef WIN32

	/*
	 * win32 do not check the return value of the thread,
	 * so you should set value_ptr NULL.
	 */
	res = WaitForSingleObject((HANDLE)tid, dwMillSeconds);
	if(res == WAIT_FAILED) {
		return QVOD_ERROR;
	}

#else /* posix */

	if (0 == tid)
	{
		return QVOD_OK;
	}

	res = pthread_join(tid, value_ptr);
	if(res != 0) {
		return QVOD_ERROR;
	}

#endif

	return QVOD_OK;
}


int QvodWaitForMultiThreads(int nCount, const qvod_thread_t *handles)
{
	int ret = QVOD_OK;
	//(void*)&ret;

#ifdef WIN32 /* WIN32 */

	int res;

	res = WaitForMultipleObjects(nCount, (HANDLE*)handles, true, INFINITE);
	if(res == WAIT_FAILED) {
		return QVOD_ERROR;
	}

#else /* posix */

	int res, i;

	for(i = 0; i < nCount; ++i) {
		if (0 != handles[i]){
			res = pthread_join(handles[i], NULL);
			if(res != 0) {
				ret = QVOD_ERROR;
			}
		}
	}

#endif /* posix end */

	return ret;
}


