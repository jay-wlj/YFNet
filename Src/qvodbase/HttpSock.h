#pragma once
#include "qvod_sock.h"
#include "common-RefCountedObject.h"
using namespace std;

#ifndef SafeDeleteArray
#define SafeDeleteArray(p)		 if(p){delete []p; p = NULL;}
#endif
#ifndef SafeDelete
#define SafeDelete(pObject)				if(pObject) {delete pObject; pObject = NULL;}
#endif

typedef struct _HTTPSOCK_DATA
{
public:
	virtual ~_HTTPSOCK_DATA(){}

}HttpSockData, *PHttpSockData;


class CHttpSock : public RefCountedObject
{
public:
	CHttpSock(void);
	CHttpSock(qvod_sock_t sock);
	virtual ~CHttpSock(void);

	CHttpSock& operator=(const CHttpSock& rhs);
	CHttpSock& operator=(const qvod_sock_t& rhs);
	operator qvod_sock_t&() { return m_socket;}
	qvod_sock_t& GetSocket() { return m_socket; }

	void SetData(PHttpSockData pData);
	PHttpSockData GetData();

	virtual int Connect(const unsigned long ipv4, const struct in6_addr& ipv6, int nPort, bool isipv6);
	virtual int Connect(const struct sockaddr_in6& svrAddr);
	virtual int Connect(const qvod_sockaddr_t& SvrAddr);
	virtual int SendData(const char* pBuf, const unsigned int& len);
	virtual int RecvData(char* pBuf, unsigned int len);

	void Close();
protected:
	qvod_sock_t	m_socket;		// socket套接字
	PHttpSockData m_udata;		// 绑定的数据
	//void * m_udata;					// 绑定的数据
};
