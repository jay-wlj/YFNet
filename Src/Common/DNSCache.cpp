// DNSCache.cpp: implementation of the CDNSCache class.
//
//////////////////////////////////////////////////////////////////////

#include "common-DNSCache.h"
#include "qvod_string.h"
#include "qvod_sock.h"
#include "common-commonfun.h"
#include "qvod_lock.h"
#include "AutoPtr.h"

CDNSCache* CDNSCache::m_dnsCache = NULL;
extern bool g_bRunning;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDNSCache::CDNSCache()
{
	QvodCreateThread(NULL,ThreadProc, this);
	QvodCreateThreadEvent(&m_hEvent, &m_hEventLock);
	//QvodSetThreadEvent(&m_hEvent, &m_hEventLock); /* silver add */
	//QvodSleep(500);
}

CDNSCache::~CDNSCache()
{
	QvodDestroyThreadEvent(&m_hEvent, &m_hEventLock);
}

CDNSCache* CDNSCache::Instance()
{
	if ( m_dnsCache == NULL)
	{
		m_dnsCache = new CDNSCache;		
	}

	return m_dnsCache;
}

void CDNSCache::FreeInstance()
{
	SafeDelete(m_dnsCache);
}

void CDNSCache::Init()
{
	std::vector<std::string> vPreHost;
	vPreHost.push_back("vodpub.nfile.cn");
	vPreHost.push_back("vodpubws.nfile.cn");
	vPreHost.push_back("vodpubyf.nfile.cn");

	{
		CAutoLock Lock(m_runLock);		
		for (int i=0; i<vPreHost.size(); i++)
		{
			m_runCache.insert(std::make_pair(vPreHost[i],0));
		}
	}
}

CDNSCache::DNS_CACHE_ERROR CDNSCache::Inquire(const std::string& url, bool force)
{
	if (! force)	// 强制重新解析
	{
		CAutoLock lock(m_resultLock);
		if (m_retAddr.find(url) != m_retAddr.end())
		{
			//已经解析出来了，不需要重复解析
			return SUCCESS;
		}
	}

	{
		CAutoLock Lock(m_runLock);		
		std::map<std::string,unsigned long>::iterator itr = m_runCache.find(url);
		if ( itr == m_runCache.end() )
		{
			m_runCache.insert(std::make_pair(url,0));
			//QvodSetThreadEvent(&m_hEvent, &m_hEventLock);
		}
	}
	return UNSET;
}


CDNSCache::DNS_CACHE_ERROR CDNSCache::Resolve(const std::string& url,unsigned long& ip)
{
	CAutoLock Lock(m_resultLock);
	std::map<std::string, IPAddrInfo>::iterator itr = m_retAddr.find(url);
	if ( itr != m_retAddr.end() )
	{
		const IPAddrInfo addr = (*itr).second;
		if (! addr.isIpv6)	// 非ipv6赋值给ip
		{
			ip = addr.ipv4;
		}
		if(!addr.isIpv6 && 0 == addr.ipv4)
		{
			//失败的将记录删除，下次再尝试
			m_retAddr.erase(itr);
			return SUCCESS;
		}
		else
		{
			return SUCCESS;
		}
	}	

	return UNSET;
}

CDNSCache::DNS_CACHE_ERROR CDNSCache::Resolve(const std::string& url, IPAddrInfo& addr)
{
	CAutoLock Lock(m_resultLock);
	std::map<std::string, IPAddrInfo>::iterator itr = m_retAddr.find(url);
	if ( itr != m_retAddr.end() )
	{
		addr = (*itr).second;
		if (! addr.isIpv6 && 0 == addr.ipv4)
		{
			//失败的将记录删除，下次再尝试
			m_retAddr.erase(itr);
		}
		return SUCCESS;	
	}	

	return UNSET;
}

bool ParseIpByHost(const char* hostname, IPAddrInfo& retIp)
{   //  want info
	struct addrinfo *result, *res;

	struct addrinfo addrWant;
    memset(&addrWant, 0x0, sizeof(addrWant));
    addrWant.ai_family = PF_UNSPEC;
    addrWant.ai_socktype = SOCK_STREAM;
    addrWant.ai_protocol = IPPROTO_TCP;
#ifdef MACOSX
    addrWant.ai_flags = AI_DEFAULT;         //
#endif


#ifdef QVODDEBUG
	QVOD_DWORD dwTick = QvodGetTime();
	_OUTPUT(INFO, "start getaddrinfo: %s", hostname);
#endif
	int err = getaddrinfo(hostname,NULL /*"http"*/, &addrWant, &result);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "end getaddrinfo: %s tick=%d", hostname, QvodGetTime()-dwTick); 
#endif

	if (err != 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "getaddrinfo fail! err=%s", gai_strerror(err));
