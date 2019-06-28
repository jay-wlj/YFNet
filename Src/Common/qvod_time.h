
/*
 * QVOD Basetypes.
 */

#ifndef _QVOD_TIME_H_
#define _QVOD_TIME_H_


#include "qvod_basetypes.h"


#ifdef WIN32 /* WIN32 */

#include <windows.h>
#include <time.h>

#define	QVOD_SLEEP(a) Sleep(a)

#else /* posix */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

#define	QVOD_SLEEP(a) (usleep(1000*(a))) /* in this way, "a" should smaller than 1000 */

#endif /* posix end */


/* function */
int QvodSleep(unsigned int milliseconds);
long QvodGetTime();
int QvodQueryPerformanceCounter(QVOD_LARGE_INTEGER *lpPerformanceCount);
int QvodGetTimeStr(char *dst_buf);


#endif /* _QVOD_TIME_H_ */

