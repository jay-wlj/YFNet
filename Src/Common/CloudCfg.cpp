#include "CloudCfg.h"
#include "common-commonfun.h"
#include "global.h"
#include "common-utility.h"
#include "json.h"
#include "CfgHost.h"

#define CLOUDCFG_VERSION	101

static char get_fmt_of_STANDARD[] = 
"GET %s HTTP/1.1\r\n"
"Accept: */*\r\n"
"Accept-Language: zh-cn\r\n"
#ifdef WIN32
"User-Agent: Yunfan Pc %s\r\n"
#elif MACOSX
"User-Agent: Yunfan Ios %s\r\n"
#else
"User-Agent: Yunfan Android %s\r\n"
#endif
"Host: %s\r\n"
"Connection: Keep-Alive\r\n\r\n";

CCloudCfg* CCloudCfg::m_instance = NULL;

CCloudCfg::CCloudCfg():
m_hThread(NULL)
{
	m_bExit = false;
}
CCloudCfg::~CCloudCfg()
{
}

CCloudCfg* CCloudCfg::Instance()
{
	if (NULL == m_instance)
	{
		m_instance = new CCloudCfg();
	}
	return m_instance;
}

void CCloudCfg::FreeInstance()
{
	SafeDelete(m_instance);
}

bool CCloudCfg::OpenTcpSocket(qvod_sock_t& sock)
{
	if (QVOD_INVALID_SOCKET != sock)
	{
		return true;
	}
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(QVOD_INVALID_SOCKET == sock)
	{
		return false;
	}

	int iTimeOut = 5000;
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, iTimeOut);
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_SNDTIMEO, iTimeOut);

	unsigned long ip = inet_addr(CLOUDCFGSERVERIP);
	GetIpFromDomain(ip);
	qvod_sockaddr_t remoteaddr = {0};
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = ip;
	remoteaddr.sin_port = htons(8054);

	int ret = connect(sock, (sockaddr*)&remoteaddr, sizeof(remoteaddr));
	if (QVOD_SOCKET_ERROR == ret)
	{
		QvodCloseSocket(sock);
		sock = QVOD_INVALID_SOCKET;
		return false;
	}
	return true;
}

bool CCloudCfg::CloseTcpSocket(qvod_sock_t& sock)
{
	if (QVOD_INVALID_SOCKET != sock)
	{
		QvodCloseSocket(sock);
		sock = QVOD_INVALID_SOCKET;
	}
	return true;
}

void CCloudCfg::Init()
{
	QvodCreateThread(&m_hThread, (qvod_routine_pt)ThreadProc, this);
}

void CCloudCfg::UnInit()
{
	m_bExit = true;
	QvodWaitForThread(m_hThread, NULL, 100);
	m_hThread = NULL;
}

void CCloudCfg::ThreadProc(QVOD_LPVOID lpParam)
{
	CCloudCfg *pThis = (CCloudCfg*)lpParam;
	
	string strRet;

}

bool CCloudCfg::GetConfigValue(qvod_sock_t& sock, unsigned short key,int& value)
{
	if (QVOD_INVALID_SOCKET == sock)
	{
		return false;
	}
	CloudCfgReq creq;
	int reqlen = sizeof(creq);
	memset((char*)&creq, 0, reqlen);
	creq.len = htons(reqlen);
	creq.key = htons(key);
	creq.version = htons(CLOUDCFG_VERSION);	
	int ret = send(sock, (char*)&creq, reqlen, 0);
	if (QVOD_SOCKET_ERROR == ret)
	{
#ifdef _DEBUG
		_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,WSAGetLastError());
#endif
		return false;
	}
	char recvbuf[1024] = {0};
	ret = recv(sock, recvbuf, sizeof(recvbuf), 0);
	if (ret < sizeof(CloudCfgRsp))
	{
#ifdef _DEBUG
		_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,WSAGetLastError());
#endif
		return false;
	}
	CloudCfgRsp* prsp = (CloudCfgRsp*)recvbuf;
	if (ntohs(prsp->key) != key)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"%s:%s_%d key = %d,retkey = %d\n",__FILE__,__FUNCTION__,__LINE__,key,ntohs(prsp->key));
#endif
		return false;
	}
	unsigned short valuelen = ntohs(prsp->len) - sizeof(CloudCfgRsp);
	int recvlen = 0;
	char* valbuf = new (std::nothrow) char[valuelen];
	if (NULL == valbuf)
	{
#ifdef _DEBUG
		_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,GetLastError());
