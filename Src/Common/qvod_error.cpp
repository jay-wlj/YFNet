
/*
 * QVOD Error.
 */

#include "qvod_error.h"



qvod_err_t QvodGetLastError()
{
#ifdef WIN32

	return GetLastError();

#else /* posix */

	return errno;

#endif
}


