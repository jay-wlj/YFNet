#include "common-commonfun.h"
#include "HttpAgent.h"
#include "Include.h"
#include "common-utility.h"
#include "AgentInfo.h"
#include "common-AutoPtr.h"
#include "sha1.h"
#include "stdafx.h" /* silver add */
#include "tinyxml.h"
#include "common-SpeedCounter.h"
#include "MsgPoolInterface.h"
#include "TaskMgrInterface.h"
#include "AutoPtr.h"
#include "zlib.h"
#include "cfghost.h"
#include "NetUrl.h"
#include "common-DNSCache.h"

static const int CHUNK = (256*1024);		// gzip解压缓存数据大小

static char get_fmt_of_STANDARD[] = 
"GET %s HTTP/1.1\r\n"
"Accept: */*\r\n"
"Accept-Language: zh-cn\r\n"
#ifdef WIN32
"User-Agent: Yunfan Pc %s\r\n"
"Referer: http://win.lapianapp.com\r\n"
#elif MACOSX
"User-Agent: Yunfan Ios %s\r\n"
"Referer: http://ios.lapianapp.com\r\n"
#else
"User-Agent: Yunfan Android %s\r\n"
"Referer: http://android.lapianapp.com\r\n"
#endif
"Host: %s\r\n"
"Connection: Keep-Alive\r\n\r\n";

static char get_fmt_of_STANDARD2[] = 
"GET %s HTTP/1.1\r\n"
"Accept: */*\r\n"
"Accept-Language: zh-cn\r\n"
#ifdef WIN32
"User-Agent: Yunfan Pc %s\r\n"
"Referer: http://win.lapianapp.com\r\n"
#elif MACOSX
"User-Agent: Yunfan Ios %s\r\n"
"Referer: http://ios.lapianapp.com\r\n"
#else
"User-Agent: Yunfan Android %s\r\n"
"Referer: http://android.lapianapp.com\r\n"
#endif
"Host: %s\r\n"
"%s"
"Connection: Keep-Alive\r\n\r\n";

static char get_fmt_of_RANGE[] =
"GET %s HTTP/1.1\r\n"
"Accept: */*\r\n"
"Accept-Language: zh-cn\r\n"
#ifdef WIN32
"User-Agent: Yunfan Pc %s\r\n"
"Referer: http://win.lapianapp.com\r\n"
#elif MACOSX
"User-Agent: Yunfan Ios %s\r\n"
"Referer: http://ios.lapianapp.com\r\n"
#else
"User-Agent: Yunfan Android %s\r\n"
"Referer: http://android.lapianapp.com\r\n"
#endif
"Host: %s\r\n"
"Range: bytes=%llu-%llu\r\n"
"Connection: Keep-Alive\r\n\r\n";

static char get_fmt_of_START[] =
"GET %s%s%llu HTTP/1.1\r\n"
"Accept: */*\r\n"
#ifdef WIN32
"User-Agent: Yunfan Pc %s\r\n"
#elif MACOSX
"User-Agent: Yunfan Ios %s\r\n"
#else
"User-Agent: Yunfan Android %s\r\n"
#endif
"Host: %s\r\n"
"Referer: %s\r\n"
"Connection: Keep-Alive\r\n\r\n";

static char get_fmt_of_START2[] =
"GET %s HTTP/1.1\r\n"
"Accept: */*\r\n"
#ifdef WIN32
"User-Agent: Yunfan Pc %s\r\n"
#elif MACOSX
"User-Agent: Yunfan Ios %s\r\n"
#else
"User-Agent: Yunfan Android %s\r\n"
#endif
"Host: %s\r\n"
"Referer: %s\r\n"
"Connection: Keep-Alive\r\n\r\n";


static const int c_RECVBUF_SIZE = 65*1024;			// 一次最多接收65K数据

CHttpAgent::CHttpAgent(void)
{
	m_status = STOP;
	m_nSocket = 0;
	m_timestamp = QvodGetTime();
	m_iGlobalDownSpeed = 0;
#ifdef WIN32 /* WIN32 */
	//nothing
#else /* posix */
	/* silver add, init m_IfSockConnected */
	for(int i = 0; i < 64; ++i) {
		m_IfSockConnected[i] = 0;
		pfds[i].events = 0;
		pfds[i].revents = 0;
		pfds[i].fd = QVOD_INVALID_SOCKET;
	}
#endif /* posix end */
	for (int i=0; i<sizeof(m_SocketArray)/sizeof(m_SocketArray[0]); i++)
	{
		m_SocketArray[i] = NULL;
	}
#ifdef SUPPORT_HTTPS
	CHttpSSLSock::InitSSLContext();		// SSL初始化
#endif
}

CHttpAgent::~CHttpAgent(void)
{
    {
		CAutoLock lock(m_nSockLock);
		for (int i=0; i<m_nSocket; i++)
		{
			SafeRelease(m_SocketArray[i]);
			//SafeDelete(m_SocketArray[i]);
		}
    }
#ifdef SUPPORT_HTTPS
	CHttpSSLSock::UnInitSSLContext();
#endif
}

#ifdef WIN32
WSAEVENT* CHttpAgent::GetEventArray(int index/* = 0*/)
{
	return &m_EventArray[index];
}
#else /* posix */
//nothing
#endif /* posix end */

qvod_sock_t CHttpAgent::GetSockArray(int index)
{
	qvod_sock_t ret = QVOD_INVALID_SOCKET;
    CAutoLock lock(m_nSockLock);
    if (m_SocketArray[index])
    {
        ret = m_SocketArray[index]->GetSocket();
    }
//#ifdef 
//	_OUTPUT(INFO, "m_SocketArray[index]=%0x sock=%d", m_SocketArray[index], ret);
//#endif
    return ret;
}

HASH CHttpAgent::GetSockBindHash(int index)
{
	HASH hash;
    CAutoLock lock(m_nSockLock);
    if (m_SocketArray[index])
    {
		PHttpAgentSockData pSockData  = dynamic_cast<PHttpAgentSockData>(m_SocketArray[index]->GetData());
		if (pSockData)
		{
			hash = pSockData->hash;
		}
    }
	return hash;
}

void CHttpAgent::Run()
{
	if(m_status == RUNNING)
		return;
	m_status = RUNNING;
	QvodCreateThread(&m_handle, (qvod_routine_pt)Routine, this); 
}

void CHttpAgent::Stop()
{
	if(m_status == STOP)
		return;
	m_status = STOP;

	QvodWaitForThread(m_handle, NULL); 
}

void CHttpAgent::SetSavePath(char* url,char* savepath,QVOD_UINT64 ipos)
{
	//锟斤拷锟斤拷目录
	char tempPath[1024]={0};
	char* pbuf = tempPath;
	int pathlen = QvodGetTempPath(1024,pbuf);
	pbuf += pathlen;

	strcpy(tempPath+pathlen,"yfacc/");
	pbuf += strlen("yfacc/");

	QvodMkdir(tempPath);
	
	GetNameFromUrl(url, pbuf);

	//锟斤拷锟斤拷锟斤拷pos位锟斤拷
	if(ipos)
	{
		char add[25]={0}; 
		sprintf(add,"_%lld\0",ipos);
		strcpy(tempPath + strlen(tempPath),add);
	}

	char* preview = GetPreviewPoint(url);
	if (preview)
	{
		strcpy(tempPath + strlen(tempPath), preview);
	}
	strcpy(savepath,tempPath);
}

bool CHttpAgent::DelTask(const HASH& hash)
{
	TerminalSock(hash, true);
	// 同时需要删除hash任务下的子请求数
	DelOneTaskSock(hash);

	CAutoLock lock(m_errorLock);
	std::map<HASH,map<string, string> >::iterator itr = m_mHttpErrors.find(hash);
	if (itr != m_mHttpErrors.end())
	{
		itr->second.clear();
		m_mHttpErrors.erase(itr);
	}
	return false;
}

int CHttpAgent::ReConnect(const HASH& hash)
{
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
	{
		return -1;
	}
	//重置部分数值
	aginfo->downloadlen = 0;
	aginfo->filelen = 0;
	aginfo->contentlen = 0;
	aginfo->sendlen = 0;
	aginfo->senddatalen = 0;
	aginfo->bRemoveInvalidHead = false;
	aginfo->bshare = false;
	aginfo->connected = false;
	aginfo->reuse = false;
	aginfo->downloadtime = QvodGetTime();
	aginfo->iMaxDownloadTime = (aginfo->endpos-aginfo->pos)/16384;
	if (aginfo->iMaxDownloadTime < 5)
	{
		aginfo->iMaxDownloadTime = 5;
	}
//	aginfo->time = 0;

	const char* geturl = NULL;
	if (aginfo->strUrl.length() > 0)
	{
		if ('/' == aginfo->strUrl[0])
		{
			aginfo->strUrl = aginfo->strHost + aginfo->strUrl;
		}
	}

    std::string strHost;
	int nPos = aginfo->strUrl.find("/");
	if (nPos != string::npos)
	{
		strHost = aginfo->strUrl.substr(0, nPos);
		geturl = aginfo->strUrl.c_str() + nPos;
	}

	if (NULL == geturl)
	{
		return -1;
	}
	//char cdnhost[256] = {0};
	//strcpy(cdnhost, host);

	memset(aginfo->rawbuf,0,sizeof(aginfo->rawbuf));
	if (aginfo->bHaveRange)
	{
		if (aginfo->endpos > 0)
		{
			sprintf(aginfo->rawbuf, get_fmt_of_RANGE, geturl, LIB_VERSION, strHost.c_str(), aginfo->pos,aginfo->endpos);
		}
		else
		{
			string strHeader;
			char szHeader[50] = {0};
			sprintf(szHeader, "Range: bytes=%llu-\r\n", aginfo->pos);
			sprintf(aginfo->rawbuf, get_fmt_of_STANDARD2, geturl,LIB_VERSION,strHost.c_str(), szHeader);
		}
	}
	else
	{
		if (aginfo->type == CAgentInfo::_START)
		{
			if (strstr(geturl, "start="))
			{
				sprintf(aginfo->rawbuf, get_fmt_of_START2, geturl, LIB_VERSION, strHost.c_str(), aginfo->referer);
			} 
			else
			{
				if (strstr(geturl,"?"))
				{
					sprintf(aginfo->rawbuf, get_fmt_of_START, geturl,"&start=", aginfo->pos, LIB_VERSION, strHost.c_str(), aginfo->referer);
				}
				else
				{
					sprintf(aginfo->rawbuf, get_fmt_of_START, geturl,"?start=", aginfo->pos, LIB_VERSION, strHost.c_str(), aginfo->referer);
				}
			}
		}
		else
		{
			sprintf(aginfo->rawbuf, get_fmt_of_STANDARD, geturl, LIB_VERSION, strHost.c_str());
		}
	}
	aginfo->strHost = strHost;
	string strcdnhost = strHost;
    bool bHttps = aginfo->strOrgUrl.find("https://") != string::npos;
	if (strHost.length() > 0)
	{
        aginfo->port = bHttps? 443: 80;
		int nPos = strHost.find(":");
		if (nPos != string::npos)
		{
			aginfo->port = atoi(strHost.c_str()+nPos+1);
			strcdnhost = strHost.substr(0, nPos);
		}

		//aginfo->ip = GetHostByName(strCdnHost.c_str());
		aginfo->port = htons(aginfo->port);
	}
	
	const IPAddrInfo ipaddr = GetIpByHost(strcdnhost.c_str());
	qvod_sock_t s = CreateSocket(ipaddr.isIpv6);
	//qvod_sock_t s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    CHttpSock *pSock = NULL;
    if (bHttps)
    {
#ifdef SUPPORT_HTTPS
		pSock = new CHttpSSLSock(s);
#else
		_OUTPUT(INFO, "not SUPPORT_HTTPS return!");
		return -1;
#endif
    }
    else
    {
        pSock = new CHttpSock(s);
    }

#ifdef QVODDEBUG
		char strnip[16] = {0};
		IpInt2Str(aginfo->ip, strnip);
		_OUTPUT(INFO, "CHttpAgent::ReConnect %s new ip %s, port %d\n", aginfo->strHost.c_str(), ipaddr.strIP.c_str(), ntohs(aginfo->port));
#endif // QVODDEBUG

	PHttpAgentSockData pData = new HttpAgentSockData();
	pData->hash = hash;
	pSock->SetData(pData);

#ifdef WIN32
	WSAEVENT event = WSACreateEvent(); 
	if (WSA_INVALID_EVENT == event )
	{
		SafeDelete(pSock);
		return -1;
	}

	int iRcvBufLen = 128*1024;
	setsockopt(s,SOL_SOCKET,SO_RCVBUF,(const char*)&iRcvBufLen,sizeof(int));
	WSAEventSelect(s,event,FD_READ|FD_CONNECT|FD_CLOSE);

	//SOCKADDR_IN SvrAddr;
	//SvrAddr.sin_addr.S_un.S_addr = aginfo->ip;
	//SvrAddr.sin_port = aginfo->port;
	//SvrAddr.sin_family = AF_INET;
	//int r = pSock->Connect(SvrAddr);
	int r =pSock->Connect(ipaddr.ipv4, ipaddr.ipv6, aginfo->port, ipaddr.isIpv6);
	//int r = connect(s,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(r == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		SafeDelete(pSock);
		return -1;
	}

	{
		CAutoLock lock(m_nSockLock);					// 加锁访问代码段
		//SafeDelete(m_SocketArray[m_nSocket]);
		SafeRelease(m_SocketArray[m_nSocket]);
		m_SocketArray[m_nSocket] = pSock;
		m_EventArray[m_nSocket] = event;
		m_hashArr[m_nSocket]=hash;	
		m_nSocket++;
	}