#endif
		return false;
	}
	memcpy(valbuf+recvlen, recvbuf+sizeof(CloudCfgRsp), ret-sizeof(CloudCfgRsp));
	recvlen += ret - sizeof(CloudCfgRsp);
	while (recvlen < valuelen)
	{
		ret = recv(sock, recvbuf, sizeof(recvbuf), 0);
		if ( ret <= 0)
		{
#ifdef _DEBUG
			_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,WSAGetLastError());
#endif
			break;
		}
		memcpy(valbuf+recvlen, recvbuf, min(ret,valuelen-recvlen));
		recvlen += ret;
	}
	if (recvlen == valuelen && 4 == valuelen)
	{
		memcpy(&value, valbuf, sizeof(value));
		value = ntohl(value);
		delete[] valbuf;
#ifdef QVODDEBUG
		_OUTPUT(INFO,"key %d value = %d\n",key,value);
#endif
		return true;
	}
#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"%s:%s_%d recvlen = %d,valuelen = %d\n",__FILE__,__FUNCTION__,__LINE__,recvlen,valuelen);
#endif
	delete[] valbuf;
	return false;
}

int CCloudCfg::GetIpFromDomain(unsigned long& ip)
{
	unsigned long iIP = GetHostByName(CLOUDCFGSERVER);
	if (0 == iIP)
	{
		return -1;
	}
	ip = iIP;
	return 0;
}

int CCloudCfg::GetCloudCfg( unsigned short key,int& value )
{
	int ret = 0;
	qvod_sock_t sock = QVOD_INVALID_SOCKET;
	if (OpenTcpSocket(sock))
	{
		if (GetConfigValue(sock,key,value))
		{
			ret = 1;
		}
	}
	else
	{
		return ret;
	}
	CloseTcpSocket(sock);
	return ret;
}

int CCloudCfg::GetCloudCfg( unsigned short iBeginKey,unsigned short iEndKey,int* pValueArray )
{
	if (iEndKey < iBeginKey)
	{
		return 0;
	}
	qvod_sock_t sock = QVOD_INVALID_SOCKET;
	if (OpenTcpSocket(sock))
	{
		for (unsigned short i = iBeginKey,j = 0; i <= iEndKey; ++i,++j)
		{
			if (!GetConfigValue(sock,i,pValueArray[j]))
			{
				CloseTcpSocket(sock);
				return -1;
			}
		}
	}
	else
	{
		return -2;
	}
	CloseTcpSocket(sock);
	return 1;
}

int CCloudCfg::GetCloudCfg( unsigned short key,string& strValue )
{
	int iRet = 0;
	qvod_sock_t sock = QVOD_INVALID_SOCKET;
	if (OpenTcpSocket(sock))
	{
		do 
		{
			CloudCfgReq creq;
			int reqlen = sizeof(creq);
			memset((char*)&creq, 0, reqlen);
			creq.len = htons(reqlen);
			creq.key = htons(key);
			creq.version = htons(CLOUDCFG_VERSION);	
			int ret = send(sock, (char*)&creq, reqlen, 0);
			if (QVOD_SOCKET_ERROR == ret)
			{
#ifdef _DEBUG
				_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,WSAGetLastError());
#endif
				break;
			}
			char recvbuf[1024] = {0};
			ret = recv(sock, recvbuf, sizeof(recvbuf), 0);
			if (ret < sizeof(CloudCfgRsp))
			{
#ifdef _DEBUG
				_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,WSAGetLastError());
#endif
				break;
			}
			CloudCfgRsp* prsp = (CloudCfgRsp*)recvbuf;
			if (ntohs(prsp->key) != key)
			{
#ifdef QVODDEBUG
				_OUTPUT(ERROR1,"%s:%s_%d key = %d,retkey = %d\n",__FILE__,__FUNCTION__,__LINE__,key,ntohs(prsp->key));
#endif
				break;
			}
			unsigned short valuelen = ntohs(prsp->len) - sizeof(CloudCfgRsp);
			int recvlen = 0;
			char* valbuf = new (std::nothrow) char[valuelen];
			if (NULL == valbuf)
			{
#ifdef _DEBUG
				_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,GetLastError());
#endif
				break;
			}
			memcpy(valbuf+recvlen, recvbuf+sizeof(CloudCfgRsp), ret-sizeof(CloudCfgRsp));
			recvlen += ret - sizeof(CloudCfgRsp);
			while (recvlen < valuelen)
			{
				ret = recv(sock, recvbuf, sizeof(recvbuf), 0);
				if ( ret <= 0)
				{
#ifdef _DEBUG
					_OUTPUT(ERROR1,"%s:%s_%d error = %d\n",__FILE__,__FUNCTION__,__LINE__,WSAGetLastError());
#endif
					break;
				}
				memcpy(valbuf+recvlen, recvbuf, min(ret,valuelen-recvlen));
				recvlen += ret;
			}
			if (recvlen == valuelen && valuelen > 0)
			{					
				strValue = string(valbuf,valuelen);
				delete[] valbuf;
				iRet = 1;
				break;
			}
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"%s:%s_%d recvlen = %d,valuelen = %d\n",__FILE__,__FUNCTION__,__LINE__,recvlen,valuelen);
#endif
			delete[] valbuf;
		} while (false);		
	}
	else
	{
		return iRet;
	}
	CloseTcpSocket(sock);
	return iRet;
}


