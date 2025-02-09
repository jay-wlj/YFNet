
/*
 * QVOD Time.
 */

#include "qvod_time.h"
#include "qvod_string.h"
#include "common-commonfun.h"

#ifndef WIN32 /* WIN32 */
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

int QvodSleep(unsigned int milliseconds)
{
#ifdef WIN32 /* WIN32 */

	Sleep(milliseconds);
	return 0;

#else /* posix */

	/* form time struct */
	struct timeval interval;
	interval.tv_sec = milliseconds / 1000;
	interval.tv_usec = (milliseconds % 1000) * 1000;

	int res = select(0, NULL, NULL, NULL, &interval);
	if(res != 0)
	{
		/* not timeout, error */
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"%s(%d)-%s: QvodSleep select not timeout\n", __FILE__, __LINE__, __FUNCTION__);
#endif
		return -1;
	}

	return 0;

#endif /* posix end */
}

/* return milliseconds */
long QvodGetTime()
{
#ifdef WIN32 /* WIN32 */

	/*
	 * the number of milliseconds that
	 * have elapsed since the system was started.
	 */
	return GetTickCount(); /* milliseconds */

#else /* posix */

	int res;

#ifdef MACOSX
	struct timeval sNow;
	gettimeofday(&sNow, NULL);
	return sNow.tv_sec*1000 + sNow.tv_usec/1000;
#else
	struct timespec sNow;
	res = clock_gettime(CLOCK_MONOTONIC, &sNow);
	if(res != 0)
	{
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"%s(%d)-%s: clock_gettime error\n", __FILE__, __LINE__, __FUNCTION__);
#endif
		return -1;
	}

	return sNow.tv_sec * 1000 + sNow.tv_nsec / 1000000; /* milliseconds */
#endif

#endif /* posix end */

}


int QvodQueryPerformanceCounter(QVOD_LARGE_INTEGER *lpPerformanceCount)
{
	int res;

#ifdef WIN32 /* WIN32 */

	res = QueryPerformanceCounter(lpPerformanceCount);
	if(res == 0) {
		/* failed */
		return QVOD_ERROR;
	}

#else /* posix */

	struct timeval tv;
	//QVOD_LONGLONG msec;

	res = gettimeofday(&tv, NULL);
	if(res != 0) {
		/* failed */
		return QVOD_ERROR;
	}
	lpPerformanceCount->QuadPart = tv.tv_sec * 1000000 + tv.tv_usec;

#endif /* posix end */

	return QVOD_OK;
}


/* return time string */
int QvodGetTimeStr(char *dst_buf)
{
#ifdef WIN32 /* WIN32 */

    SYSTEMTIME st;

    GetLocalTime(&st);

	sprintf(dst_buf," %04d-%02d-%02d %02d:%02d:%02d.%03d ",
			st.wYear, st.wMonth, st.wDay, st.wHour,
			st.wMinute, st.wSecond, st.wMilliseconds);

#else /* posix */

	time_t t;
	struct tm st;

	t = time(NULL);
	localtime_r(&t, &st);

	sprintf(dst_buf," %04d-%02d-%02d %02d:%02d:%02d.%03d ",
			st.tm_year + 1900, st.tm_mon + 1, st.tm_wday, st.tm_hour,
			st.tm_min, st.tm_sec, 0);

#endif /* posix end */

	return QVOD_OK;
}