#else
	int iRcvBufLen = 128*1024;
	setsockopt(s,SOL_SOCKET,SO_RCVBUF,(const char*)&iRcvBufLen,sizeof(int));
	QvodSetNonblocking(s);

	//qvod_sockaddr_t SvrAddr;
	//SvrAddr.sin_addr.s_addr = aginfo->ip;
	//SvrAddr.sin_port = aginfo->port;
	//SvrAddr.sin_family = AF_INET;
	//int r = pSock->Connect(SvrAddr);
	int r =pSock->Connect(ipaddr.ipv4, ipaddr.ipv6, aginfo->port, ipaddr.isIpv6);
	//int r = connect(s,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(r == QVOD_SOCKET_ERROR && QvodNetGetLastError() != EINPROGRESS && QvodNetGetLastError() != EINPROGRESS)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CHttpAgent::ReConnect connect [%s]:%d error = %d\n",ipaddr.strIP.c_str(),ntohs(aginfo->port),QvodNetGetLastError());
#endif
		SafeDelete(pSock);
		return -1;
	}

	{
		CAutoLock lock(m_nSockLock);					// 加锁访问代码段
		//SafeDelete(m_SocketArray[m_nSocket])
		SafeRelease(m_SocketArray[m_nSocket]);
		m_SocketArray[m_nSocket] = pSock;
		m_hashArr[m_nSocket]=hash;
		pfds[m_nSocket].fd = s;
		pfds[m_nSocket].events |= (POLLOUT|POLLERR|POLLHUP|POLLNVAL);
		m_nSocket++;
	}
#endif

	aginfo->last_recv_tick	= QvodGetTime();	// record the beginning tick
	aginfo->downloadtime	= QvodGetTime();
	aginfo->sock			= s;

#ifdef QVODDEBUG
	std::string msg0=Hash2Char((unsigned char*)hash.data);
	_OUTPUT(INFO,"CHttpAgent::ReConnect connectimes=%d [%s],[%s] sock=%d\n",aginfo->connectimes, msg0.c_str(),aginfo->strOrgUrl.c_str(), s);
#endif // QVODDEBUG


	return 0;
}

int CHttpAgent::ReadPiece(const HASH& hash, char *buf, QVOD_UINT64 len, QVOD_UINT64 pos)
{
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
	{
		return -1;
	}

	//发生错误停止
	if(aginfo->status == CAgentInfo::_STOP)
	{
		return -1;
	}
    
    switch (aginfo->type)
    {
        case CAgentInfo::_RANGE:
        case CAgentInfo::_START:
            return Read(hash,buf,len,pos);
        case CAgentInfo::_M3U8TS:
            return ReadTs(hash, buf, len, pos);
            break;
    }

	return -1;
}

