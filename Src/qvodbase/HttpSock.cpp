#include "StdAfx.h"
#include "HttpSock.h"
#include "common-commonfun.h"

CHttpSock::CHttpSock(void):
m_socket(QVOD_INVALID_SOCKET),
m_udata(NULL)
{
}

CHttpSock::CHttpSock(qvod_sock_t sock):
m_udata(NULL)
{
	m_socket = sock;
}

CHttpSock::~CHttpSock(void)
{
	SafeDelete(m_udata);
	Close();
}

void CHttpSock::SetData(PHttpSockData pData)
{
	SafeDelete(m_udata);
	m_udata = pData;
}
PHttpSockData CHttpSock::GetData()
{
	return m_udata;
}
CHttpSock& CHttpSock::operator=(const CHttpSock& rhs)
{
	if (this != &rhs)
	{
		m_socket = rhs.m_socket;
	}
	SafeDelete(m_udata);
	m_udata = rhs.m_udata;
	return *this;
}

CHttpSock& CHttpSock::operator=(const qvod_sock_t& rhs)
{
	m_socket = rhs;
	return *this;
}

void CHttpSock::Close()
{
#ifdef QVODDEBUG
	if (QVOD_INVALID_SOCKET != m_socket)
	{
		_OUTPUT(INFO, "CHttpSock Close sock=%d", m_socket);
	}
#endif
	if (QVOD_INVALID_SOCKET != m_socket)
	{
		QvodCloseSocket(m_socket);
	}
	m_socket = QVOD_INVALID_SOCKET;
}

int CHttpSock::Connect(const qvod_sockaddr_t& SvrAddr)
{
	int ret = connect(m_socket,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	return ret;
}

int CHttpSock::Connect(const struct sockaddr_in6& svrAddr)
{
	int ret = connect(m_socket, (struct sockaddr*)&svrAddr, sizeof(svrAddr));
	return ret;
}

int CHttpSock::Connect(const unsigned long ipv4, const struct in6_addr& ipv6, int port, bool isipv6)
{
	if (!isipv6)
	{
		qvod_sockaddr_t SvrAddr;
		SvrAddr.sin_addr.s_addr = ipv4;
		SvrAddr.sin_port = port;//htons(port);
		SvrAddr.sin_family = AF_INET;	
		return Connect(SvrAddr);
	}

	struct sockaddr_in6 dest;
	memset(&dest, 0x0, sizeof(sockaddr_in6));
	dest.sin6_family = AF_INET6;		//ipv6
    dest.sin6_port = port; //htons(port);
    dest.sin6_addr = ipv6;
	return Connect(dest);
}


int CHttpSock::SendData(const char* pBuf, const unsigned int& len)
{
	if (NULL == pBuf || 0 == len)
	{
		return 0;
	}
	int ret = send(m_socket, pBuf, len, 0);
	return ret;
}

int CHttpSock::RecvData(char* pBuf, unsigned int len)
{
	int ret = recv(m_socket, pBuf, len, 0);
	return ret;
}