#endif
		return false;
	}
	for (res = result; res!= NULL; res=res->ai_next)
	{
		char host[1025] = "";
//#ifdef QVODDEBUG
//	_OUTPUT(INFO, "start getnameinfo: %s", hostname); 
//	QVOD_DWORD tick = QvodGetTime();
//#endif
//		err = getnameinfo(res->ai_addr, res->ai_addrlen, host, 1025, NULL, 0, 0);
//#ifdef QVODDEBUG
//	_OUTPUT(INFO, "end getnameinfo: %s ip=%s tick=%d", hostname, host, QvodGetTime()-tick); 
//#endif
//		if (err != 0)
//		{
//#ifdef QVODDEBUG
//			_OUTPUT(INFO, "getnameinfo fail! err=%d", err);
//#endif
//			continue;
//		}
       bool bNext = false;
		switch (res->ai_addr->sa_family)
		{
		case AF_INET:
			{
				struct sockaddr_in* sock_addripv4 = reinterpret_cast<struct sockaddr_in*>(res->ai_addr);
				if (NULL == inet_ntop(AF_INET, (struct sockaddr_in*)&sock_addripv4->sin_addr, host, sizeof(host)))
				{
					bNext = true;
					break;
				}
				retIp.strIP = host;
				retIp.ipv4 = inet_addr(host);
				retIp.isIpv6 = false;
			}
			break;
		case AF_INET6:
			{
				struct sockaddr_in6* sockaddr_ipv6 = reinterpret_cast<struct sockaddr_in6*>(res->ai_addr);
				if (NULL == inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, host, sizeof(host)))
				{
					bNext = true;
					break;
				}
				retIp.strIP = host;
				retIp.ipv6 = sockaddr_ipv6->sin6_addr;
				retIp.isIpv6 = true;
			}
			break;
		default:
			break;
		}
		if (bNext)
		{
			continue;
		}
        if (AF_INET == res->ai_addr->sa_family || AF_INET6 == res->ai_addr->sa_family)
        {
            break;
        }
	}
	freeaddrinfo(result);
	return true;
}

QVOD_THREAD_RETURN QVOD_WINAPI CDNSCache::ThreadProc(QVOD_LPVOID lpParam)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "thread CDNSCache::ThreadProc start\n");
#endif
	CDNSCache* pCache = (CDNSCache*)lpParam;
	while(g_bRunning)
	{
		pCache->m_runLock.Lock();
		std::map<std::string,unsigned long>::iterator itr = pCache->m_runCache.begin();
        if(itr != pCache->m_runCache.end())
        {
            std::string strname = itr->first;
            pCache->m_runCache.erase(itr);
            pCache->m_runLock.Unlock();
#ifdef QVODDEBUG
			QVOD_DWORD dwTick = QvodGetTime();
			_OUTPUT(INFO, "start parse %s", strname.c_str());
#endif

			// 解析ipdata
            IPAddrInfo addr;
			if (! ParseIpByHost(strname.c_str(), addr))
			{
				pCache->Inquire(strname, true);
				//continue;
			}

			if (! addr.isIpv6 && 0 == addr.ipv4)	 // 没有就默认获取ipv4
			{
				hostent *pHost = gethostbyname(strname.c_str());
				if ( pHost != NULL )
				{
					memcpy(&addr.ipv4,pHost->h_addr_list[0],sizeof(unsigned long));
					//memcpy(&iIP,pHost->h_addr_list[0],sizeof(long));
	#ifdef QVODDEBUG
					char sip[16] = {0};
					IpInt2Str(addr.ipv4,sip);
					_OUTPUT(INFO, "DNScache gethostbyname ip %s, name %s tick=%d\n", sip, strname.c_str(), QvodGetTime()-dwTick);	
	#endif
				}				
			}
		
#ifdef QVODDEBUG
			_OUTPUT(INFO, "DNScache get ip %s, ipv4=%lu, name %s tick=%d\n", addr.strIP.c_str(), addr.ipv4, strname.c_str(), QvodGetTime()-dwTick);
#endif

			{
				CAutoLock Lock(pCache->m_resultLock);
				std::map<std::string,IPAddrInfo>::iterator itrRes = pCache->m_retAddr.find(strname);
				if ( itrRes != pCache->m_retAddr.end() )
				{
#ifdef QVODDEBUG
					IPAddrInfo ai = itrRes->second;
					if (! (ai == addr))
					{
						_OUTPUT(INFO, "dns parse diff ip! name %s preip:%s nowip:%s", strname.c_str(), ai.strIP.c_str(), addr.strIP.c_str());
					}
#endif
					pCache->m_retAddr.erase(itrRes);
				}
				pCache->m_retAddr.insert(std::make_pair(strname, addr));

			}
        }
        else
        {
            pCache->m_runLock.Unlock();
            QvodSleep(1);
        }
	}
#ifdef QVODDEBUG
	_OUTPUT(ERROR1, "thread CDNSCache::ThreadProc stop\n");
#endif
	return QVOD_THREAD_RETURN_NULL;
}

void CDNSCache::Remove(const string& host)
{
	CAutoLock Lock(m_resultLock);

	std::map<std::string, IPAddrInfo>::iterator itr2 = m_retAddr.find(host);
	if (itr2 != m_retAddr.end())
	{
		m_retAddr.erase(itr2);
	}
}

void CDNSCache::ReParseDnsAll()
{
	std::vector<string> vHosts;
	{
		CAutoLock Lock(m_resultLock);
		for (std::map<std::string, IPAddrInfo>::iterator itr = m_retAddr.begin(); itr != m_retAddr.end(); itr++)
		{
			vHosts.push_back(itr->first);	
		}
		//m_retAddr.clear();	// 先不用删除已解析的dns,后面重新解析到会直接替换之前的
	}
	
	//retry parse dns
	for (int i=0; i<vHosts.size(); i++)
	{
		Inquire(vHosts[i], true);
	}
}

IPAddrInfo GetIpByHost(const char* name, bool bDelExist)
{
	IPAddrInfo addr;
	if (NULL == name)
	{
		return addr;
	}
	string strName = name;
	string strHostName = name;
	int nPos = strName.find(":");
	if (nPos != string::npos)
	{
		strHostName = strName.substr(0, nPos);
	}

	if (bDelExist)
	{
		CDNSCache::Instance()->Remove(strHostName);
	}

	CDNSCache::Instance()->Inquire(strName);
	for (int i = 0; i < 500; i++)
	{
		if(CDNSCache::Instance()->Resolve(strName, addr) != CDNSCache::SUCCESS)
		{
			QvodSleep(10);
		}
		else
		{
			return addr;
		}
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "DNS parse %s failed!\n", strName.c_str());
#endif
	return addr;
}