int CHttpAgent::OnData(const int i)
{
	HASH idxHash = m_hashArr[i];
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(idxHash,&aginfo))
	{
		HASH sockHash = GetSockBindHash(i);			// 找到sock绑定的hash
		if(sockHash == idxHash || !GetAgentInfo(idxHash,&aginfo))
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpAgent::OnData GetAgentInfo false  i=%d sockhash=%s\n", i, Hash2Char(sockHash.data).c_str());
#endif			
			return -1;
		}
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::OnData GetSockBindHash  i=%d sockhash=%s hash=%s\n", i, Hash2Char(sockHash.data).c_str(), Hash2Char(idxHash.data).c_str());
#endif		
		idxHash = sockHash;
	}
	const HASH hash = idxHash;		// 保存当前的hash

	//读取数据
	char recvBuf[c_RECVBUF_SIZE] = {0};
	int buflen = c_RECVBUF_SIZE;
	char* pbuf = recvBuf;
	int totallen = 0;
	int datalen  = 0;
	int err = 0;
	int nlen;

	{
		CAutoLock lock(m_nSockLock);		// 加锁读取
		bool  bSame = HasHttpSock(i, aginfo->sock);
		if (! bSame)	// 绑定的sock不一样
		{
#ifdef QVODDEBUG
			if (m_SocketArray[i])
			{
				PHttpAgentSockData pData = dynamic_cast<PHttpAgentSockData>(m_SocketArray[i]->GetData());
				HASH sockHash;
				if (pData)
				{
					sockHash = pData->hash;
				}
				_OUTPUT(ERROR1, "CHttpAgent::OnData index=%d hash=%s   sockhash=%s", i, Hash2Char(hash.data).c_str(), Hash2Char(sockHash.data).c_str());
			}
#endif
			return -1;
		}
		while (buflen > 0 )
		{
			nlen = m_SocketArray[i]->RecvData(pbuf,QVOD_MIN2(buflen, c_RECVBUF_SIZE));
			//nlen = recv(m_SocketArray[i],pbuf,QVOD_MIN2(buflen, c_RECVBUF_SIZE),0);
			//接受断开
			if(nlen == 0)
			{
	#ifdef QVODDEBUG
				_OUTPUT(ERROR1, "CHttpAgent::OnData recv error %d, connection closed\n", errno);
	#endif
				break;
			}
			//接受异常
			if(nlen == -1)
			{
				err = QvodNetGetLastError();

				if(QVOD_NET_EWOULDBLOCK == err || EAGAIN == err || QVOD_DISK_FULL == err)		// 等待下一次读
				{
					break;
				}
				else
				{
#ifdef QVODDEBUG
				_OUTPUT(ERROR1, "CHttpAgent::OnData recv err:%d errno:%d datalen=%d\n", err, errno, datalen);
#endif
					aginfo->errcode = 3;
					aginfo->errcode |= (err << 16);
					return -1;
				}
			}
			pbuf += nlen;
			buflen -= nlen;
			datalen += nlen;
			totallen += nlen;
			if (aginfo->filelen <= aginfo->downloadlen + datalen)
			{
				break;
			}
		}
	}
	pbuf = recvBuf;
	//处理读取到的数据
	if(datalen == 0)
	{
		if(QVOD_NET_EWOULDBLOCK == err || EAGAIN == err || QVOD_DISK_FULL == err)		// 等待下一次读
		{
			return 0;
		}
		aginfo->errcode = 4;
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpAgent::OnData recv err:%d errno:%d\n", err, errno);
#endif
		return -1;
	}
	//分析处理第一个数据包
	if(aginfo->filelen==0)
	{
		aginfo->strDataBuf.clear();		// 清掉原始数据
#ifdef QVODDEBUG
		char *head = strstr(recvBuf, "\r\n\r\n");
		if (head)
		{
			string strHead(recvBuf, head-recvBuf);
			string strHash = Hash2Char(hash.data);
			_OUTPUT(INFO, "CHttpAgent::OnData begin, [%s] datalen=%d recv:\n%s \n\n", strHash.c_str(), datalen, strHead.c_str());
		}
#endif
		//异常数据,直接返回，丢弃数据
		if(!strstr(recvBuf,"HTTP/1."))
		{
			aginfo->errcode = 5;

#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::OnData strstr(recvBuf,\"%s\"", recvBuf);
#endif
			return -1;
		}
		char* strend = strstr(recvBuf,"\r\n");
		if(!strend) 
		{
			aginfo->errcode = 6;

#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::OnData strstr(recvBuf = NULL\n");
#endif
			return -1;
		}
		//*strend++ = 0;
		pbuf = strend+2;
		//qvod_strupr(recvBuf);
		
		if(!strstr(recvBuf,"200 ") && !strstr(recvBuf,"206 "))
		{	
			if(strstr(recvBuf," 302 ") || strstr(recvBuf," 301 ") || strstr(recvBuf," 303 "))
			{
				char* strredirect = strstr(strend,"Location: ");
				if(strredirect)
				{
					strredirect += strlen("Location: ");
					char* pstrend = strstr(strredirect,"\r\n");
					if(pstrend) 
					{
						*pstrend = 0; 
						bool bHttps = (NULL != strstr(strredirect, "https://"));
						bool bOrgHttps = aginfo->strOrgUrl.find("https://") != string::npos;
						if (bHttps != bOrgHttps)		//  重定向后https http有变化，则替换strOrgUrl
						{
#ifdef QVODDEBUG
							_OUTPUT(INFO,"CHttpAgent recv 302 : old orgurl=%s new orgurl=%s\n", aginfo->strOrgUrl.c_str(), strredirect);
#endif // QVODDEBUG
							aginfo->strOrgUrl = strredirect;
						}
#ifdef QVODDEBUG
						_OUTPUT(INFO,"CHttpAgent recv 302 : %s\n", strredirect);
#endif // QVODDEBUG
						char* pos = strstr(strredirect, "http://");
						if (pos)
						{
							strredirect = pos + 7;
						}	
						else
						{
							pos = strstr(strredirect, "https://");
							if (pos)
							{
								strredirect = pos + 8;
							}
						}

						//若重定向是内网地址，说明是被路由器截获了，这种也算网络不可达
						unsigned int ip = inet_addr(strredirect);
						if (ip != INADDR_NONE && ip != 0 && IsLANIP(ip))
						{
							aginfo->errcode = 1;
							aginfo->strErrBuf = "IsLANIP";
							return -1;
						}
						aginfo->strUrl = strredirect;

						//char* pServer = strstr(strend, "Server: ");
						//if (pServer)
						//{
						//	pServer += strlen("Server: ");
						//	char* pend = strstr(pServer, "\r\n");
						//	if (pend)
						//	{
						//		aginfo->strErrBuf.append(pServer, pend-pServer);
						//	}
						//}
#ifdef QVODDEBUG
						_OUTPUT(ERROR1, "CHttpAgent::OnData 302 Location=%s\n", strredirect);
#endif
						return -1;
					}
				}
			}
			else	if(strstr(recvBuf,"40") || strstr(recvBuf,"41") || strstr(recvBuf,"50"))
			{
				const char* pBuf = strstr(recvBuf, " ");
				if (pBuf)
				{
					aginfo->httpcode = atoi(pBuf+1);
				}
				//40X或50X的错误，链接失效了
				aginfo->errcode = 7;
			}
			//返回失败数据
			aginfo->strErrBuf.assign(recvBuf, datalen);
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::OnData !=200 recvbuf:%s\n", recvBuf);
#endif
			return -1;
		}

		//若访问成功
		//种子长度
		pbuf = strstr(strend,"Meta-Length: ");
		if(pbuf)
		{
			pbuf += strlen("Meta-Length: ");
			aginfo->metalen = atoi(pbuf);
		}
		//文件长度
		pbuf = strstr(strend,"Content-Length: ");
		if(!pbuf)
		{
			pbuf = strstr(strend,"Content-length: ");
			if(!pbuf)
			{
				aginfo->filelen = 0x7FFFFFFF;
				pbuf = strend;
			}
			else
			{
				pbuf += strlen("Content-Length: ");
				aginfo->filelen = QVOD_ATOI64(pbuf);
				aginfo->contentlen = QVOD_ATOI64(pbuf);
			}
		}
		else
		{
			pbuf += strlen("Content-Length: ");
			aginfo->filelen = QVOD_ATOI64(pbuf);
			aginfo->contentlen = QVOD_ATOI64(pbuf);
		}
		pbuf = strstr(strend,"X-Content-Length: ");//for 6.cn 
		if(pbuf)
		{
			pbuf += strlen("X-Content-Length: ");
			int xContentLen = atoi(pbuf);
			if (aginfo->filelen > xContentLen)
			{
				aginfo->filelen = xContentLen;
			}
		}
		aginfo->wholeFilelen = aginfo->contentlen + aginfo->pos;
		if (aginfo->type == CAgentInfo::_START)	// for _START task, the Content-Length field is special
		{
			if (aginfo->endpos - aginfo->pos + 1 < aginfo->filelen)
			{
				//contentlen大于实际需要的长度
				aginfo->filelen = aginfo->endpos - aginfo->pos + 1;
			}
		}
		pbuf = strstr(strend, "Content-Range: bytes ");
		{
			string contentRange = HttpGetValue(strend, "Content-Range: bytes ");
			size_t ipos = contentRange.find('/');
			if (string::npos != ipos)
			{
				aginfo->wholeFilelen = QVOD_ATOI64(contentRange.substr(ipos+1).c_str());
				QVOD_INT64 start = aginfo->pos;
				QVOD_INT64 end = aginfo->endpos;
				sscanf(contentRange.c_str(),"%lld-%lld/", &start, &end);
				if (aginfo->pos != start || aginfo->endpos != end)
				{
					aginfo->port = start;
					aginfo->endpos = end;
				}
			}
			else
			{
				aginfo->pos = 0;			// 响应头没有部分传输，需重置pos,endpos
				aginfo->endpos = 0;
			}
		}
		string strVal = GetHeadValue(strend, "Content-Encoding");
		aginfo->bGzip = (strVal =="gzip");
		strVal = GetHeadValue(strend, "Transfer-Encoding");
		aginfo->bThunked = strVal=="chunked";

		pbuf = strstr(strend,"\r\n\r\n");
		if(!pbuf)
		{
			aginfo->errcode = 8;
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::OnData pbuf \n");
#endif
			return -1;
		}
		pbuf += 4;
		datalen -= (int)(pbuf - recvBuf);
        if (0 == aginfo->endpos)
        {
            int iMaxDownloadTime = aginfo->wholeFilelen/16384;
            aginfo->iMaxDownloadTime = iMaxDownloadTime < 5?5:iMaxDownloadTime;
			if (CAgentInfo::_M3U8 == aginfo->type)
			{
				aginfo->iMaxDownloadTime *= 2;		// m3u8文件给予双倍时间获取数据
				aginfo->iMaxDownloadTime += (aginfo->last_recv_tick-aginfo->downloadtime);
			}
#ifdef QVODDEBUG
            _OUTPUT(INFO,"CHttpAgent http contentlen = %lld,change maxdowntime %d sec\n",aginfo->wholeFilelen,aginfo->iMaxDownloadTime);
#endif
        }

		if (aginfo->bThunked)
		{
			aginfo->iMaxDownloadTime = -1;			// thunked分块下载，不确定下载总耗时，先设无穷大
		}
	}
	//处理无内容情况
	if(datalen == 0)
	{
		return 0;
	}
	//根据类型处理收到的http数据内容
	switch(aginfo->type)
	{
	case CAgentInfo::_RANGE:
	case CAgentInfo::_START:
	case CAgentInfo::_M3U8:
	case CAgentInfo::_M3U8TS:
		{
			if (! aginfo->bThunked)
			{
				//创建缓冲区
				CAutoLock lock(m_fileLock);
				if(aginfo->pdatabuf == NULL)
				{
					aginfo->pdatabuf = new char[aginfo->filelen+1];
					if (!aginfo->pdatabuf)
					{
						return -1;
					}
				}
			}

			bool bDownFinish = false;
			size_t to_copy = datalen;
			{
				//写入数据 ，写锁局限在该代码块内，避免下面的TermiateSock调用产生死锁
				CAutoLock lock(m_fileLock);

				if (aginfo->bThunked)		// thunked分块下载
				{
					aginfo->strDataBuf.append(pbuf, datalen);
					aginfo->downloadlen = aginfo->strDataBuf.length();

					QVOD_UINT64 uDataLen = 0;
					if (IsChunkEnd(aginfo->strDataBuf, aginfo->strDataBuf.length(), uDataLen))		// 分块下载结束
					{
	#ifdef QVODDEBUG
						_OUTPUT(INFO, "CHttpAgent::OnData IsChunkEnd recvsize=%d", aginfo->strDataBuf.length());
	#endif
						aginfo->pdatabuf = new char[uDataLen+1];
						QVOD_UINT64 useBufLen = 0;
						QVOD_UINT64 handedCL = 0;
						if (NULL == GetChunkData((char* const)aginfo->strDataBuf.c_str(), aginfo->strDataBuf.length(), aginfo->pdatabuf, uDataLen, useBufLen, handedCL))
						{
							QVOD_UINT64 orgLen = aginfo->strDataBuf.length();

							aginfo->strDataBuf.clear();		// 清空原始原始数据释放内存

							if (0 == useBufLen)					// 数据错误
							{
								return -1;
							}

							aginfo->downloadlen = useBufLen;
							aginfo->filelen = useBufLen;
							bDownFinish = ! aginfo->bGzip;

							// thunked数据接收完成
							if (aginfo->bGzip)		// 解压数据
							{
	#ifdef QVODDEBUG
								QVOD_DWORD dwTick = QvodGetTime();
	#endif
								string strRet;
								if (! UnGzip(aginfo->pdatabuf, aginfo->downloadlen, strRet))	// 解压数据失败
								{
	#ifdef QVODDEBUG
									_OUTPUT(INFO, "CHttpAgent::OnData unGzip failed!");
	#endif
									return 1;
								}

								SafeDeleteArray(aginfo->pdatabuf);																					// 删除加密数据								
								aginfo->filelen = aginfo->downloadlen = strRet.length();											// 更正下载数据内容长度
								aginfo->pdatabuf = new char[aginfo->downloadlen+1];												
								if (aginfo->downloadlen>0)
								{
									memcpy((void*)aginfo->pdatabuf, (void*)strRet.c_str(), aginfo->downloadlen);		// 复制解压后的数据
								}

								bDownFinish = true;
	#ifdef QVODDEBUG
								_OUTPUT(INFO, "CHttpAgent::OnData unzip ok! tick=%d zipsize=%d datasize=%lld", QvodGetTime()-dwTick, orgLen, aginfo->downloadlen);
	#endif
								//FILE *fp = fopen("C:/2.txt", "wb");
								//if (fp)
								//{
								//	fwrite(aginfo->pdatabuf, 1, aginfo->downloadlen, fp);
								//	fclose(fp);
								//}
							}
						}
					}
				}
				else				
				{
					to_copy = QVOD_MIN2(aginfo->filelen - aginfo->downloadlen, datalen);	// avoid of overwriting
					if (to_copy > 0)
					{
						memcpy(aginfo->pdatabuf+aginfo->downloadlen,pbuf,to_copy);
						aginfo->downloadlen += to_copy;
					}

					bDownFinish = (aginfo->downloadlen == aginfo->filelen);
					if (CAgentInfo::_START == aginfo->type && aginfo->pos != 0 )
					{
						if (!aginfo->bRemoveInvalidHead && aginfo->downloadlen > 13+11)
						{
							bool bH264 = true;
							//不是从0开始的位置，湖南tv的需要去掉一些不必要的头
							if (0x09 == aginfo->pdatabuf[13] && 0x12 == aginfo->pdatabuf[13+11])
							{
								//h263
								bH264 = false;		
							}
							if (!bH264)
							{
								//h263的，把任务改为纯http的
								CTaskMgrInterFace::Instance()->ChangeTaskType(hash,HTTPDOWNTASK);
							}
							QVOD_UINT64 iFilesize = CTaskMgrInterFace::Instance()->GetFileSize(hash);
							if (iFilesize > 0 && aginfo->contentlen > 0)
							{
									//去掉多余的
								QVOD_UINT64 iInvalidLen = aginfo->contentlen+aginfo->pos-iFilesize;		
								if (aginfo->downloadlen >= iInvalidLen && iInvalidLen > 0)
								{
									aginfo->downloadlen -= iInvalidLen;
									memset(aginfo->pdatabuf, 0, iInvalidLen);	//防止第一次收到只有iInvalidLen字节时memmove没变化
									memmove(aginfo->pdatabuf, aginfo->pdatabuf+iInvalidLen, aginfo->downloadlen);
									//wholefilelen需要减掉该头大小
									aginfo->wholeFilelen -= iInvalidLen;
									aginfo->bRemoveInvalidHead = true;
									if (datalen > to_copy && aginfo->downloadlen < aginfo->filelen)
									{
									//接收到的缓存还有数据，且未下载完
									//芒果tv不是从0开始的位置会加上iInvalidLen字节或更长的flv头，且一直推送到文件尾，所以收到的数据长度可能会超出预期的数据长度
									//导致to_copy包含了前面没用的flv头，减掉flv头后即可把数据补上
										int iRecopyLen = datalen - to_copy;
										iRecopyLen = QVOD_MIN2(aginfo->filelen - aginfo->downloadlen, iRecopyLen);
										memcpy(aginfo->pdatabuf+aginfo->downloadlen,pbuf+to_copy,iRecopyLen);
										aginfo->downloadlen += iRecopyLen;
		#ifdef QVODDEBUG
										_OUTPUT(INFO,"CHttpAgent filelen = %lld,to_copy = %d,datalen = %d,recopylen = %d\n",
											aginfo->filelen,to_copy,datalen,iRecopyLen);
		#endif
									}
		#ifdef QVODDEBUG
									_OUTPUT(INFO,"CHttpAgent Remove flv head %lld,wholefilelen = %lld,downlen = %lld\n",iInvalidLen,aginfo->wholeFilelen,aginfo->downloadlen);
		#endif
								}
							}
						}
					}
				}
			}

			CMsgPoolInterface::Instance()->InterDownSize(to_copy);
			m_iGlobalDownSpeed += to_copy;
			HASH szTaskHash = hash;
			if (!aginfo->strHash.empty())	// 子请求的hash替换成所属任务hash
			{
				memcpy(szTaskHash.data, aginfo->strHash.c_str(), 20);
			}
			CSpeedCounter::Instance()->PushSpeedData(&szTaskHash,NULL,to_copy,GlobalDownSpeedCallBack);
			SPEEDATA data;
			data.hash = szTaskHash;
			data.size = to_copy;
			CTaskMgrInterFace::Instance()->InterDownSize(data);	//把http速度算到任务的总速度里
			aginfo->last_recv_tick = QvodGetTime();

			//下载完成
			if(bDownFinish)
			{
				if (!aginfo->bshare)
				{
					aginfo->bshare = true;
					if (aginfo->type == CAgentInfo::_RANGE || aginfo->type == CAgentInfo::_START)
					{
						agentrange *range = new agentrange;
						range->head.len = sizeof(agentrange)-4;
						range->head.id = AGENTRANGE;
						range->result = 0;

						{
							CAutoLock lock(m_fileLock);		// 加锁访问
							range->start = aginfo->pos;
							range->buf = aginfo->pdatabuf;
							aginfo->pdatabuf = NULL;	// released in msgpool
							range->len = aginfo->downloadlen;
							range->filesize = aginfo->wholeFilelen;
						}
						memcpy(range->hash,hash.data,20);
#ifdef QVODDEBUG
						_OUTPUT(INEED,"CHttpAgent::OnData finish!  [%s] push AGENTRANGE msg, start=%lld,len=%lld,type=%d,tick=%d retrys=%d tick=%d retrys=%d\n",Hash2Char(hash.data).c_str(), aginfo->pos,aginfo->downloadlen,aginfo->type,QvodGetTime()-aginfo->initime, (int)aginfo->connectimes);
#endif
						{
							CAutoLock lock(m_msgQueueLock);
							m_msgQueue.push((agentmsghead*)range);
						}
						if (aginfo->type == CAgentInfo::_RANGE)
						{
							aginfo->reuse = true;
						}
						else if (aginfo->type == CAgentInfo::_START)	// _START task must close socket immediately
						{
							TerminalSock(hash, true, false, false);
						}
					}
					else if (CAgentInfo::_M3U8 == aginfo->type)
					{
                        TerminalSock(hash, true, false, false);
						agentm3u8* m3u8 = new agentm3u8;
						m3u8->head.len = sizeof(agentm3u8)-4;
						m3u8->head.id = AGENTM3U8;
						m3u8->result = 0;
						{
							CAutoLock lock(m_fileLock);		// 加锁访问
							m3u8->buf = aginfo->pdatabuf;
							aginfo->pdatabuf = NULL;	// released in msgpool
							m3u8->len = aginfo->downloadlen;
						}
						memcpy(m3u8->hash,hash.data,20);
						{
							CAutoLock lock(m_msgQueueLock);
							m_msgQueue.push((agentmsghead*)m3u8);
						}
#ifdef QVODDEBUG
						_OUTPUT(INFO,"CHttpAgent::OnData finish!  [%s] push AGENTM3U8 msg, tick=%d retrys=%d\n",Hash2Char(hash.data).c_str(), QvodGetTime()-aginfo->initime, (int)aginfo->connectimes);
#endif
					}
					else if (CAgentInfo::_M3U8TS == aginfo->type)
					{
                        TerminalSock(hash, true, false, false);
						agentm3u8ts* ts = new agentm3u8ts;
						ts->head.len = sizeof(agentm3u8ts)-4;
						ts->head.id = AGENTM3U8TS;
						ts->result = 0;
						{
							CAutoLock lock(m_fileLock);		// 加锁访问
							ts->buf = aginfo->pdatabuf;
							aginfo->pdatabuf = NULL;	// released in msgpool
							ts->len = aginfo->downloadlen;
							ts->filesize = aginfo->wholeFilelen;
							ts->start = aginfo->pos;
						}
						const char* pFilenamePos = strrchr(aginfo->strOrgUrl.c_str(),'/');
						if (pFilenamePos)
						{
							++pFilenamePos;
							ts->filename = pFilenamePos;
						}

						memcpy(ts->hash,hash.data,20);
						if (! aginfo->strHash.empty())	// 子请求的hash替换成所属任务hash
						{
							memcpy(ts->hash,aginfo->strHash.c_str(),20);
							DelOneTaskHash(HASH(aginfo->strHash.c_str()), hash);
						}
#ifdef QVODDEBUG
						_OUTPUT(INFO,"CHttpAgent::OnData finish! [%s] push %s AGENTM3U8TS msg,downlen=%lld,filesize=%lld filepos=%llu tick=%d retrys=%d\n", Hash2Char(hash.data).c_str(), ts->filename.c_str(),ts->len,ts->filesize, aginfo->uFilePos, (QvodGetTime()-aginfo->initime), aginfo->connectimes);
#endif
						CAutoLock lock(m_msgQueueLock);
						m_msgQueue.push((agentmsghead*)ts);
					}
				}
				return 1;
			}
			break;
		}
	default:
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CHttpAgent******find a unknow agent %d , i %d, hash %s,pos %llu*******\n", aginfo->type, i, aginfo->strUrl.c_str(), aginfo->pos);
#endif
		break;
	}
	return (int)(pbuf - recvBuf);
}

