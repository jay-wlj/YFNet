
/*
 * QVOD Net.
 */

#include "qvod_sock.h"


//extern int errno;


int	QvodNetGetLastError()
{

#ifdef WIN32 /* WIN32 */

	return WSAGetLastError();

#else /* posix */

	return errno;

#endif /* posix end */

}


int QvodCloseSocket(qvod_sock_t sock)
{
#ifdef WIN32 /* WIN32 */

	return closesocket(sock);

#else /* posix */

    //QVOD_DEBUG("################################### close %d\n", sock);
	return close(sock);

#endif /* posix end */
}


int QvodSetNonblocking(qvod_sock_t sock)
{
	int opts;

#ifdef WIN32 /* WIN32 */

	QVOD_DWORD flag = 1;
	opts = ioctlsocket(sock, FIONBIO, &flag);
	if(opts == SOCKET_ERROR) {
		return QVOD_ERROR;
	}

#else /* posix */

    opts = fcntl(sock, F_GETFL);
    if(opts < 0) {
        return QVOD_ERROR;
    }

    opts |= O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0) {
        return QVOD_ERROR;
    }

#endif /* posix end */

	return QVOD_OK;
}


int QvodSetBlocking(qvod_sock_t sock)
{
	int opts;

#ifdef WIN32 /* WIN32 */

	QVOD_DWORD flag = 0;
	opts = ioctlsocket(sock, FIONBIO, &flag);
	if(opts == SOCKET_ERROR) {
		return QVOD_ERROR;
	}

#else /* posix */

    opts = fcntl(sock, F_GETFL);
    if(opts < 0) {
        return QVOD_ERROR;
    }

	opts &= ~O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0) {
        return QVOD_ERROR;
    }

#endif /* posix end */

	return QVOD_OK;
}

int QvodSetSockTimeout(qvod_sock_t sock, int level, int optname, QVOD_DWORD dwMilliseconds)
{
#ifdef WIN32
	QVOD_DWORD iTimeout = dwMilliseconds;
	setsockopt(sock,level,optname,(const char*)&iTimeout,sizeof(QVOD_DWORD));
#else
	struct timeval nMs;
	nMs.tv_sec = dwMilliseconds/1000;
	nMs.tv_usec = (dwMilliseconds%1000) * 1000;
	setsockopt(sock, level, optname, (char*)&nMs, sizeof(struct timeval));
#endif
	return QVOD_OK;
}


