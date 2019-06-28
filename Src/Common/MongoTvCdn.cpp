#include "MongoTvCdn.h"
#include "common-commonfun.h"
#include "common-utility.h"
#include "qvod_sock.h"
#include "qvod_error.h"

#define OTHER_CDN_HOST	"stat.titan.imgo.tv"
CMongoTvCdn* CMongoTvCdn::m_instance = NULL;
CMongoTvCdn* CMongoTvCdn::Instance()
{
	if (!m_instance)
	{
		m_instance = new CMongoTvCdn;
	}
	return m_instance;
}

void CMongoTvCdn::FreeInstance()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

CMongoTvCdn::CMongoTvCdn()
{
	m_bRunning = false;
	m_hThread = NULL;
	QvodCreateThreadEvent(&m_event,&m_eventLock);
}

CMongoTvCdn::~CMongoTvCdn()
{
	m_reqVec.clear();
}

void CMongoTvCdn::Stop()
{
	if (!m_bRunning)
	{
		return ;
	}
	m_bRunning = false;
	if (m_hThread)
	{
		QvodWaitForThread(m_hThread, NULL, 1000);
		m_hThread = NULL;
	}

	QvodDestroyThreadEvent(&m_event,&m_eventLock);
}

bool CMongoTvCdn::AddRequest( const char* url,bool bIsOtherCdn,int type )
{
	CAutoLock lock(m_lock);
	if (!m_bRunning)
	{
		 QvodCreateThread(&m_hThread, (qvod_routine_pt)Routine, this);
		if (!m_hThread)
		{
			return false;
		}
	}
	SMongoTvCdnReq req;
	req.strUrl = url;
	req.type = type;
	req.bIsOtherCdn = bIsOtherCdn;
	m_reqVec.push_back(req);
	QvodSetThreadEvent(&m_event,&m_eventLock);
	return true;
}

QVOD_THREAD_RETURN CMongoTvCdn::Routine( void* para )
{
	CMongoTvCdn* pThis = (CMongoTvCdn*)para;
	pThis->DoRoutine();
#ifndef WIN32
    return 0;
#endif
}

void CMongoTvCdn::DoRoutine()
{
	m_bRunning = true;
	while (m_bRunning)
	{
		int result =QvodWaitSingleThreadEvent(&m_event,10,&m_eventLock);
		if (QVOD_WAIT_FAILED == result || QVOD_WAIT_TIMEOUT == result)
		{
			continue;
		}
		m_lock.Lock();
		vector<SMongoTvCdnReq> vec = m_reqVec;
		m_reqVec.clear();
		m_lock.Unlock();
		for (vector<SMongoTvCdnReq>::iterator iter = vec.begin(); iter != vec.end() && m_bRunning; ++iter)
		{
			string strHost;
			char szSend[4096] = {0};
			if (iter->bIsOtherCdn)
			{
				strHost = OTHER_CDN_HOST;
				int iEncodeLen = iter->strUrl.size()*4;
				char* pEncode = new char[iEncodeLen];
				memset(pEncode, 0, iEncodeLen);
				urlEncode(iter->strUrl.c_str(), iter->strUrl.size(), pEncode, iEncodeLen);
				string strUrlEncode = pEncode;
				delete[] pEncode;
				if (KEEPALIVE == iter->type)
				{
					sprintf_s(szSend,sizeof(szSend),"GET /play/heartbeat.do?url=%s HTTP/1.1\r\nHost: %s\r\n\r\n",strUrlEncode.c_str(),strHost.c_str());
				}
				else	if (LOGOUT == iter->type)
				{
					sprintf_s(szSend,sizeof(szSend),"GET /play/offline.do?url=%s HTTP/1.1\r\nHost: %s\r\n\r\n",strUrlEncode.c_str(),strHost.c_str());
				}
				else	if (START == iter->type)
				{
					sprintf_s(szSend,sizeof(szSend),"GET /play/start.do?url=%s HTTP/1.1\r\nHost: %s\r\n\r\n",strUrlEncode.c_str(),strHost.c_str());
				}
				else
				{
					continue;
				}
			}
			else
			{
				//取uuid
				int iUuidPos = iter->strUrl.find("uuid=");
				if (string::npos == iUuidPos)
				{
					continue;
				}
				int iUuidEndPos = iter->strUrl.find("&",iUuidPos);
				if (string::npos != iUuidEndPos)
				{
					iUuidEndPos -= iUuidPos;
				}
				string strUuid = iter->strUrl.substr(iUuidPos,iUuidEndPos);
				//取host
				int iHttpPos = iter->strUrl.find("http://");
				if (string::npos == iHttpPos)
				{
					continue;
				}
				iHttpPos += strlen("http://");
				int iHttpEndPos = iter->strUrl.find("/",iHttpPos);
				if (string::npos == iHttpEndPos)
				{
					continue;
				}
				strHost = iter->strUrl.substr(iHttpPos,iHttpEndPos-iHttpPos);
				//组装请求				
				if (KEEPALIVE == iter->type)
				{
					sprintf_s(szSend,sizeof(szSend),"GET /heartbeat.do?%s HTTP/1.1\r\nHost: %s\r\n\r\n",strUuid.c_str(),strHost.c_str());
				}
				else	if (LOGOUT == iter->type)
				{
					sprintf_s(szSend,sizeof(szSend),"GET /offline.do?%s HTTP/1.1\r\nHost: %s\r\n\r\n",strUuid.c_str(),strHost.c_str());
				}
				else
				{
					continue;
				}
			}		
			
			int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			if (QVOD_INVALID_SOCKET == sock)
			{
				continue;
			}
			int iTimeout = 2000;
			QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, iTimeout);
			QvodSetSockTimeout(sock, SOL_SOCKET, SO_SNDTIMEO, iTimeout);
			//setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&iTimeout,sizeof(iTimeout));
			//setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(const char*)&iTimeout,sizeof(iTimeout));
			unsigned short port = 80;
			if (strHost.find(":") != string::npos)
			{
				port = atoi(strHost.substr(strHost.find(":")+1).c_str());
			}
			qvod_sockaddr_t addr = {0};
			addr.sin_addr.s_addr = GetHostByName(strHost.c_str());
			addr.sin_port = htons(port);
			addr.sin_family = AF_INET;
			if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == QVOD_SOCKET_ERROR)
			{
				QvodCloseSocket(sock);
				continue;
			}
#ifdef QVODDEBUG
			_OUTPUT(INFO,"%s",szSend);
#endif
			send(sock,szSend,strlen(szSend),0);
			memset(szSend, 0, sizeof(szSend));
			recv(sock,szSend,sizeof(szSend),0);
#ifdef QVODDEBUG
			_OUTPUT(INFO,"%s",szSend);
#endif
			QvodCloseSocket(sock);
		}
	}
}