int CHttpAgent::OnConnect(int i)
{
	const HASH hash = GetSockBindHash(i);
	int sendlen = 0;
	char host[128] = {0};
	char buf[1024];
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
	{
		return -1;
	}
//#ifdef QVODDEBUG
//	char msg0[41]={0};
//	memcpy(msg0,Hash2Char(m_hashArr[i].data).c_str(),41);
//	_OUTPUT(INFO, "CHttpAgent::OnConnect [%s] type=%d\n", msg0, aginfo->type);
//#endif

	aginfo->status=CAgentInfo::_RUNING;
	aginfo->connected = true;
	const char* url = aginfo->strUrl.c_str();
	char* pbuf = buf;
	int err = 0;
	switch (aginfo->type)
	{
	case CAgentInfo::_RANGE:
	case CAgentInfo::_START:
	case CAgentInfo::_M3U8:
	case CAgentInfo::_M3U8TS:
		{
			aginfo->last_recv_tick = QvodGetTime();
			//char buffer[2048] = {0};
			//strcpy(buffer, "GET ");
			//char* pos = strstr(aginfo->rawbuf, "GET http://");
			//if (pos)
			//{
			//	pos = pos + strlen("GET http://");
			//	pos = strstr(pos, "/");
			//	if (pos)
			//	{
			//		strcpy(buffer+strlen(buffer), pos);
			//	}
			//	else
			//	{
			//		strcpy(buffer, aginfo->rawbuf);
			//	}
			//}
			//else
			//{
			//	strcpy(buffer, aginfo->rawbuf);
			//}
			{
				CAutoLock lock(m_nSockLock);

				bool  bSame = HasHttpSock(i, aginfo->sock);
				if (! bSame)	// 绑定的sock不一样
				{
		#ifdef QVODDEBUG
					PHttpAgentSockData pData = dynamic_cast<PHttpAgentSockData>(m_SocketArray[i]->GetData());
					HASH sockHash;
					if (pData)
					{
						sockHash = pData->hash;
					}
					_OUTPUT(ERROR1, "CHttpAgent::OnConnect index=%d hash=%s   sockhash=%s", i, m_SocketArray[i]->GetSocket(), Hash2Char(hash.data).c_str(), Hash2Char(sockHash.data).c_str());
		#endif
					return -1;
				}

				sendlen = m_SocketArray[i]->SendData(aginfo->rawbuf,(int)strlen(aginfo->rawbuf));
			}
			if (-1 == sendlen)
			{
				err = QvodNetGetLastError();
				if(QVOD_NET_EWOULDBLOCK == err || EAGAIN == err)		// 等待下一次发送
				{
					return 1;	// edit by weilijian, <0 will close socket
				}		
			}
			//sendlen = send(m_SocketArray[i],buffer,(int)strlen(buffer),0);
			//int error = errno;
			aginfo->reuse = false;	// after sending a http request, the socket now is not reuseable
//#ifdef QVODDEBUG
//			char msg0[41]={0};
//			memcpy(msg0,Hash2Char(m_hashArr[i].data).c_str(),41);
//			_OUTPUT(INFO, "CHttpAgent [%s] send(sendlen = %d,error = %d,sock_%d:%d)\n", msg0, sendlen, error, m_SocketArray[i], aginfo->sock);
//#endif
			break;
		}
	case CAgentInfo::_POST:
		{
			aginfo->last_recv_tick = QvodGetTime();
			if (0 == aginfo->sendlen)	// 发送请求头
			{		
				{
					CAutoLock lock(m_nSockLock);
					
					bool  bSame = HasHttpSock(i, aginfo->sock);
					if (! bSame)	// 绑定的sock不一样
					{
			#ifdef QVODDEBUG
						PHttpAgentSockData pData = dynamic_cast<PHttpAgentSockData>(m_SocketArray[i]->GetData());
						HASH sockHash;
						if (pData)
						{
							sockHash = pData->hash;
						}
						_OUTPUT(ERROR1, "CHttpAgent::OnConnect index=%d hash=%s   sockhash=%s", i, m_SocketArray[i]->GetSocket(), Hash2Char(hash.data).c_str(), Hash2Char(sockHash.data).c_str());
			#endif
						return -1;
					}

					sendlen = m_SocketArray[i]->SendData(aginfo->rawbuf,(int)strlen(aginfo->rawbuf));
				}
				if (-1 == sendlen)
				{
					err = QvodNetGetLastError();
					if(QVOD_NET_EWOULDBLOCK == err || EAGAIN == err)		// 等待下一次发送
					{
						return 1;	// edit by weilijian, <0 will close socket
					}		
				}
				//sendlen = send(m_SocketArray[i], aginfo->rawbuf,(int)strlen(aginfo->rawbuf),0);
				aginfo->sendlen += sendlen;

#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpUpload::OnConnect [%s] type=%d sendlen=%d info=%s\n", Hash2Char(m_hashArr[i].data).c_str(), aginfo->type, aginfo->sendlen, aginfo->rawbuf);
#endif
			}
			// 发送数据部分
			{
				const unsigned int nDataLen = aginfo->strSendDataBuf.length();
				if (nDataLen > aginfo->senddatalen)
				{
					{
						CAutoLock lock(m_nSockLock);

						bool  bSame = HasHttpSock(i, aginfo->sock);
						if (! bSame)	// 绑定的sock不一样
						{
				#ifdef QVODDEBUG
							PHttpAgentSockData pData = dynamic_cast<PHttpAgentSockData>(m_SocketArray[i]->GetData());
							HASH sockHash;
							if (pData)
							{
								sockHash = pData->hash;
							}
							_OUTPUT(ERROR1, "CHttpAgent::OnConnect index=%d hash=%s   sockhash=%s", i, m_SocketArray[i]->GetSocket(), Hash2Char(hash.data).c_str(), Hash2Char(sockHash.data).c_str());
				#endif
							return -1;
						}
						sendlen = m_SocketArray[i]->SendData(aginfo->strSendDataBuf.c_str()+aginfo->senddatalen,(int)(aginfo->strSendDataBuf.length()-aginfo->senddatalen));
					}
					aginfo->sendlen += sendlen;
					aginfo->senddatalen += sendlen;
					if (aginfo->senddatalen == aginfo->strSendDataBuf.length())		// 发送完毕
					{
						aginfo->strSendDataBuf.clear();		// 释放内存
					}

					CMsgPoolInterface::Instance()->InterUpSize(sendlen);
					KEY key;
					CSpeedCounter::Instance()->PushSpeedData(&m_hashArr[i],NULL,sendlen,GlobalUpSpeedCallBack);
					SPEEDATA data;
					data.hash = m_hashArr[i];
					data.size = sendlen;
					CTaskMgrInterFace::Instance()->InterUpSize(data);	//把http速度算到任务的总速度里

#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpUpload::OnConnect [%s] type=%d senddatalen=%d \n", Hash2Char(m_hashArr[i].data).c_str(), aginfo->type, aginfo->senddatalen);
#endif
					if (-1 == sendlen)
					{
						err = QvodNetGetLastError();
						if(QVOD_NET_EWOULDBLOCK == err || EAGAIN == err)		// 等待下一次发送
						{
							sendlen = 1;	// edit by weilijian <0 will close socket
							//return 1;	// edit by weilijian
						}		
					}
				}
				else
				{
	#ifdef QVODDEBUG
					//_OUTPUT(INFO, "CHttpUpload CAgentInfo::_POST sendlen<0");
	#endif
				}
			}
		}
		break;
	}

	m_IfSockConnected[i] = 1;

#ifndef WIN32
	if (aginfo->senddatalen >= aginfo->strSendDataBuf.length())	// 数据发送完成，置读事件
	{
		pfds[i].events = POLLIN;
	}
#endif
	return sendlen;
}

int CHttpAgent::OnConnectFail(int i)
{
	const HASH hash = GetSockBindHash(i);
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
	{
		return -1;
	}
	aginfo->errcode = 11;
#ifdef QVODDEBUG
	char msg0[50]={0};
	memcpy(msg0,Hash2Char(m_hashArr[i].data).c_str(),40);
	_OUTPUT(ERROR1,"\nCHttpAgent::OnConnectFail ,%s\n",msg0);
#endif
	TerminalSock(hash, false, false, true);
	return 0;
}

int CHttpAgent::OnClose(const HASH& hash)
{
	CAutoLock lock(m_nSockLock);
	for(int i=0;i<m_nSocket;i++)
	{
		if(m_hashArr[i]==hash)
		{
			return OnClose(i);
		}
	}
	return 0;
}

int CHttpAgent::OnClose(int i)
{
	CAutoLock lock(m_nSockLock);
	//防止溢出
	if(i>=m_nSocket || m_nSocket>=64)
		return -1;

	//加锁删除socket
	m_SocketArray[i]->Close();
	//QvodCloseSocket(m_SocketArray[i]);

	AutoPtr<CAgentInfo> aginfo;
	if(GetAgentInfo(m_hashArr[i], &aginfo))
	{
		aginfo->reuse = false;
		aginfo->last_recv_tick = 0;
	}

#ifdef WIN32
	WSACloseEvent(m_EventArray[i]);
#endif

	if(m_nSocket>0)
	{
		if (i != --m_nSocket)
		{
			//memset( m_hashArr[i].data, 0x0, 20);
			m_hashArr[i] = m_hashArr[m_nSocket];	

			//SafeDelete(m_SocketArray[i]);
			SafeRelease(m_SocketArray[i]);
			m_SocketArray[i] = m_SocketArray[m_nSocket];
			m_SocketArray[m_nSocket] = NULL;
		}
		
#ifdef WIN32
		m_EventArray[i] = m_EventArray[m_nSocket];
#else
		pfds[i] = pfds[m_nSocket];
		pfds[m_nSocket].fd = QVOD_INVALID_SOCKET;
		pfds[m_nSocket].events = 0;
		pfds[m_nSocket].revents = 0;
#endif
	}
	return 0;
}

bool CHttpAgent::HasHttpSock(const int &i, const unsigned& sock)		// sock对象绑定的sock是否一样
{
	CAutoLock lock(m_nSockLock);
	if (NULL == m_SocketArray[i])		// 接受断开
	{
	#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::HasCHttpSock  m_SocketArray[%d] is NULL ", i);
	#endif
		return false;
	}
	if (m_SocketArray[i]->GetSocket() != sock)	// 绑定的sock不一样
	{
	#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::HasCHttpSock m_SocketArray[%d]=%d, sock=%d", i, m_SocketArray[i]->GetSocket(), sock);
	#endif
		return false;
	}
	return true;
}

int CHttpAgent::CopyErrBuf(const HASH& hash,char* buf)
{
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
		return -1;
	if(! aginfo->strErrBuf.empty())
	{
		memcpy(buf,aginfo->strErrBuf.c_str(),aginfo->strErrBuf.length());
		return aginfo->strErrBuf.length();
	}
	return 0;
}

bool CHttpAgent::RecvPacket(char* buf,int& len)
{
	CAutoLock lock(m_msgQueueLock);
	if(!m_msgQueue.empty())
	{
		agentmsghead* pagentmsg = m_msgQueue.front();
		if(len<pagentmsg->len+4)
			return false;
		len = pagentmsg->len+4;
		memcpy(buf,pagentmsg,len);
		delete pagentmsg;
		m_msgQueue.pop();
		return true;
	}
	return false;
}

unsigned int CHttpAgent::QueryFileSpeed(const HASH& hash)
{
	AutoPtr<CAgentInfo> aginfo;
	if(GetAgentInfo(hash,&aginfo))
	{
		//未下载完成时候
		//QVOD_DWORD curtime = QvodGetTime();
		if(aginfo->downloadlen!=aginfo->filelen)
		{
			return aginfo->xsecdownlen/SPCINTERVALTIME;
		}
	}
	return 0;
}

QVOD_UINT64 CHttpAgent::QueryFileDownLoadLen(const HASH& hash)
{
	AutoPtr<CAgentInfo> aginfo;
	if(GetAgentInfo(hash,&aginfo))
	{
		//未下载完成时候
		if(aginfo->downloadlen!=aginfo->filelen)
		{
			return aginfo->downloadlen;
		}
	}
	return 0;
}

QVOD_UINT64 CHttpAgent::QueryFileFileLen(const HASH& hash)
{
	AutoPtr<CAgentInfo> aginfo;
	if(GetAgentInfo(hash,&aginfo))
	{
		return aginfo->wholeFilelen;
	}
	return 0;
}

bool CHttpAgent::GetAgentInfo(const HASH& hash,CAgentInfo** agent)
{
	CAutoLock lock(m_agentLock);
	map<HASH,CAgentInfo*>::iterator it=m_agentMap.find(hash);
	if(it!=m_agentMap.end())
	{
		*agent = it->second;
		(*agent)->duplicate();
		return true;
	}
	return false;
}

