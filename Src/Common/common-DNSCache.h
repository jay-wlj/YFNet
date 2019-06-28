// DNSCache.h: interface for the CDNSCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DNSCACHE_H__252D21CC_58D3_42DC_85BD_0AB687ECE2D5__INCLUDED_)
#define AFX_DNSCACHE_H__252D21CC_58D3_42DC_85BD_0AB687ECE2D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32 /* WIN32 */
#pragma warning(disable:4786)
#else /* posix */
//nothing
#endif /* posix end */

#include <map>
#include <string>
#include "qvod_core.h"




struct IPAddrInfo{
	IPAddrInfo()
	{
		isIpv6 =false;
		ipv4 = 0;
	}
	bool operator==(const IPAddrInfo& info)
	{
		return (isIpv6==info.isIpv6 && ipv4 == info.ipv4 && strIP == info.strIP);
	}
	bool isIpv6;
	unsigned long ipv4;
	in6_addr ipv6;
	string strIP;
};

IPAddrInfo GetIpByHost(const char* name, bool bDelExist = false);

class CDNSCache
{
public:
	enum DNS_CACHE_ERROR
	{
		SUCCESS,//成功
		UNSET,  //未决
		FAIL    //失败
	};
public:
	//单例模型
	static CDNSCache* Instance();
	static void FreeInstance();

	void Init();

	//分析Url地址
	DNS_CACHE_ERROR Resolve(const std::string& strUrl,IPAddrInfo& addr);
	DNS_CACHE_ERROR Resolve(const std::string& strUrl,unsigned long& iIP);
	DNS_CACHE_ERROR Inquire(const std::string& url, bool force=false);
	
	
	void Remove(const string& host);
	void ReParseDnsAll();		// 重新解析所有的域名dns
public:
	CDNSCache();
	virtual ~CDNSCache();
private:
	static QVOD_THREAD_RETURN QVOD_WINAPI ThreadProc(QVOD_LPVOID lpParam);

private:
	static CDNSCache*			m_dnsCache;
	std::map<std::string, unsigned long>  m_runCache;

	CLock                       m_runLock;
	CLock                       m_resultLock;
	qvod_thread_event_t         m_hEvent;
	pthread_mutex_t				m_hEventLock;

	std::map<std::string, IPAddrInfo>	m_retAddr;
};

#endif // !defined(AFX_DNSCACHE_H__252D21CC_58D3_42DC_85BD_0AB687ECE2D5__INCLUDED_)

