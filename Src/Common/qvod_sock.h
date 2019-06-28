
/*
 * QVOD Sock.
 */

#ifndef _QVOD_SOCK_H_
#define _QVOD_SOCK_H_

#include "qvod_string.h"


#ifdef WIN32 /* WIN32 */

#include <WS2tcpip.h>
#include <WSPiApi.h>
#include <WinSock2.h>

#define QVOD_INVALID_SOCKET 	INVALID_SOCKET
#define QVOD_SOCKET_ERROR		SOCKET_ERROR
#define QVOD_INADDR_ANY			ADDR_ANY
#define QVOD_NET_EWOULDBLOCK 	WSAEWOULDBLOCK
#define QVOD_NET_ECONNRESET		WSAECONNRESET
#define QVOD_ETIMEOUT			WSAETIMEDOUT
#define QVOD_DISK_FULL			ERROR_DISK_FULL

typedef	SOCKADDR_IN 		qvod_sockaddr_t;
typedef SOCKET				qvod_sock_t;
typedef int					qvod_socklen_t;
typedef IN6_ADDR            qvod_sockaddr_ip6;


#else /* posix */

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
//#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define QVOD_INVALID_SOCKET 	-1
#define QVOD_SOCKET_ERROR		-1
#define QVOD_INADDR_ANY			INADDR_ANY
#define QVOD_NET_EWOULDBLOCK 	EWOULDBLOCK
#define QVOD_NET_ECONNRESET		ECONNRESET
#define QVOD_ETIMEOUT			ETIMEOUT
#define QVOD_DISK_FULL			ENOSPC

typedef struct sockaddr_in 	qvod_sockaddr_t;
typedef int					qvod_sock_t;
typedef socklen_t			qvod_socklen_t;

typedef in6_addr            qvod_sockaddr_ip6;
#endif /* posix end */


int	QvodNetGetLastError();
int QvodCloseSocket(qvod_sock_t sock);
int QvodSetNonblocking(qvod_sock_t sock);
int QvodSetBlocking(qvod_sock_t sock);

int QvodSetSockTimeout(qvod_sock_t sock, int level, int optname, QVOD_DWORD dwMilliseconds);

#endif /* _QVOD_SOCK_H_ */