void CHttpAgent::DelAgentInfo(const HASH& hash)
{
	CAutoLock lock(m_agentLock);
	map<HASH,CAgentInfo*>::iterator it=m_agentMap.find(hash);
	if(it!=m_agentMap.end())
	{
		if (it->second)
		{
			it->second->release();
		}
		m_agentMap.erase(it);
	}
}

void CHttpAgent::ClearAgentInfoMap()
{
	CAutoLock lock(m_agentLock);
	map<HASH,CAgentInfo*>::iterator it = m_agentMap.begin();
	for (; it != m_agentMap.end(); it++)
	{
		it->second->release();
	}
	m_agentMap.clear();
}

void CHttpAgent::InterDownSize(const HASH& hash,int size)
{
	AutoPtr<CAgentInfo> aginfo;
	if(GetAgentInfo(hash,&aginfo))
	{
		aginfo->xsecdownlen += size;
	}
}

void CHttpAgent::InterDownSize(int size)
{
	m_iGlobalDownSpeed += size;
}

bool CHttpAgent::TerminalSock(const int& index,bool initiative,bool ieclose,bool onConnectFailed)
{
	HASH hash = GetSockBindHash(index);
	if (hash != m_hashArr[index])		// sock绑定的hash和
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::TermiateSock index=%d hash=%s  m_hashArr[%d]=%s", index, Hash2Char(hash.data).c_str(), index, Hash2Char(m_hashArr[index].data).c_str());
#endif
	}
	return TerminalSock(hash, initiative, ieclose, onConnectFailed);
}

bool CHttpAgent::TerminalSock(const HASH& hash,bool initiative,bool ieclose,bool onConnectFail)
{
	CAutoLock lock(m_agentLock);
	AutoPtr<CAgentInfo> pAgentInfo;
	if (GetAgentInfo(hash, &pAgentInfo))
	{
		if (pAgentInfo->type == CAgentInfo::_RANGE || pAgentInfo->type == CAgentInfo::_START ||
			CAgentInfo::_M3U8 == pAgentInfo->type || CAgentInfo::_M3U8TS == pAgentInfo->type)
		{
			//关闭连接
			OnClose(hash);
			if (!initiative && !ieclose && pAgentInfo->connectimes < 5 && pAgentInfo->strErrBuf.empty() && 0 ==pAgentInfo->downloadlen)
			{
				if ( 0 == ReConnect(hash))
				{
					// 重新解析域名IP
					std::string strHost, strParams;
					std::string strUrl = NetUrl::GetURLHost(pAgentInfo->strOrgUrl, strHost, strParams);
					CDNSCache::Instance()->Remove(strHost);
					CDNSCache::Instance()->Inquire(strHost);

					pAgentInfo->connectimes++;
					if (pAgentInfo->connectimes >= 3)			// 重试超过3次则通知上层
					{
						CallBackToUI(5, pAgentInfo->connectimes, Hash2Char(hash.data).c_str(), 40);
					}
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent ReConnect connectimes=%d", pAgentInfo->connectimes);
#endif
					return true;
				}
			}
			if(!pAgentInfo->bshare)
			{
				pAgentInfo->bshare=true;								
				char* pMsg = NULL;
				switch(pAgentInfo->type)
				{
				case CAgentInfo::_RANGE:
				case CAgentInfo::_START:
					{							
						agentrange *range = new agentrange;
						range->head.len = sizeof(agentrange) - 4;
						range->head.id = AGENTRANGE;
                        range->buf = NULL;
                        range->len = 0;
						if (pAgentInfo->downloadlen)
						{
							if (pAgentInfo->downloadlen == pAgentInfo->filelen)
							{
								range->result = 0;	// complete
							}
							else
							{
								range->result = 1;	// partial
							}
							CAutoLock lock(m_fileLock);
							range->start = pAgentInfo->pos;
							range->buf = pAgentInfo->pdatabuf;
							pAgentInfo->pdatabuf = NULL;	// released in msgpool
							range->len =pAgentInfo->downloadlen;
							range->filesize = pAgentInfo->wholeFilelen;
						} 
						else
						{
							range->result = 2;	// no data
							if (pAgentInfo->connectimes >= 5)
							{
								range->result = 4;	// no data, more than retry times
							}
						}
						if (7 == pAgentInfo->errcode)
						{
							//http返回不是200或重定向，链接失效了
							range->result = 3;
							range->errcode = pAgentInfo->httpcode;
						}							
						memcpy(range->hash,hash.data,20);
#ifdef QVODDEBUG
						_OUTPUT(INEED,"CHttpAgent push AGENTRANGE msg, start=%llu,len=%u,tick=%d retrys=%d\n",pAgentInfo->pos,pAgentInfo->downloadlen, (QvodGetTime()-pAgentInfo->initime), pAgentInfo->connectimes);
#endif
						pMsg = (char*)range;
					}
					break;
				case CAgentInfo::_M3U8:
					{
						agentm3u8* m3u8 = new agentm3u8;
						m3u8->head.len = sizeof(agentm3u8)-4;
						m3u8->head.id = AGENTM3U8;
                        m3u8->buf = NULL;
                        m3u8->len = 0;
						if (pAgentInfo->downloadlen)
						{
							if (pAgentInfo->downloadlen == pAgentInfo->filelen)
							{
								m3u8->result = 0;	// complete
							}
							else
							{
								m3u8->result = 1;	// partial
							}
							CAutoLock lock(m_fileLock);
							m3u8->buf = pAgentInfo->pdatabuf;
							pAgentInfo->pdatabuf = NULL;	// released in msgpool
							m3u8->len = pAgentInfo->downloadlen;
						} 
						else
						{
							m3u8->result = 2;	// no data
						}
						if (7 == pAgentInfo->errcode)
						{
							//http锟斤拷锟截诧拷锟斤拷200锟斤拷锟截讹拷锟斤拷锟斤拷锟斤拷失效锟斤拷
							m3u8->result = 3;
							m3u8->errcode = pAgentInfo->httpcode;
							// 下载失败，删除buf数据
							SafeDeleteArray(m3u8->buf);
							m3u8->len =pAgentInfo->strOrgUrl.length()+1;
							m3u8->buf = new char[m3u8->len+1];
							memset(m3u8->buf, 0, m3u8->len);
							strcpy(m3u8->buf, pAgentInfo->strOrgUrl.c_str());
						
							const char* pFilenamePos = strrchr(pAgentInfo->strOrgUrl.c_str(),'/');
							if (pFilenamePos)
							{
								++pFilenamePos;
								AddM3u8DownError(hash, string(pFilenamePos), pAgentInfo->strErrBuf);
							}
						}	
						memcpy(m3u8->hash,hash.data,20);
						pMsg = (char*)m3u8;
#ifdef QVODDEBUG
						_OUTPUT(INFO,"CHttpAgent push %s AGENTM3U8 msg,ret = %d,tick=%d retrys=%d\n",Hash2Char(hash.data).c_str(),m3u8->result, (QvodGetTime()-pAgentInfo->initime), pAgentInfo->connectimes);
#endif
					}
					break;
				case CAgentInfo::_M3U8TS:
					{
						agentm3u8ts* ts = new agentm3u8ts;
						ts->head.len = sizeof(agentm3u8ts)-4;
						ts->head.id = AGENTM3U8TS;
                        ts->buf = NULL;
                        ts->len = 0;
                        ts->filesize = 0;
						if (pAgentInfo->downloadlen)
						{
							if (pAgentInfo->downloadlen == pAgentInfo->filelen)
							{
								ts->result = 0;	// complete
							}
							else
							{
								ts->result = 1;	// partial
							}
							CAutoLock lock(m_fileLock);
							ts->buf = pAgentInfo->pdatabuf;
							pAgentInfo->pdatabuf = NULL;	// released in msgpool
							ts->len = pAgentInfo->downloadlen;
							ts->filesize = pAgentInfo->wholeFilelen;
							ts->start = pAgentInfo->pos;
						} 
						else
						{
							ts->result = 2;	// no data
						}
						const char* pFilenamePos = strrchr(pAgentInfo->strOrgUrl.c_str(),'/');
						if (pFilenamePos)
						{
							++pFilenamePos;
							ts->filename = pFilenamePos;
						}
						if (7 == pAgentInfo->errcode)
						{
							// http 下载错误 40X或50X 失效
							ts->result = 3;
							ts->errcode = pAgentInfo->httpcode;

							AddM3u8DownError(hash, ts->filename, pAgentInfo->strErrBuf);
						}	
						memcpy(ts->hash,hash.data,20);
						if (! pAgentInfo->strHash.empty())
						{
							memcpy(ts->hash,pAgentInfo->strHash.c_str(),20);
							DelOneTaskHash(HASH(pAgentInfo->strHash.c_str()), hash);
						}
						pMsg = (char*)ts;
#ifdef QVODDEBUG
						_OUTPUT(INFO,"CHttpAgent push %s AGENTM3U8TS msg,downlen = %lld,filesize = %lld, tick=%d retrys=%d\n",ts->filename.c_str(),ts->len,ts->filesize, (QvodGetTime()-pAgentInfo->initime), pAgentInfo->connectimes);
#endif
					}
					break;
				}
				
				if (pMsg)
				{
					CAutoLock lock(m_msgQueueLock);
					m_msgQueue.push((agentmsghead*)pMsg);
				}					
			}
			DelAgentInfo(hash);
		}
		else
		{
			//关闭连接
			OnClose(hash);

			//删除缓存信息
			DelAgentInfo(hash);
		}
		return true;
	}
	return false;
}

void CHttpAgent::deleteUnusedSock()
{
	QVOD_DWORD timestamp = QvodGetTime(); // 注意这里的类型
	if(timestamp - m_timestamp < 1000)
	{
		return;
	}
	vector<HASH> vDelHashs;
	CAutoLock lock(m_agentLock);
	map<HASH,CAgentInfo*>::iterator it = m_agentMap.begin();
	for(;it != m_agentMap.end();)
	{
		HASH hash = it->first;
		CAgentInfo* aginfo = it->second;
        if (NULL == aginfo)
        {
            it++;
            continue;
        }
        
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CHttpAgent deleteunselected  [%s] type=%d connected=%d lasttick=%lu downtime=%lu maxdowntime=%u, timestamp=%lu reuse=%d downlen=%lld contentlen=%lld",
			Hash2Char(hash.data).c_str(), aginfo->type, aginfo->connected, aginfo->last_recv_tick, aginfo->downloadtime, aginfo->iMaxDownloadTime, timestamp, aginfo->reuse, aginfo->downloadlen, aginfo->contentlen);
#endif
		if ((aginfo->type == CAgentInfo::_RANGE || aginfo->type == CAgentInfo::_START ||
                  CAgentInfo::_M3U8 == aginfo->type || CAgentInfo::_M3U8TS == aginfo->type) )
		{
			//超过10秒没接收到数据或使用、下载总时间超过最大时间、5秒内没连上cdn，关掉连接
			bool bTimeout = false;
			if (aginfo->last_recv_tick >0 )
			{
				bTimeout = timestamp >= (g_cfg.m_uMaxRecvTime*1000 + aginfo->last_recv_tick);
			}
			if (bTimeout ||
				((timestamp >= (aginfo->downloadtime + aginfo->iMaxDownloadTime*1000)) && !aginfo->reuse) ||
				((!aginfo->connected && (timestamp >= aginfo->downloadtime + g_cfg.m_uMaxConnectTime*1000)) && !aginfo->reuse))	// care of negative wrap
			{
				int type = 0;

				if (aginfo->last_recv_tick>0 && (timestamp >= (g_cfg.m_uMaxRecvTime*1000 + aginfo->last_recv_tick)))
				{
					type = 1;
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent [%s] no more range request for %usec, close socket\n", Hash2Char(hash.data).c_str(), g_cfg.m_uMaxRecvTime);
#endif
				}
				else if ((timestamp >= (aginfo->downloadtime + aginfo->iMaxDownloadTime*1000)) && !aginfo->reuse)
				{
					type = 2;
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent [%s] recv or connect timeout(>=%usec), close socket\n", Hash2Char(hash.data).c_str(), 
						(timestamp-aginfo->downloadtime) / 1000);
#endif
				}
				else if ((!aginfo->connected && (timestamp >= aginfo->downloadtime + g_cfg.m_uMaxConnectTime*1000)) && !aginfo->reuse)
				{
					type = 0;
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent [%s] connect timeout(>=%usec), close socket\n", Hash2Char(hash.data).c_str(), 
						(timestamp-aginfo->downloadtime) / 1000);
#endif
				}
				vDelHashs.push_back(hash);
//				CallBackToUI(5, type, Hash2Char(it->first.data).c_str(), 40);
//				aginfo->bshare = true;
//                char* pMsg = NULL;
//                if(aginfo->type == CAgentInfo::_RANGE || aginfo->type == CAgentInfo::_START)
//                {
//                    CAutoLock lock(m_fileLock);
//                    agentrange *range = new agentrange;
//                    range->head.len = sizeof(agentrange)-4;
//                    range->head.id = AGENTRANGE;
//                    range->result = 1;
//                    range->start = aginfo->pos;
//                    range->buf = aginfo->pdatabuf;
//                    aginfo->pdatabuf = NULL;	// released in msgpool
//                    range->len = aginfo->downloadlen;
//                    range->filesize = aginfo->wholeFilelen;
//                    memcpy(range->hash,it->first.data,20);
//                    pMsg = (char*)range;
//#ifdef QVODDEBUG
//                    _OUTPUT(INEED,"CHttpAgent push AGENTRANGE msg, start=%llu,len=%llu\n",aginfo->pos,aginfo->downloadlen);
//#endif
//                }
//                else if(CAgentInfo::_M3U8 == aginfo->type)
//                {
//                    CAutoLock lock(m_fileLock);
//                    agentm3u8* m3u8 = new agentm3u8;
//                    m3u8->head.len = sizeof(agentm3u8)-4;
//                    m3u8->head.id = AGENTM3U8;
//                    m3u8->result = 1;
//                    m3u8->buf = aginfo->pdatabuf;
//                    aginfo->pdatabuf = NULL;	// released in msgpool
//                    m3u8->len = aginfo->downloadlen;
//                    memcpy(m3u8->hash, it->first.data, 20);
//                     pMsg = (char*)m3u8;
//#ifdef QVODDEBUG
//                    _OUTPUT(INEED,"CHttpAgent push AGENTM3U8 msg,len = %lld\n",m3u8->len);
//#endif
//                }
//                else    if(CAgentInfo::_M3U8TS == aginfo->type)
//                {
//                    CAutoLock lock(m_fileLock);
//                    agentm3u8ts* ts = new agentm3u8ts;
//                    ts->head.len = sizeof(agentm3u8ts)-4;
//                    ts->head.id = AGENTM3U8TS;
//                    ts->result = 1;
//                    ts->buf = aginfo->pdatabuf;
//                    aginfo->pdatabuf = NULL;	// released in msgpool
//                    ts->len = aginfo->downloadlen;
//                    ts->start = aginfo->pos;
//                    ts->filesize = aginfo->wholeFilelen;
//					const char* pFilenamePos = strrchr(aginfo->strOrgUrl.c_str(),'/');
//                    if (pFilenamePos)
//                    {
//                        ++pFilenamePos;
//                        ts->filename = pFilenamePos;
//                    }
//                    memcpy(ts->hash, it->first.data, 20);
//                    pMsg = (char*)ts;
//#ifdef QVODDEBUG
//                    _OUTPUT(INEED,"CHttpAgent push AGENTM3U8TS msg,name = %s,start = %lld,len = %lld,filesize = %lld\n",
//                            ts->filename.c_str(),ts->start,ts->len,ts->filesize);
//#endif
//                }
//                if(pMsg)
//                {
//                    m_msgQueueLock.Lock();
//                    m_msgQueue.push((agentmsghead*)pMsg);
//                    m_msgQueueLock.Unlock();
//                }
//				OnClose(hash);
//				it->second->release();
//				m_agentMap.erase(it++);

//				CallBackToUI(5, type, Hash2Char(it->first.data).c_str(), 40);
//				continue;
			}
		}
		it++;
	}

	for (int i=0; i<vDelHashs.size(); i++)
	{
		TerminalSock(vDelHashs[i], false, false, false);
	}
	m_timestamp = timestamp;
}

