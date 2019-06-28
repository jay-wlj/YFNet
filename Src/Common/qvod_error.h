
/*
 * QVOD Error.
 */

#ifndef _QVOD_ERROR_H_
#define _QVOD_ERROR_H_


#ifdef WIN32

#include <windows.h>
#include <assert.h>

#define QVOD_WAIT_TIMEOUT	WAIT_TIMEOUT
#define QVOD_WAIT_FAILED	WAIT_FAILED

typedef int qvod_err_t;

#else /* posix */

#include <errno.h>
#include <assert.h>

#define QVOD_WAIT_TIMEOUT	ETIMEDOUT
#define QVOD_WAIT_FAILED	-1

typedef int qvod_err_t;

//extern int errno;

#endif


/* func */
qvod_err_t QvodGetLastError();




#endif /* _QVOD_ERROR_H_ */