QVOD_THREAD_RETURN CHttpAgent::Routine(void* param)
{
#ifdef WIN32
	CHttpAgent* pAgent = (CHttpAgent*)param;
	unsigned int index;
	WSANETWORKEVENTS NetWorkEvents;
	while (pAgent->m_status == CHttpAgent::RUNNING)
	{
		if(pAgent->m_status == CHttpAgent::STOP)//用户关闭线程
		{
			break;
		}
		if(pAgent->m_nSocket < 1)
		{
			Sleep(16);
			continue;
		}
		//删除超时的连接
		pAgent->deleteUnusedSock();

		//开始处理事件
		index = WSAWaitForMultipleEvents(pAgent->m_nSocket,pAgent->GetEventArray(0),FALSE,10,FALSE);
		if (index == WSA_WAIT_FAILED )
		{
			continue;
		}
		if(index == WSA_WAIT_TIMEOUT)
		{
			continue;
		}
		index = index - WSA_WAIT_EVENT_0;
		int i;
		//遍历后面所有事件，看后面事件是否被传信
		for ( i = index;i<pAgent->m_nSocket;i++)
		{
			index = WSAWaitForMultipleEvents(1,pAgent->GetEventArray(i),TRUE,0,FALSE);
			if (index == WSA_WAIT_FAILED || index == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
			ResetEvent(pAgent->m_EventArray[i]);
			WSAEnumNetworkEvents(pAgent->GetSockArray(i),*pAgent->GetEventArray(i),&NetWorkEvents);
			const HASH hash = pAgent->m_hashArr[i];
			//FD_READ
			if (NetWorkEvents.lNetworkEvents & FD_READ)
			{
				if (NetWorkEvents.iErrorCode[FD_READ_BIT])
				{
					AutoPtr<CAgentInfo> aginfo;
					if(pAgent->GetAgentInfo(hash,&aginfo))
					{
						aginfo->errcode = 12;
						aginfo->errcode |= (WSAGetLastError() << 16);
					}
					pAgent->TerminalSock(hash);
					continue;
				}
				else
				{
					int result = pAgent->OnData(i);
					if(-1 == result)
					{
						pAgent->TerminalSock(hash);
						continue;
					}
				}
			}

			if(NetWorkEvents.lNetworkEvents & FD_CONNECT)
			{
				if (NetWorkEvents.iErrorCode[FD_CONNECT_BIT])
				{
					pAgent->OnConnectFail(i);
				}
				else
				{
					pAgent->OnConnect(i);
				}
			}

			if (NetWorkEvents.lNetworkEvents & FD_CLOSE )
			{
				AutoPtr<CAgentInfo> aginfo;
				if(pAgent->GetAgentInfo(hash,&aginfo))
				{
					aginfo->errcode = 13;
				}
				pAgent->TerminalSock(hash);
			}
		}
	}
	//删除套接字
	for(int i = 0;i< pAgent->m_nSocket;i++)
	{
		QvodCloseSocket(pAgent->GetSockArray(i));
	}
	//释放m_agentmap
	pAgent->ClearAgentInfoMap();
	pAgent->m_nSocket = 0;

	return ;
#else
	CHttpAgent* pAgent = (CHttpAgent*)param;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10 * 1000;

	while(pAgent->m_status == CHttpAgent::RUNNING) {

		if(pAgent->m_status == CHttpAgent::STOP)//用户关闭线程
		{
			break;
		}
		if (pAgent->m_nSocket < 1)
		{
			QVOD_SLEEP(10);
			continue;
		}
		
		//删除超时的连接
		pAgent->deleteUnusedSock();

		int res = poll(pAgent->pfds, pAgent->m_nSocket, 10);
		if (-1 == res) 
		{
			QVOD_SLEEP(10);
			if (EBADF == errno || EINTR == errno) 
			{
#ifdef QVODDEBUG
				_OUTPUT(FAIL, "CHttpAgent poll fail %d and continue\n", errno);
#endif
				continue;
			}
#ifdef QVODDEBUG
			_OUTPUT(INEED, "CHttpAgent poll error %d\n", errno);
#endif
			continue;
		}
		if (0 == res) 
		{
			continue;
		}

		/* judge if sockets have event */
		for(int i = 0; i< pAgent->m_nSocket; i++) 
		{
			if (pAgent->pfds[i].revents & POLLIN) 
			{
				if (pAgent->m_IfSockConnected[i] != 1) 
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent not connect and recv event\n");
#endif
                    if (! pAgent->TerminalSock(i))
                    {
                        pAgent->OnClose(i);
#ifdef QVODDEBUG
                        _OUTPUT(INFO, "CHttpAgent TerminalSock failed m_IfSockConnected hash=%s\n", Hash2Char(pAgent->m_hashArr[i].data).c_str());
#endif
                    }
					continue;
				}
				int result = pAgent->OnData(i);
				if(-1 == result)
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent recv data failed -1\n");
#endif
					if (! pAgent->TerminalSock(i))
					{
						pAgent->OnClose(i);
#ifdef QVODDEBUG
						_OUTPUT(INFO, "CHttpAgent TerminalSock failed OnData hash=%s\n", Hash2Char(pAgent->m_hashArr[i].data).c_str());
#endif
					}
					continue;
				}
			}
			if (pAgent->pfds[i].revents & POLLOUT)
			{
				int err = 0;
				qvod_socklen_t len = sizeof(err);
				qvod_sock_t s = pAgent->GetSockArray(i);
				if(s <= 0) 
				{
					_OUTPUT(INFO, "CHttpAgent qvod_sock_t s = pAgent->GetSockArray(i) i=%d m_nSocket=%d", i, pAgent->m_nSocket);
					continue;
				}
				if(getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len) == 0) 
				{
					if(err == 0) 
					{
						if (pAgent->OnConnect(i) <= 0)
						{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent OnConnect failed \n");
#endif
							if (! pAgent->TerminalSock(i))
							{
								pAgent->OnClose(i);
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpAgent TerminalSock failed OnConnect hash=%s\n", Hash2Char(pAgent->m_hashArr[i].data).c_str());
#endif
							}
							continue;
						}
					} 
					else 
					{
						pAgent->OnConnectFail(i);
					}
					
				} 
				else 
				{
					pAgent->OnConnectFail(i);
				}
				continue;
			}
			if (pAgent->pfds[i].revents & (POLLERR|POLLHUP|POLLNVAL))
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CHttpAgent have error event %X, %d\n", pAgent->pfds[i].revents, errno);
#endif
				if (-1 == pAgent->OnConnectFail(i))
				{
					pAgent->OnClose(i);
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CHttpAgent TerminalSock failed error hash=%s\n", Hash2Char(pAgent->m_hashArr[i].data).c_str());
#endif
				}
			}
		}

	} /* while */

	//删除套接字
	for(int i = 0;i< pAgent->m_nSocket;i++)
	{
		QvodCloseSocket(pAgent->GetSockArray(i));
	}
	pAgent->ClearAgentInfoMap();
	pAgent->m_nSocket = 0;

	/* attention: no listen socket */

	return 0;
#endif
}

qvod_sock_t CHttpAgent::CreateSocket(bool isvip6)
{
	if (isvip6)
	{
		return socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	}
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int CHttpAgent::GetIndexByHash(const HASH& hash)
{
	CAutoLock lock(m_nSockLock);
	int i;
	for (i = 0; i < m_nSocket; i++)
	{
		if (m_hashArr[i] == hash)
		{
			return i;
		}
	}
	return -1;
}

unsigned int CHttpAgent::GetGlobalDownSpeed(unsigned char *pHash)
{
	unsigned int speed = 0;
	if ( pHash == NULL )
	{
		speed = m_iGlobalDownSpeed / SPCINTERVALTIME;
	}

	return speed;
}

int CHttpAgent::AddRange(const std::string& strUrl,char* reqname,const HASH& hash,QVOD_UINT64 beginpos,QVOD_UINT64 endpos,int type, char* referer)
{
	CAutoLock lock(m_agentLock);
	AutoPtr<CAgentInfo> aginfo;
	if(GetAgentInfo(hash,&aginfo))
	{
		if (beginpos == aginfo->pos && endpos == aginfo->endpos)
		{
			return 1;
		}
		else
		{
			if ((aginfo->strOrgUrl != strUrl) && aginfo->reuse)	// same url, try to reuse this connection
			{
				aginfo->pos = beginpos;
				aginfo->endpos = endpos;

				const char *geturl = strchr(aginfo->strUrl.c_str(), '/');
				if (!geturl)
				{
					geturl = aginfo->strUrl.c_str();
				}
				sprintf(aginfo->rawbuf, get_fmt_of_RANGE, geturl, LIB_VERSION, aginfo->strHost.c_str(), aginfo->pos, aginfo->endpos);

				aginfo->filelen = 0;
				aginfo->contentlen = 0;
				aginfo->bRemoveInvalidHead = false;
				aginfo->downloadlen = 0;
				aginfo->bshare = false;
				aginfo->reuse = false;
				aginfo->connectimes = 0;
				aginfo->iDataBufLen = 0;
				OnConnect(GetIndexByHash(hash));
				aginfo->downloadtime = QvodGetTime();
				aginfo->initime = QvodGetTime();
				aginfo->iMaxDownloadTime = (endpos-beginpos)/16384;
				if (aginfo->iMaxDownloadTime < 5)
				{
					aginfo->iMaxDownloadTime = 5;
				}
				return 1;
			}
			else
			{
				TerminalSock(hash, true);
#ifdef QVODDEBUG
				_OUTPUT(INFO,"CHttpAgent AddRange delete exist agent %s\n",Hash2Char(hash.data).c_str());
#endif
			}
		}
	}

	char cdnhost[250] = {0};
	const char *url = NULL;
	bool bHttps = strUrl.find("https://") != string::npos;
	string strTag = bHttps? "https://": "http://";
    int pos = strUrl.find(strTag);
    if (pos != string::npos)
    {
		pos += strTag.length();
        url = strUrl.c_str() + pos;
        int pos2 = strUrl.find("/",pos);
        if (pos2 != string::npos)
        {
            strcpy(cdnhost,strUrl.substr(pos,pos2-pos).c_str());
        }
    }

	aginfo = new CAgentInfo;
	aginfo->duplicate();
	aginfo->pos = beginpos;
	aginfo->endpos = endpos;
	aginfo->strOrgUrl = strUrl;
	if (url)
	{
		aginfo->strUrl = url;
	}
	strcpy(aginfo->referer,referer);
	aginfo->bHaveRange = (type == CAgentInfo::_RANGE ? true : false);
	aginfo->iMaxDownloadTime = (endpos-beginpos)/16384;
	if (aginfo->iMaxDownloadTime < 5)
	{
		aginfo->iMaxDownloadTime = 5;
	}

	aginfo->strHost = cdnhost;
	const char* pPos = NULL;
	if (url)
	{
		pPos = strstr(url,"/");
		if (pPos)
		{
			url = pPos;
		}
	}
	if (type == CAgentInfo::_START)
	{
		if (strstr(url,"?"))
		{
			sprintf(aginfo->rawbuf, get_fmt_of_START, url,"&start=", aginfo->pos, LIB_VERSION, aginfo->strHost.c_str(), referer);
		}
		else
		{
			sprintf(aginfo->rawbuf, get_fmt_of_START, url,"?start=", aginfo->pos, LIB_VERSION, aginfo->strHost.c_str(), referer);
		}
	}
	else
	{
		sprintf(aginfo->rawbuf, get_fmt_of_RANGE, url, LIB_VERSION, cdnhost, aginfo->pos, aginfo->endpos);
	}
	//连接数限制
	if(m_nSocket >= 64)
	{
		return -1;
	}

	unsigned long ip = 0;
	unsigned short port = bHttps? 443: 80;	//https端口为443 http端口为80
	string strcdnhost = cdnhost;
	pPos = strstr(cdnhost, ":");
	if(pPos)
	{
		port = atoi(pPos+1);
		strcdnhost = strcdnhost.substr(0, pPos-cdnhost);
	}

	const IPAddrInfo ipaddr = GetIpByHost(strcdnhost.c_str());
	qvod_sock_t s = CreateSocket(ipaddr.isIpv6);
	
	CHttpSock *pSock = NULL;
	if (bHttps)
	{
#ifdef SUPPORT_HTTPS
		pSock = new CHttpSSLSock(s);
#else
		_OUTPUT(INFO, "not SUPPORT_HTTPS return!");
		return -1;
#endif
	}
	else
	{
		pSock = new CHttpSock(s);
	}

	PHttpAgentSockData pData = new HttpAgentSockData();
	pData->hash = hash;
	pSock->SetData(pData);

	int iRcvBufLen = 128*1024;
	setsockopt(s,SOL_SOCKET,SO_RCVBUF,(const char*)&iRcvBufLen,sizeof(int));



	//ip = GetHostByName(cdnhost);
	port = htons(port);

	aginfo->ip=ip;
	aginfo->port=port;
	aginfo->sock = s;

	{
		CAutoLock lock(m_agentLock);
		m_agentMap[hash]=aginfo;
	}

	{
		CAutoLock lock(m_nSockLock);					// 加锁访问代码段
#ifdef WIN32
		WSAEVENT event = WSACreateEvent(); 
		if (WSA_INVALID_EVENT == event )
		{
			SafeDelete(pSock);
			return -1;
		}
		m_EventArray[m_nSocket] = event;
		WSAEventSelect(s,event,FD_READ|FD_CONNECT|FD_CLOSE);
#else
		pfds[m_nSocket].fd = s;
		pfds[m_nSocket].events |= (POLLOUT|POLLERR|POLLHUP|POLLNVAL);
		QvodSetNonblocking(s);
#endif

		aginfo->type = (CAgentInfo::AgentType)type;
		//SafeDelete(m_SocketArray[m_nSocket])
		SafeRelease(m_SocketArray[m_nSocket]);
		m_SocketArray[m_nSocket] = pSock;
		m_hashArr[m_nSocket] = hash;
		m_nSocket++;
	}
#ifdef QVODDEBUG
	string msg0=Hash2Char((unsigned char*)hash.data);
	_OUTPUT(ERROR1,"CHttpAgent::AddRange hash %s, url %s, pos %lld,endpos %lld,refer %s\n",msg0.c_str(),aginfo->strUrl.c_str(), aginfo->pos,aginfo->endpos,aginfo->referer);
#endif // QVODDEBUG

	//qvod_sockaddr_t SvrAddr;
	//SvrAddr.sin_addr.s_addr = ip;
	//SvrAddr.sin_port = port;
	//SvrAddr.sin_family = AF_INET;
	//int r = pSock->Connect(SvrAddr);
	int r =pSock->Connect(ipaddr.ipv4, ipaddr.ipv6, port, ipaddr.isIpv6);
	int nLastError = QvodNetGetLastError() ;
	if(r == QVOD_SOCKET_ERROR &&  nLastError!=QVOD_NET_EWOULDBLOCK && nLastError!=EINPROGRESS)
	{
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "CHttpAgent::AddRange connect %s failed %u\n", ipaddr.strIP.c_str(),errno);
#endif // QVODDEBUG
		TerminalSock(hash, true);
		return -1;
	}

#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"CHttpAgent::AddRange Connect  %s,%s,sock %d,ip %s\n",msg0.c_str(),aginfo->strUrl.c_str(), s,ipaddr.strIP.c_str());
#endif
	aginfo->last_recv_tick = QvodGetTime();	// record the beginning tick
	aginfo->downloadtime = QvodGetTime();
	aginfo->initime = QvodGetTime();
	return 1;
}

int CHttpAgent::AddTask(const HASH &hash, const string &url, const string &refer,  QVOD_INT64 start, QVOD_INT64 len)
{
	if (!url.size() || start < 0 || len < 1 || len > MAX_AGENT_RANGE)
	{
		return -1;
	}
	int type = CAgentInfo::_RANGE;
	if (refer.find("hunantv.com")!= string::npos)
	{
		type = CAgentInfo::_START;
	}
	int ret = AddRange(url.c_str(), "", hash, start, start + len - 1, type, const_cast<char *>(refer.c_str()));
	if (ret != 1)
	{
		DelTask(hash);
	}
	return ret;
}

int CHttpAgent::AddTask(const HASH &hash, const HttpPostReq& req)
{
	return -1;
	//return CHttpUploadAgent::Instance()->AddTask(hash, req);
}

int CHttpAgent::DeleteTask(const HASH &hash)
{
	return DelTask(hash);
}

QVOD_INT64 CHttpAgent::GetFileSize(const HASH &hash)
{
	return QueryFileFileLen(hash);
}

int CHttpAgent::GetTaskInfo(const HASH &hash, struct taskinfo *info)
{
	AutoPtr<CAgentInfo> aginfo;
	if (!GetAgentInfo(hash, &aginfo))
	{
		return -1;
	}
	if (info)
	{
		info->start = aginfo->pos;
		info->len = aginfo->endpos - aginfo->pos + 1;
		info->downloaded = aginfo->downloadlen;
		info->status = aginfo->status;
		info->filesize = aginfo->wholeFilelen;
	}
	return 0;
}

int CHttpAgent::Read(const HASH &hash, char *buf, int len, QVOD_INT64 pos)
{
	AutoPtr<CAgentInfo> aginfo;
	if (!buf || !len || !GetAgentInfo(hash, &aginfo))
	{
		return -1;
	}
	CAutoLock lock(m_fileLock);
	if (NULL == aginfo->pdatabuf)
	{
		return -1;
	}
	int n_copy = 0;
	if (pos >= aginfo->pos + aginfo->downloadlen)	// read area that not downloaded
	{
		return 0;
	}
	else if (pos + len <= aginfo->pos + aginfo->downloadlen)	// fully downloaded
	{
		n_copy = len;
	}
	else	// part data can be offered
	{
		n_copy = aginfo->pos + aginfo->downloadlen - pos;
	}
    if (pos>=aginfo->pos && n_copy > 0 && aginfo->pdatabuf)
    {
        memcpy(buf, aginfo->pdatabuf + (pos - aginfo->pos), n_copy);
#ifdef QVODDEBUG
        //_OUTPUT(INFO,"HttpAgent Read pos = %lld,len = %d,copy = %d\n",pos,len,n_copy);
#endif
    }
	return n_copy;
}

int CHttpAgent::ReadTs(const HASH &hash, char *buf, int len, QVOD_INT64 pos)
{
    AutoPtr<CAgentInfo> aginfo;
    if (!buf || !len || !GetAgentInfo(hash, &aginfo))
    {
        return -1;
    }
    CAutoLock lock(m_fileLock);
    if (NULL == aginfo->pdatabuf)
    {
        return -1;
    }
    int n_copy = 0;
    const QVOD_UINT64 nStart = aginfo->uFilePos;
    const QVOD_UINT64 nDownloadLen = aginfo->downloadlen;
    if (pos >= nStart + nDownloadLen)	// read area that not downloaded
    {
        return 0;
    }
    else if (pos + len <= nStart + nDownloadLen)	// fully downloaded
    {
        n_copy = len;
    }
    else	// part data can be offered
    {
        n_copy = nStart + nDownloadLen - pos;
    }
    if (pos>=nStart && n_copy > 0)
    {
        memcpy(buf, aginfo->pdatabuf + (pos - nStart), n_copy);
#ifdef QVODDEBUG
        //_OUTPUT(INFO,"HttpAgent Read pos = %lld,len = %d,copy = %d\n",pos,len,n_copy);
#endif
    }
    return n_copy;
}

int CHttpAgent::AddM3u8TsTask(const HASH& hash,SM3u8TsReq& req )
{
	CAutoLock lock(m_agentLock);
	AutoPtr<CAgentInfo> aginfo;
	QVOD_INT64 beginpos = req.start;
	QVOD_INT64 endpos = beginpos + req.length;
    if (endpos > 0)
    {
        --endpos;
    }
	int type = ( 0 == req.type?CAgentInfo::_M3U8:CAgentInfo::_M3U8TS);
	if(GetAgentInfo(hash,&aginfo))
	{
		if (req.strOrlurl == aginfo->strOrgUrl && beginpos == aginfo->pos && endpos == aginfo->endpos)
		{
			return 1;
		}
		else
		{
			TerminalSock(hash, true);
            QvodSleep(10);
		}
	}
	// 当前链接数不能超过64个
	if(m_nSocket >= 64)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent error m_nSocket>64");
#endif
		return -1;
	}
    char cdnhost[256] = {0};
    const char* url = NULL;
	bool bHttps = req.strOrlurl.find("https://") != string::npos;
	string strTag = bHttps? "https://": "http://";
    int pos = req.strOrlurl.find(strTag);
    if (pos != string::npos)
    {
		pos += strTag.length();
        url =req.strOrlurl.c_str() + pos;
        int pos2 = req.strOrlurl.find("/",pos);
        if (pos2 != string::npos)
        {
            strcpy(cdnhost,req.strOrlurl.substr(pos,pos2-pos).c_str());
        }
    }
	else
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpAgent::AddM3u8TsTask 非http https协议  orgurl=%s", req.strOrlurl.c_str());
#endif
		return -1;
	}
    
	aginfo = new CAgentInfo;
	aginfo->duplicate();
	aginfo->pos = beginpos;
	aginfo->endpos = endpos;
    aginfo->uFilePos = req.uFilePos;
	aginfo->strOrgUrl = req.strOrlurl;
	aginfo->strHash = req.strHash;

	if (! aginfo->strHash.empty())
	{
		AddOneTaskHash(HASH(aginfo->strHash.c_str()), hash);	// 添加子请求hash与所属任务hash对应关系
	}

	if (url)
	{
		aginfo->strUrl = url;
	}
	strcpy(aginfo->referer,req.strReferer.c_str());
	if (beginpos > 0 || endpos > 0)
	{
		aginfo->bHaveRange = true;
	}
	else
	{
		aginfo->bHaveRange = false;
	}
	aginfo->iMaxDownloadTime = (endpos-beginpos)/16384;
	if (aginfo->iMaxDownloadTime < 5)
	{
		aginfo->iMaxDownloadTime = 5;
	}
	aginfo->strHost = cdnhost;
	string strcdnhost = cdnhost;
	unsigned short port = bHttps? 443: 80;
	const char* pPos = strstr(cdnhost, ":");
	if(pPos)
	{
		port = atoi(pPos+1);
		strcdnhost = strcdnhost.substr(0, pPos-cdnhost);
	}
    pPos = strstr(url,"/");
    if (pPos)
    {
        url = pPos;
    }
	if (0 == beginpos && 0 == endpos)
	{
		string strHeader;

		if (CAgentInfo::_M3U8 == type)		// 使用zip压缩
		{
			strHeader += "Accept-Encoding: gzip, deflate\r\n";
			aginfo->iMaxDownloadTime = 45;	// trunk分块传输最大下载耗时
		}
		sprintf(aginfo->rawbuf, get_fmt_of_STANDARD2, url, LIB_VERSION, cdnhost, strHeader.c_str());
	}
	else
	{
		sprintf(aginfo->rawbuf, get_fmt_of_RANGE, url, LIB_VERSION, cdnhost, aginfo->pos, aginfo->endpos);
	}

	const IPAddrInfo ipaddr = GetIpByHost(strcdnhost.c_str());
	qvod_sock_t s = CreateSocket(ipaddr.isIpv6);
	//if (QVOD_INVALID_SOCKET == s)
//	{
//#ifdef QVODDEBUG
//		_OUTPUT(INFO, "CHttpAgent::AddM3u8TsTask socket failed! sock=%d err=%d", s, errno);
//#endif
//		//return -1;
//	}
	CHttpSock *pSock = NULL;
	if (bHttps)
	{
#ifdef SUPPORT_HTTPS
		pSock = new CHttpSSLSock(s);
#else
		_OUTPUT(INFO, "not SUPPORT_HTTPS return!");
		return -1;
#endif
	}
	else
	{
		pSock = new CHttpSock(s);
	}
	
	PHttpAgentSockData pData = new HttpAgentSockData();
	pData->hash = hash;
	pSock->SetData(pData);

	int iRcvBufLen = 128*1024;
	setsockopt(s,SOL_SOCKET,SO_RCVBUF,(const char*)&iRcvBufLen,sizeof(int));

	//unsigned long ip = 0;

	//ip = GetHostByName(strcdnhost.c_str());
	port = htons(port);

	//aginfo->ip=ip;
	aginfo->port=port;
	aginfo->sock = s;

	{
		CAutoLock lock(m_agentLock);
		m_agentMap[hash]=aginfo;
	}
	
	{
		CAutoLock lock(m_nSockLock);		// 加锁访问代码段

	#ifdef WIN32
		WSAEVENT event = WSACreateEvent(); 
		if (WSA_INVALID_EVENT == event )
		{
			SafeDelete(pData);
			SafeDelete(pSock);
			return -1;
		}
		m_EventArray[m_nSocket] = event;
		WSAEventSelect(s,event,FD_READ|FD_CONNECT|FD_CLOSE);
	#else
		pfds[m_nSocket].fd = s;
		pfds[m_nSocket].events |= (POLLOUT|POLLERR|POLLHUP|POLLNVAL);
		QvodSetNonblocking(s);
	#endif

		aginfo->type = (CAgentInfo::AgentType)type;
		//SafeDelete(m_SocketArray[m_nSocket])
		SafeRelease(m_SocketArray[m_nSocket])
		m_SocketArray[m_nSocket] = pSock;
		m_hashArr[m_nSocket] = hash;
		m_nSocket++;
	}

#ifdef QVODDEBUG
	string msg0=Hash2Char((unsigned char*)hash.data);
	_OUTPUT(INFO,"CHttpAgent::AddM3u8Ts  [%s], orgurl %s, sock=%d pos %lld,endpos %lld,refer %s\n",msg0.c_str(),aginfo->strOrgUrl.c_str(), s, aginfo->pos,aginfo->endpos,aginfo->referer);
#endif // QVODDEBUG

	//qvod_sockaddr_t SvrAddr;
	//SvrAddr.sin_addr.s_addr = ip;
	//SvrAddr.sin_port = port;
	//SvrAddr.sin_family = AF_INET;
	//int r = pSock->Connect(SvrAddr);
	int r =pSock->Connect(ipaddr.ipv4, ipaddr.ipv6, port, ipaddr.isIpv6);
	int netErr = QvodNetGetLastError();
	if(r == QVOD_SOCKET_ERROR && netErr != QVOD_NET_EWOULDBLOCK &&
		netErr != EINPROGRESS)
	{
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "CHttpAgent::AddM3u8Ts connect %s failed %u\n", ipaddr.strIP.c_str(),errno);
#endif // QVODDEBUG
        TerminalSock(hash, true);
		/*
		51是由ipv4切到ipv6时，connect ipv4返回的错误
		65是由ipv6切到ipv4时，connect ipv6返回的错误
		这两种情况都应该重新解析dns到正确的ipv4或ipv6
		*/

		if (51 == netErr || 65  == netErr)	
		{
			// 重新解析一次域名
			CDNSCache::Instance()->Inquire(strcdnhost, true);
		}
		return -1;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO,"CHttpAgent::AddM3u8Ts Connect  %s,%s,sock %d,ip %s \n",msg0.c_str(),aginfo->strUrl.c_str(), s,ipaddr.strIP.c_str());
#endif
	aginfo->last_recv_tick = QvodGetTime();	// record the beginning tick
	aginfo->downloadtime = QvodGetTime();
	aginfo->initime = QvodGetTime();
	return 1;
}


int CHttpAgent::ReadM3u8Ts(const HASH &hash,string& filename, char *buf, int len, QVOD_INT64 pos )
{
	AutoPtr<CAgentInfo> aginfo;
	if (GetAgentInfo(hash,&aginfo))
	{
		const char* pFilenamePos = strrchr(aginfo->strOrgUrl.c_str(),'/');
		if (pFilenamePos)
		{
			++pFilenamePos;
			if (filename == string(pFilenamePos))
			{
				return Read(hash,buf,len,pos);
			}
		}
	}

	HASH childHash;
	if (FindOneTaskHash(hash, filename,  childHash))
	{
		return ReadM3u8Ts(childHash, filename, buf, len, pos);
	}
	return -1;
}

QVOD_INT64 CHttpAgent::GetM3u8TsSize(const HASH &hash,const string& filename )
{
	AutoPtr<CAgentInfo> aginfo;
	if (GetAgentInfo(hash,&aginfo))
	{
		const char* pFilenamePos = strrchr(aginfo->strOrgUrl.c_str(),'/');
		if (pFilenamePos)
		{
			++pFilenamePos;
			if (filename == string(pFilenamePos))
			{
				return aginfo->wholeFilelen;
			}
		}
	}
	// 从该任务hash查找分片hash任务
	HASH childHash;
	if (FindOneTaskHash(hash, filename,  childHash))
	{
		return GetM3u8TsSize(childHash, filename);
	}
	return 0;
}

bool CHttpAgent::GetM3u8DownError(const HASH& hash, const string& filename, string& strErrorInfo)
{
	CAutoLock lock(m_errorLock);

	std::map<HASH,map<string, string> >::iterator itr = m_mHttpErrors.find(hash);
	if (itr != m_mHttpErrors.end())
	{
		map<string, string>::iterator itTs = itr->second.find(filename);
		if (itTs != itr->second.end())
		{
			strErrorInfo = itTs->second;
			return true;
		}
	}
	return false;
}

void CHttpAgent::AddM3u8DownError(const HASH& hash, const string& filename, const string& strErrorInfo)
{
	CAutoLock lock(m_errorLock);

	std::map<HASH,map<string, string> >::iterator itr = m_mHttpErrors.find(hash);
	if (itr != m_mHttpErrors.end())
	{
		map<string, string> &mErros = itr->second;
		mErros[filename] = strErrorInfo;
		return ;
	}
	map<string, string> mErros;
	mErros[filename] = strErrorInfo;
	m_mHttpErrors[hash] = mErros;
}


std::string CHttpAgent::GetHeadValue(const char* pStrInfo,  const char* pKey)
{
	std::string strRet;
	const char* pStr = strstr(pStrInfo, pKey);
	if (pStr)
	{
		pStr += strlen(pKey);
		if (! strstr(pKey, ": "))
		{
			pStr += 2;		//": ";
		}
		const char* pEndStr = strstr(pStr, "\r\n");
		if (pEndStr)
		{
			strRet.assign(pStr, pEndStr-pStr);
		}
	}
	return strRet;
}

bool CHttpAgent::IsChunkEnd(const std::string& strBuf, QVOD_UINT64 length, QVOD_UINT64 &knownLen/*已知的chunk长度, 用于内存申请*/)
{
	int nPos = strBuf.find("0\r\n\r\n");
	if (nPos != std::string::npos)
	{
		knownLen = nPos;
		return true ;
	}

	knownLen = length ;

	return false ;
}

//返回值是最后一个未读取完的chunk的起始地址, 返回为0表示chunk结束
// 未读取完可能是因为pChukData数据不完整， 也可能是因为pBuff长度不足
char* CHttpAgent::GetChunkData(char const *pChunkData, QVOD_UINT64 datalen, char *pBuff, QVOD_UINT64 bufflen, QVOD_UINT64 &usedBuffLen, QVOD_UINT64 &handledChunkDataLen)
{
	usedBuffLen = 0;
	handledChunkDataLen = 0;

	char const * p = pChunkData;
	QVOD_UINT64 offset = 0;
	QVOD_UINT64 len = 0;
	QVOD_UINT64 buffoffset = 0;
	do 
	{
		char *pEnd = NULL;
		len = strtoul(p, &pEnd, 16);
		//len = atoi(p);
		if (len == 0)
		{
			break;
		}
		offset += (len  + (pEnd - p) + 4);//\r\n...\r\n
		if (datalen > offset//这个chunk已接受完
			&& buffoffset + len <= bufflen//buff 长度足够
			)
		{
			const char *pDat = p + (pEnd - p) + 2;			// 213\r\n
			memcpy(pBuff + buffoffset, pDat, len);
			usedBuffLen += len;
			handledChunkDataLen += len + (pEnd - p) + 4; //123\r\n...\r\n
			buffoffset += len;
			p = pChunkData + offset;
		}
		else
		{
			return const_cast<char*>(p);
		}
	} while (true);
	return 0;
}

// GZIP解压缩实现 edit by 魏理坚 2016/12/1
bool CHttpAgent::UnGzip(const char* buf, const int &len, string& strBuf)
{
	bool bRet = false;
	int ret = 0;
	unsigned int have = 0;
	z_stream strm;

	  /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, 47);							// 初始化z_stream结构体
    if (ret != Z_OK)
        return ret;

	QPtr::AutoArrayPtr<unsigned char> pBuf = new unsigned char[CHUNK+1];
	do {
		int nComPressLen = (len-have) > CHUNK? CHUNK: (len-have);

		if (0 == nComPressLen)
		{
			break;
		}
		strm.avail_in = nComPressLen;
		strm.next_in = (z_const Bytef *)(buf +have);

		do {
			strm.avail_out = CHUNK;
			strm.next_out = pBuf;

			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
			
			unsigned int nLen = CHUNK - strm.avail_out;	// 本次已解压的数据长度
			have += nLen;
			strBuf.append((const char*)(unsigned char*)pBuf, nLen);
		} while (strm.avail_out == 0);

	} while (ret != Z_STREAM_END);		 /* done when inflate() says it's done */

	 /* clean up and return */
    (void)inflateEnd(&strm);

	 return ret == Z_STREAM_END;
}


void CHttpAgent::AddOneTaskHash(const HASH& taskHash, const HASH& childHash)
{
	CAutoLock lock(m_hashonetaskLock);
	m_mOneTaskHashs[taskHash].insert(childHash);
}

void CHttpAgent::DelOneTaskHash(const HASH& taskHash, const HASH& childHash)
{
	CAutoLock lock(m_hashonetaskLock);
	std::map<HASH, set<HASH> >::iterator itr=m_mOneTaskHashs.find(taskHash);
	if (itr != m_mOneTaskHashs.end())
	{
		itr->second.erase(childHash);
	}	
}

void CHttpAgent::DelOneTaskSock(const HASH& taskHash)
{
	set<HASH> hashSets;
	{
		CAutoLock lock(m_hashonetaskLock);			// 代码块，防止死锁

		std::map<HASH, set<HASH> >::iterator itr=m_mOneTaskHashs.find(taskHash);
		if (itr != m_mOneTaskHashs.end())
		{
			hashSets = itr->second;
		}
	}

#ifdef QVODDEBUG
	for (set<HASH>::iterator it=hashSets.begin(); it!=hashSets.end(); it++)
	{
		_OUTPUT(INFO, "DelOneTaskSock taskhash=%s childhash=%s", Hash2Char(taskHash.data).c_str(), Hash2Char((*it).data).c_str());
	}
#endif
	{
		CAutoLock lock(m_agentLock);		// 这里需要加锁批量删除
		for (set<HASH>::iterator it=hashSets.begin(); it!=hashSets.end(); it++)
		{
			TerminalSock((*it), true);
		}
	}
}

bool CHttpAgent::FindOneTaskHash(const HASH& taskHash, const string& filename, HASH& outHash)
{
	CAutoLock lock(m_hashonetaskLock);
	std::map<HASH, set<HASH> >::iterator itr=m_mOneTaskHashs.find(taskHash);
	if (itr != m_mOneTaskHashs.end())
	{
		set<HASH> &setHash  = itr->second;
		for (set<HASH>::iterator it=setHash.begin(); it!= setHash.end(); it++)
		{
			AutoPtr<CAgentInfo> aginfo;
			if (GetAgentInfo((*it),&aginfo))
			{
				const char* pFilenamePos = strrchr(aginfo->strOrgUrl.c_str(),'/');
				if (pFilenamePos)
				{
					++pFilenamePos;
					if (filename == string(pFilenamePos))
					{
						outHash = (*it);
						return true;
					}
				}
			}	
		}
	}	
	return false;
}


bool CHttpAgent::GetAgentDownload(const HASH& hash,int& downloadlen, int& totallen)
{
	AutoPtr<CAgentInfo> pAgentInfo;
	if (GetAgentInfo(hash, &pAgentInfo))
	{
		downloadlen = pAgentInfo->downloadlen;
		totallen = pAgentInfo->contentlen;
		return true;
	}
	return false;
}
