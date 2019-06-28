#include "common-commonfun.h"
#include "HttpUpload.h"
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



static const int CHUNK = (256*1024);		// gzip解压缓存数据大小

static char post_fmt_of_STANDARD[] =
"POST %s HTTP/1.1\r\n"
"Accept: */*\r\n"
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
"Content-Length: %d\r\n"
"%s"
"Connection: Keep-Alive\r\n\r\n";

static const int c_RECVBUF_SIZE = 65*1024;			// 一次最多接收65K数据

CHttpUpload::CHttpUpload(void)
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

CHttpUpload::~CHttpUpload(void)
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
WSAEVENT* CHttpUpload::GetEventArray(int index/* = 0*/)
{
	return &m_EventArray[index];
}
#else /* posix */
//nothing
#endif /* posix end */

qvod_sock_t CHttpUpload::GetSockArray(int index)
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

void CHttpUpload::Run()
{
	if(m_status == RUNNING)
		return;
	m_status = RUNNING;
	QvodCreateThread(&m_handle, (qvod_routine_pt)Routine, this); 
}

void CHttpUpload::Stop()
{
	if(m_status == STOP)
		return;
	m_status = STOP;

	QvodWaitForThread(m_handle, NULL); 
	QVOD_SLEEP(10);
}


bool CHttpUpload::DelTask(const HASH& hash)
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

int CHttpUpload::ReConnect(const HASH& hash)
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

	aginfo->strHost = strHost;
    bool bHttps = aginfo->strOrgUrl.find("https://") != string::npos;
	if (strHost.length() > 0)
	{
        aginfo->port = bHttps? 443: 80;
		int nPos = strHost.find(":");
		string strCdnHost = strHost;
		if (nPos != string::npos)
		{
			aginfo->port = atoi(strHost.c_str()+nPos+1);
			strCdnHost = strHost.substr(0, nPos);
		}

		aginfo->ip = GetHostByName(strCdnHost.c_str());
		aginfo->port = htons(aginfo->port);
#ifdef QVODDEBUG
		char strnip[16] = {0};
		IpInt2Str(aginfo->ip, strnip);
		_OUTPUT(INFO, "CHttpUpload %s new ip %s, port %d\n", strCdnHost.c_str(), strnip, ntohs(aginfo->port));
#endif // QVODDEBUG
	}

	qvod_sock_t s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
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

	SOCKADDR_IN SvrAddr;
	SvrAddr.sin_addr.S_un.S_addr = aginfo->ip;
	SvrAddr.sin_port = aginfo->port;
	SvrAddr.sin_family = AF_INET;
	int r = pSock->Connect(SvrAddr);
	//int r = connect(s,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(r == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		SafeDelete(pSock);
		return -1;
	}
#ifdef QVODDEBUG
	string msg2=Hash2Char((unsigned char*)hash.data);
	_OUTPUT(ERROR1,"CHttpUpload::ReConnect  %s,[%s],sock %d\n",msg2.c_str(),aginfo->strUrl.c_str(), s);
#endif // _DEBUG

	{
		CAutoLock lock(m_nSockLock);					// 加锁访问代码段
		SafeRelease(m_SocketArray[m_nSocket]);
		//SafeDelete(m_SocketArray[m_nSocket]);
		m_SocketArray[m_nSocket] = pSock;
		m_EventArray[m_nSocket] = event;
		m_HashSockMap[hash] = s;
		m_nSocket++;
	}


#else
	int iRcvBufLen = 128*1024;
	setsockopt(s,SOL_SOCKET,SO_RCVBUF,(const char*)&iRcvBufLen,sizeof(int));
	QvodSetNonblocking(s);

	qvod_sockaddr_t SvrAddr;
	SvrAddr.sin_addr.s_addr = aginfo->ip;
	SvrAddr.sin_port = aginfo->port;
	SvrAddr.sin_family = AF_INET;
	int r = pSock->Connect(SvrAddr);
	//int r = connect(s,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(r == QVOD_SOCKET_ERROR && QvodNetGetLastError() != EINPROGRESS && QvodNetGetLastError() != EINPROGRESS)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CHttpUpload::ReConnect connect [%s]:%d error = %d\n",inet_ntoa(SvrAddr.sin_addr),ntohs(SvrAddr.sin_port),QvodNetGetLastError());
#endif
		SafeDelete(pSock);
		return -1;
	}

	m_nSockLock.Lock();
	SafeRelease(m_SocketArray[m_nSocket])
	//SafeDelete(m_SocketArray[m_nSocket])
	m_SocketArray[m_nSocket] = pSock;
	m_HashSockMap[hash] = s;
	pfds[m_nSocket].fd = s;
	pfds[m_nSocket].events |= (POLLOUT|POLLERR|POLLHUP|POLLNVAL);
	m_nSocket++;
	m_nSockLock.Unlock();
#endif

	aginfo->last_recv_tick	= QvodGetTime();	// record the beginning tick
	aginfo->downloadtime	= QvodGetTime();
	aginfo->sock			= s;

#ifdef QVODDEBUG
	std::string msg0=Hash2Char((unsigned char*)hash.data);
	_OUTPUT(INFO,"CHttpUpload::ReConnect [%s],[%s]\n",msg0.c_str(),aginfo->strUrl.c_str());
#endif // QVODDEBUG


	return 0;
}

int CHttpUpload::OnData(const qvod_sock_t& s)
{
	HASH sockHash;
	if (! GetSockBindHash(s, sockHash))
	{
		return -1;
	}
	const HASH hash = sockHash;;		// 保存当前的hash
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
	{
		return -1;
	}
//	if(!GetAgentInfo(idxHash,&aginfo))
//	{
//		HASH sockHash = GetSockBindHash(i);			// 找到sock绑定的hash
//		if(sockHash == idxHash || !GetAgentInfo(idxHash,&aginfo))
//		{
//#ifdef QVODDEBUG
//			_OUTPUT(ERROR1, "CHttpUpload::OnData GetAgentInfo false  i=%d sockhash=%s\n", i, Hash2Char(sockHash.data).c_str());
//#endif			
//			return -1;
//		}
//#ifdef QVODDEBUG
//		_OUTPUT(ERROR1, "CHttpUpload::OnData GetSockBindHash  i=%d sockhash=%s hash=%s\n", i, Hash2Char(sockHash.data).c_str(), Hash2Char(idxHash.data).c_str());
//#endif		
//		idxHash = sockHash;
//	}
	

	//读取数据
	char recvBuf[c_RECVBUF_SIZE] = {0};
	int buflen = c_RECVBUF_SIZE;
	char* pbuf = recvBuf;
	int totallen = 0;
	int datalen  = 0;
	int err = 0;
	int nlen;
	while (buflen > 0 )
	{
		AutoPtr<CHttpSock> pSock;
		if (! GetHttpSock(s, &pSock))
		{
			return -1;
		}
		nlen = pSock->RecvData(pbuf,QVOD_MIN2(buflen, c_RECVBUF_SIZE));
		//nlen = recv(m_SocketArray[i],pbuf,QVOD_MIN2(buflen, c_RECVBUF_SIZE),0);
		//接受断开
		if(nlen == 0)
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpUpload::OnData recv error %d, connection closed\n", errno);
#endif
			break;
		}
		//接受异常
		if(nlen == -1)
		{
			err = QvodNetGetLastError();
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpUpload::OnData recv err:%d errno:%d\n", err, errno);
#endif
			if(QVOD_NET_EWOULDBLOCK == err || EAGAIN == err)		// 等待下一次读
			{
				break;
			}
			else
			{
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
	pbuf = recvBuf;
	//处理读取到的数据
	if(datalen == 0)
	{
		if(QVOD_NET_EWOULDBLOCK == err || EAGAIN == err)		// 等待下一次读
		{
			return 0;
		}
		aginfo->errcode = 4;
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpUpload::OnData (datalen == 0 && err != QVOD_NET_EWOULDBLOCK)\n");
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
			_OUTPUT(INFO, "CHttpUpload::OnData begin, [%s] datalen=%d recv:\n%s \n\n", strHash.c_str(), datalen, strHead.c_str());
		}
#endif
		//异常数据,直接返回，丢弃数据
		if(!strstr(recvBuf,"HTTP/1."))
		{
			aginfo->errcode = 5;

#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpUpload::OnData strstr(recvBuf,\"%s\"", recvBuf);
#endif
			return -1;
		}
		char* strend = strstr(recvBuf,"\r\n");
		if(!strend) 
		{
			aginfo->errcode = 6;

#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpUpload::OnData strstr(recvBuf = NULL\n");
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
							_OUTPUT(INFO,"CHttpUpload recv 302 : old orgurl=%s new orgurl=%s\n", aginfo->strOrgUrl.c_str(), strredirect);
#endif // QVODDEBUG
							aginfo->strOrgUrl = strredirect;
						}
#ifdef QVODDEBUG
						_OUTPUT(INFO,"CHttpUpload recv 302 : %s\n", strredirect);
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

						char* pServer = strstr(strend, "Server: ");
						if (pServer)
						{
							pServer += strlen("Server: ");
							char* pend = strstr(pServer, "\r\n");
							if (pend)
							{
								aginfo->strErrBuf.append(pServer, pend-pServer);
							}
						}
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpUpload OnData 302\n");
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
		_OUTPUT(ERROR1, "CHttpUpload OnData !=200 recvbuf:%s\n", recvBuf);
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
		_OUTPUT(ERROR1, "CHttpUpload OnData pbuf \n");
#endif
			return -1;
		}
		pbuf += 4;
		datalen -= (int)(pbuf - recvBuf);
        if (0 == aginfo->endpos)
        {
            int iMaxDownloadTime = aginfo->wholeFilelen/16384;
            aginfo->iMaxDownloadTime = iMaxDownloadTime < 5?5:iMaxDownloadTime;
#ifdef QVODDEBUG
            _OUTPUT(INFO,"CHttpUpload http contentlen = %lld,change maxdowntime %d sec\n",aginfo->wholeFilelen,aginfo->iMaxDownloadTime);
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
	case CAgentInfo::_POST:
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
						_OUTPUT(INFO, "CHttpUpload::OnData IsChunkEnd recvsize=%d", aginfo->strDataBuf.length());
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
									_OUTPUT(INFO, "CHttpUpload::OnData unGzip failed!");
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
								_OUTPUT(INFO, "CHttpUpload::OnData unzip ok! tick=%d zipsize=%lld datasize=%lld", QvodGetTime()-dwTick, orgLen, aginfo->downloadlen);
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
					if (aginfo->type == CAgentInfo::_POST)
					{
						agentrange *range = new agentrange;
						range->head.len = sizeof(agentrange)-4;
						range->head.id = AGENTUPLOAD;
						range->result = 0;
						{
							CAutoLock lock(m_fileLock);		// 加锁访问
							range->start = aginfo->pos;
							range->buf = aginfo->pdatabuf;
							range->id = aginfo->id;
							aginfo->pdatabuf = NULL;	// released in msgpool
							range->len = aginfo->downloadlen;
							range->filesize = aginfo->wholeFilelen;
						}
						memcpy(range->hash,hash.data,20);
						if (! aginfo->strHash.empty())	// 子请求的hash替换成所属任务hash
						{
							memcpy(range->hash,aginfo->strHash.c_str(),20);
							DelOneTaskHash(HASH(aginfo->strHash.c_str()), hash);
						}
#ifdef QVODDEBUG
						_OUTPUT(INEED,"CHttpUpload::OnData finish!  push AGENTRANGE msg, start=%llu,len=%llu,type = %d\n",aginfo->pos,aginfo->downloadlen,aginfo->type);
#endif
						{
							CAutoLock lock(m_msgQueueLock);
							m_msgQueue.push((agentmsghead*)range);
						}
						if (aginfo->type == CAgentInfo::_RANGE)
						{
							aginfo->reuse = true;
						}
						else 
						{
							TerminalSock(hash, true, false, false);
						}
					}
				}
				return 1;
			}
			break;
		}
	default:
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CHttpUpload ******find a unknow agent %d , hash %s,pos %llu*******\n", aginfo->type, aginfo->strUrl.c_str(), aginfo->pos);
#endif
		break;
	}
	return (int)(pbuf - recvBuf);
}

int CHttpUpload::OnConnect(const qvod_sock_t& sock, const int& i)
{
	HASH sockHash;
	if (! GetSockBindHash(sock, sockHash))
	{
		return -1;
	}
	const HASH hash = sockHash;
	int sendlen = 0;
	char host[128] = {0};
	char buf[1024];
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
	{
		return -1;
	}

	aginfo->status=CAgentInfo::_RUNING;
	aginfo->connected = true;
	const char* url = aginfo->strUrl.c_str();
	char* pbuf = buf;
	int err = 0;
	switch (aginfo->type)
	{
	case CAgentInfo::_POST:
		{
			aginfo->last_recv_tick = QvodGetTime();
			if (0 == aginfo->sendlen)	// 发送请求头
			{		
				{
					AutoPtr<CHttpSock> pSock = NULL;
					if (! GetHttpSock(sock, &pSock))
					{
						return -1;
					}
					sendlen = pSock->SendData(aginfo->rawbuf,(int)strlen(aginfo->rawbuf));
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
			_OUTPUT(INFO, "CHttpUpload::OnConnect [%s] type=%d sendlen=%d info=%s\n", Hash2Char(hash.data).c_str(), aginfo->type, aginfo->sendlen, aginfo->rawbuf);
#endif
			}
			// 发送数据部分
			{
				const unsigned int nDataLen = aginfo->strSendDataBuf.length();
				if (nDataLen > aginfo->senddatalen)
				{
					{
						AutoPtr<CHttpSock> pSock = NULL;
						if (! GetHttpSock(sock, &pSock))
						{
							return -1;
						}
						sendlen = pSock->SendData(aginfo->strSendDataBuf.c_str()+aginfo->senddatalen,(int)(aginfo->strSendDataBuf.length()-aginfo->senddatalen));
					}
					aginfo->sendlen += sendlen;
					aginfo->senddatalen += sendlen;
					if (aginfo->senddatalen == aginfo->strSendDataBuf.length())		// 发送完毕
					{
						aginfo->strSendDataBuf.clear();		// 释放内存
					}

					CMsgPoolInterface::Instance()->InterUpSize(sendlen);
					
					HASH szTaskHash = hash;
					if (!aginfo->strHash.empty())	// 子请求的hash替换成所属任务hash
					{
						memcpy(szTaskHash.data, aginfo->strHash.c_str(), 20);
					}

					KEY key;
					CSpeedCounter::Instance()->PushSpeedData(&szTaskHash,NULL,sendlen,GlobalUpSpeedCallBack);
					SPEEDATA data;
					data.hash = szTaskHash;
					data.size = sendlen;
					CTaskMgrInterFace::Instance()->InterUpSize(data);	//把http速度算到任务的总速度里

#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpUpload::OnConnect [%s] type=%d senddatalen=%d \n", Hash2Char(hash.data).c_str(), aginfo->type, aginfo->senddatalen);
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

int CHttpUpload::OnConnectFail(const qvod_sock_t& sock)
{
	HASH sockHash;
	if (! GetSockBindHash(sock, sockHash))
	{
		return -1;
	}
	const HASH hash = sockHash;
	AutoPtr<CAgentInfo> aginfo;
	if(!GetAgentInfo(hash,&aginfo))
	{
		return -1;
	}
	aginfo->errcode = 11;
#ifdef QVODDEBUG
	char msg0[50]={0};
	memcpy(msg0,Hash2Char(hash.data).c_str(),40);
	_OUTPUT(ERROR1,"\nCHttpUpload::OnConnectFail ,%s\n",msg0);
#endif
	TerminalSock(hash, false, false, true);
	return 0;
}

int CHttpUpload::OnClose(const HASH& hash)
{
	qvod_sock_t sock = GetSocketByHash(hash);

	return OnClose(sock);
}

int CHttpUpload::OnClose(const qvod_sock_t& s)
{
	CAutoLock lock(m_nSockLock);
	//防止溢出
	//if(i>=m_nSocket || m_nSocket>=64)
	//	return -1;

	const int i = GetIndexHttpSock(s);
	if(i>=m_nSocket || m_nSocket>=64)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "OnClose fail! GetIndexHttpSock i=%d", i);
#endif
		return -1;
	}

	//加锁删除socket
	HASH hash;
	if (GetSockBindHash(s, hash))
	{
		AutoPtr<CAgentInfo> aginfo;
		if(GetAgentInfo(hash, &aginfo))
		{
			aginfo->reuse = false;
			aginfo->last_recv_tick = 0;
		}	
		RelaseSockHash(hash);
	}

#ifdef WIN32
	WSACloseEvent(m_EventArray[i]);
#endif

	if(m_nSocket>0)
	{
		SafeRelease(m_SocketArray[i]);
		m_nSocket -- ;
		//SafeDelete(m_SocketArray[i]);
		if (i != m_nSocket)
		{
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

int CHttpUpload::GetIndexHttpSock(const qvod_sock_t& sock)
{
	CAutoLock lock(m_nSockLock);
	for (int i=0; i<m_nSocket; i++)
	{
		if (m_SocketArray[i] && (m_SocketArray[i]->GetSocket() == sock))
		{
			return i;
		}
	}
	return -1;
}

int CHttpUpload::GetIndexHttpSock(const CHttpSock* pSock)
{
	CAutoLock lock(m_nSockLock);
	for (int i=0; i<m_nSocket; i++)
	{
		if (m_SocketArray[i] == pSock)
		{
			return i;
		}
	}
	return -1;
}

bool CHttpUpload::GetHttpSock(const qvod_sock_t& s, CHttpSock** pSock )
{
	if (NULL == pSock)
	{
		return false;
	}
	CAutoLock lock(m_nSockLock);
	for (int i=0; i<m_nSocket; i++)
	{
		if (m_SocketArray[i] && m_SocketArray[i]->GetSocket() == s)
		{
			*pSock = m_SocketArray[i];
			(*pSock)->duplicate();
			return true;
		}
	}
	return false;
}

bool CHttpUpload::GetHttpSockByHash(const HASH& hash, CHttpSock** pSock )
{
	if (NULL == pSock)
	{
		return false;
	}
	qvod_sock_t sock = GetSocketByHash(hash);
	if (QVOD_INVALID_SOCKET == sock)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "GetHttpSockByHash fail! sock=%d hash=%s", sock, Hash2Char(hash.data).c_str());
#endif
		return false;
	}

	return GetHttpSock(sock, pSock);
}

bool CHttpUpload::GetSockBindHash(const qvod_sock_t& s, HASH& hash)
{
	CAutoLock lock(m_nSockLock);
	AutoPtr<CHttpSock> pSock = NULL;
	if (GetHttpSock(s, &pSock))
	{
			PHttpAgentSockData pSockData  = dynamic_cast<PHttpAgentSockData>(pSock->GetData());
			if (pSockData)
			{
				hash = pSockData->hash;
				return true;
			}
	}

	return false;
}

bool CHttpUpload::RelaseSockHash(const HASH& hash)
{
	CAutoLock lock(m_agentLock);
	map<HASH, qvod_sock_t>::iterator itr = m_HashSockMap.find(hash);
	if (itr != m_HashSockMap.end())
	{
		m_HashSockMap.erase(itr);
		return true;
	}
	return false;
}

qvod_sock_t CHttpUpload::GetSocketByHash(const HASH& hash)
{
	CAutoLock lock(m_agentLock);
	map<HASH, qvod_sock_t>::iterator itr = m_HashSockMap.find(hash);
	if (itr != m_HashSockMap.end())
	{
		return itr->second;
	}
	return 0;
}

int CHttpUpload::CopyErrBuf(const HASH& hash,char* buf)
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

bool CHttpUpload::RecvPacket(char* buf,int& len)
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

unsigned int CHttpUpload::QueryFileSpeed(HASH& hash)
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

bool CHttpUpload::GetAgentInfo(const HASH& hash,CAgentInfo** agent)
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

void CHttpUpload::DelAgentInfo(const HASH& hash)
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
void CHttpUpload::ClearAgentInfoMap()
{
	CAutoLock lock(m_agentLock);
	map<HASH,CAgentInfo*>::iterator it = m_agentMap.begin();
	for (; it != m_agentMap.end(); it++)
	{
		it->second->release();
	}
	m_agentMap.clear();
}

bool CHttpUpload::TerminalSock(const qvod_sock_t& sock,bool initiative,bool ieclose,bool onConnectFailed)
{
	HASH sockHash;
	if (! GetSockBindHash(sock, sockHash))
	{
		return -1;
	}

	const HASH hash = sockHash;
	return TerminalSock(hash, initiative, ieclose, onConnectFailed);
}
bool CHttpUpload::TerminalSock(const HASH& hash,bool initiative,bool ieclose,bool onConnectFail)
{
	CAutoLock lock(m_agentLock);
	AutoPtr<CAgentInfo> pAgentInfo;
	if (GetAgentInfo(hash, &pAgentInfo))
	{
		HASH _hash = hash;
		if (pAgentInfo->type == CAgentInfo::_POST || pAgentInfo->type == CAgentInfo::_START ||
			CAgentInfo::_M3U8 == pAgentInfo->type || CAgentInfo::_M3U8TS == pAgentInfo->type)
		{
			//关闭连接
			OnClose(hash);
			if (!initiative && !ieclose && pAgentInfo->connectimes < 5 && pAgentInfo->strErrBuf.empty() && 0 ==pAgentInfo->downloadlen)
			{
				if ( 0 == ReConnect(hash))
				{
					pAgentInfo->connectimes++;
					if (pAgentInfo->connectimes >= 3)			// 重试超过3次则通知上层
					{
						CallBackToUI(5, pAgentInfo->connectimes, Hash2Char(hash.data).c_str(), 40);
					}
#ifdef QVODDEBUG
_OUTPUT(INFO, "CHttpUpload ReConnect connectimes=%d",pAgentInfo->connectimes++);
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
				case CAgentInfo::_POST:
					{							
						agentrange *range = new agentrange;
						range->head.len = sizeof(agentrange) - 4;
						range->head.id = AGENTUPLOAD;
                        range->buf = NULL;
                        range->len = 0;
						range->id = pAgentInfo->id;
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
						if (! pAgentInfo->strHash.empty())	// 子请求的hash替换成所属任务hash
						{
							memcpy(range->hash,pAgentInfo->strHash.c_str(),20);
							DelOneTaskHash(HASH(pAgentInfo->strHash.c_str()), hash);
						}
#ifdef QVODDEBUG
						_OUTPUT(INEED,"CHttpUpload push AGENTRANGE msg, start=%llu,len=%u\n",pAgentInfo->pos,pAgentInfo->downloadlen);
#endif
						pMsg = (char*)range;
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

void CHttpUpload::deleteUnusedSock()
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
		_OUTPUT(INFO, "CHttpAgent deleteunselected type=%d connected=%d lasttick=%lld downtime=%lld maxdowntime=%lld, timestamp=%lld reuse=%d downlen=%lld contentlen=%lld", aginfo->type, aginfo->connected, aginfo->last_recv_tick, aginfo->downloadtime, aginfo->iMaxDownloadTime, timestamp, aginfo->reuse, aginfo->downloadlen, aginfo->contentlen);
		_OUTPUT(INFO, "CHttpAgent deleteunselected type=%d connected=%d lasttick=%lu downtime=%lu maxdowntime=%u, timestamp=%lu reuse=%d downlen=%lld contentlen=%lld", aginfo->type, aginfo->connected, aginfo->last_recv_tick, aginfo->downloadtime, aginfo->iMaxDownloadTime, timestamp, aginfo->reuse, aginfo->downloadlen, aginfo->contentlen);

#endif
	if ((aginfo->type == CAgentInfo::_RANGE || aginfo->type == CAgentInfo::_START ||
                  CAgentInfo::_M3U8 == aginfo->type || CAgentInfo::_M3U8TS == aginfo->type) && aginfo->last_recv_tick)
		{
			//超过10秒没接收到数据或使用、下载总时间超过最大时间、5秒内没连上cdn，关掉连接
			if (((!aginfo->connected && (timestamp >= aginfo->downloadtime + g_cfg.m_uMaxConnectTime*1000)) && !aginfo->reuse))	// care of negative wrap
			{
				int type = 0;

				vDelHashs.push_back(hash);

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

QVOD_THREAD_RETURN CHttpUpload::Routine(void* param)
{

#ifdef WIN32
	CHttpUpload* pAgent = (CHttpUpload*)param;
	unsigned int index;
	WSANETWORKEVENTS NetWorkEvents;
	while (pAgent->m_status == CHttpUpload::RUNNING)
	{
		if(pAgent->m_status == CHttpUpload::STOP)//用户关闭线程
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
			
			const qvod_sock_t sock = pAgent->GetSockArray(i);
			HASH hash;
			pAgent->GetSockBindHash(i, hash);

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
					pAgent->TerminalSock(sock);
					continue;
				}
				else
				{
					int result = pAgent->OnData(sock);
					if(-1 == result)
					{
						pAgent->TerminalSock(sock);
						continue;
					}
				}
			}

			if(NetWorkEvents.lNetworkEvents & FD_CONNECT)
			{
				if (NetWorkEvents.iErrorCode[FD_CONNECT_BIT])
				{
					pAgent->OnConnectFail(sock);
				}
				else
				{
					pAgent->OnConnect(sock, i);
				}
			}

			if(NetWorkEvents.lNetworkEvents & FD_WRITE)
			{
				if (NetWorkEvents.iErrorCode[FD_WRITE_BIT])
				{
					AutoPtr<CAgentInfo> aginfo;
					if(pAgent->GetAgentInfo(hash,&aginfo))
					{
						aginfo->errcode = 12;
						aginfo->errcode |= (WSAGetLastError() << 16);
					}
					pAgent->TerminalSock(sock);
					continue;
				}
				else
				{
					int result = pAgent->OnConnect(sock, i);
					if(-1 == result)
					{
						pAgent->TerminalSock(sock);
						continue;
					}
				}
			}

			if (NetWorkEvents.lNetworkEvents & FD_CLOSE )
			{
				AutoPtr<CAgentInfo> aginfo;
				if(pAgent->GetAgentInfo(hash,&aginfo))
				{
					aginfo->errcode = 13;
				}
				pAgent->TerminalSock(sock);
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
	CHttpUpload* pAgent = (CHttpUpload*)param;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10 * 1000;

	while(pAgent->m_status == CHttpUpload::RUNNING) {

		if(pAgent->m_status == CHttpUpload::STOP)//用户关闭线程
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
				_OUTPUT(FAIL, "CHttpUpload poll fail %d and continue\n", errno);
#endif
				continue;
			}
#ifdef QVODDEBUG
			_OUTPUT(INEED, "CHttpUpload poll error %d\n", errno);
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
			const qvod_sock_t s = pAgent->GetSockArray(i);
			HASH hash;
			pAgent->GetSockBindHash(i, hash);

			if (pAgent->pfds[i].revents & POLLIN) 
			{
				if (pAgent->m_IfSockConnected[i] != 1) 
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpUpload not connect and recv event\n");
#endif
                    if (! pAgent->TerminalSock(s))
                    {
                        pAgent->OnClose(s);
#ifdef QVODDEBUG
                        _OUTPUT(INFO, "CHttpUpload TerminalSock failed m_IfSockConnected hash=%s\n", Hash2Char(hash.data).c_str());
#endif
                    }
					continue;
				}
				int result = pAgent->OnData(s);
				if(-1 == result)
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpUpload recv data failed -1\n");
#endif
					if (! pAgent->TerminalSock(s))
					{
						pAgent->OnClose(s);
#ifdef QVODDEBUG
						_OUTPUT(INFO, "CHttpUpload TerminalSock failed OnData hash=%s\n", Hash2Char(hash.data).c_str());
#endif
					}
					continue;
				}
			}
			if (pAgent->pfds[i].revents & POLLOUT)
			{
				int err = 0;
				qvod_socklen_t len = sizeof(err);
				//qvod_sock_t s = pAgent->GetSockArray(i);
				if(s <= 0) 
				{
					_OUTPUT(INFO, "CHttpAgent qvod_sock_t s = pAgent->GetSockArray(i) i=%d m_nSocket=%d", i, pAgent->m_nSocket);
					continue;
				}
				if(getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len) == 0) 
				{
					if(err == 0) 
					{
						if (pAgent->OnConnect(s) <= 0)
						{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpUpload OnConnect failed \n");
#endif
							if (! pAgent->TerminalSock(s))
							{
								pAgent->OnClose(s);
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpUpload TerminalSock failed OnConnect hash=%s\n", Hash2Char(hash.data).c_str());
#endif
							}
							continue;
						}
					} 
					else 
					{
						pAgent->OnConnectFail(s);
					}
					
				} 
				else 
				{
					pAgent->OnConnectFail(s);
				}
				continue;
			}
			if (pAgent->pfds[i].revents & (POLLERR|POLLHUP|POLLNVAL))
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CHttpUpload have error event %X, %d\n", pAgent->pfds[i].revents, errno);
#endif
				if (-1 == pAgent->OnConnectFail(s))
				{
					pAgent->OnClose(s);
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CHttpUpload TerminalSock failed error hash=%s\n", Hash2Char(hash.data).c_str());
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

int CHttpUpload::DeleteTask(const HASH &hash)
{
	return DelTask(hash);
}


int CHttpUpload::GetTaskInfo(const HASH &hash, struct taskinfo *info)
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


std::string CHttpUpload::GetHeadValue(const char* pStrInfo,  const char* pKey)
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

bool CHttpUpload::IsChunkEnd(const std::string& strBuf, QVOD_UINT64 length, QVOD_UINT64 &knownLen/*已知的chunk长度, 用于内存申请*/)
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
char* CHttpUpload::GetChunkData(char const *pChunkData, QVOD_UINT64 datalen, char *pBuff, QVOD_UINT64 bufflen, QVOD_UINT64 &usedBuffLen, QVOD_UINT64 &handledChunkDataLen)
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

int CHttpUpload::AddTask(const HASH& hash, const HttpPostReq& req)
{
	return AddPostTask(hash, req.strOrlurl, req.id, req.strHeadAdd, req.strSendData.c_str(), req.strSendData.length(), req);
}
int CHttpUpload::AddPostTask(const HASH &hash, const string &strUrl,  const int& id, const string& strHeadAdd, const char* pBuf, const unsigned int& len, const HttpPostReq& req)
{
#ifdef QVODDEBUG
	string szHash = Hash2Char(hash.data);
	_OUTPUT(INFO, "AddPostTask hash=%s", szHash.c_str());
#endif
	CAutoLock lock(m_agentLock);
	AutoPtr<CAgentInfo> aginfo;
	if(GetAgentInfo(hash,&aginfo))
	{
		TerminalSock(hash, true);
        QvodSleep(10);
	}
	//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
	if(m_nSocket >= 64)
	{
		return -1;
	}
    char cdnhost[256] = {0};
    const char* url = NULL;
	
	bool bHttps = strUrl.find("https://") != string::npos;
	if (! bHttps)
	{
		if (strUrl.find("http://") == string::npos)
		{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CHttpUpload::AddM3u8TsTask 非http https协议");
#endif
			return -1;		
		}
	}
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
    
	string strHash = Hash2Char(hash.data);
	aginfo = new CAgentInfo;
	aginfo->duplicate();
	aginfo->id = id;
	aginfo->pos = 0;
	aginfo->endpos = 0;
	aginfo->strOrgUrl = strUrl;
	aginfo->strHash = req.strHash;
	if (! aginfo->strHash.empty())
	{
		AddOneTaskHash(HASH(aginfo->strHash.c_str()), hash);	// 添加子请求hash与所属任务hash对应关系
	}
	if (url)
	{
		aginfo->strUrl = url;
	}
	
	if (aginfo->pos > 0 || aginfo->endpos > 0)
	{
		aginfo->bHaveRange = true;
	}
	else
	{
		aginfo->bHaveRange = false;
	}
	aginfo->iMaxDownloadTime = len/16384;
	if (aginfo->iMaxDownloadTime < 5)
	{
		aginfo->iMaxDownloadTime = 5;
	}
	aginfo->strHost = cdnhost;
    const char* pPos = strstr(url,"/");
    if (pPos)
    {
        url = pPos;
    }

	aginfo->strSendDataBuf.assign(pBuf, len);
	sprintf(aginfo->rawbuf, post_fmt_of_STANDARD, url, LIB_VERSION, cdnhost, len, strHeadAdd.c_str());

	qvod_sock_t s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
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
	int iSndBufLen = 2048*1024;		// 1M
	setsockopt(s,SOL_SOCKET,SO_SNDBUF,(const char*)&iSndBufLen,sizeof(int));
	
	unsigned int ip = 0;
	unsigned short port = bHttps? 443: 80;
	string strcdnhost = cdnhost;
	pPos = strstr(cdnhost, ":");
	if(pPos)
	{
		port = atoi(pPos+1);
		strcdnhost = strcdnhost.substr(0, pPos-cdnhost);
	}
	ip = GetHostByName(strcdnhost.c_str());
	port = htons(port);

	aginfo->ip=ip;
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
	WSAEventSelect(s,event,FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE);
#else
	pfds[m_nSocket].fd = s;
	pfds[m_nSocket].events |= (POLLOUT|POLLERR|POLLHUP|POLLNVAL);
	QvodSetNonblocking(s);
#endif

		aginfo->type = CAgentInfo::AgentType::_POST;
		SafeRelease(m_SocketArray[m_nSocket]);
		//SafeDelete(m_SocketArray[m_nSocket])
		m_SocketArray[m_nSocket] = pSock;
		m_HashSockMap[hash] = s;
		m_nSocket++;
	}

#ifdef QVODDEBUG
	string msg0=Hash2Char((unsigned char*)hash.data);
	_OUTPUT(INFO,"CHttpUpload::AddPostTask hash %s, url %s, refer %s\n",msg0.c_str(),aginfo->strUrl.c_str(), aginfo->referer);
#endif // QVODDEBUG

	qvod_sockaddr_t SvrAddr;
	SvrAddr.sin_addr.s_addr = ip;
	SvrAddr.sin_port = port;
	SvrAddr.sin_family = AF_INET;
	int r = pSock->Connect(SvrAddr);
	int netErr = QvodNetGetLastError();
	if(r == QVOD_SOCKET_ERROR && netErr != QVOD_NET_EWOULDBLOCK &&
		netErr != EINPROGRESS)
	{
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "CHttpUpload AddPostTask connect %s failed %u\n", inet_ntoa(SvrAddr.sin_addr),errno);
#endif // QVODDEBUG
        TerminalSock(hash, true);
		return -1;
	}

#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"CHttpUpload::AddPostTask Connect  %s,%s,sock %d,ip %s \n",msg0.c_str(),aginfo->strUrl.c_str(), (qvod_sock_t)s,inet_ntoa(SvrAddr.sin_addr));
#endif
	aginfo->last_recv_tick = QvodGetTime();	// record the beginning tick
	aginfo->downloadtime = QvodGetTime();
	aginfo->initime = QvodGetTime();
	return 1;
}

// GZIP解压缩实现 edit by 魏理坚 2016/12/1
bool CHttpUpload::UnGzip(const char* buf, const int &len, string& strBuf)
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


void CHttpUpload::AddOneTaskHash(const HASH& taskHash, const HASH& childHash)
{
	CAutoLock lock(m_hashonetaskLock);
	m_mOneTaskHashs[taskHash].insert(childHash);
}

void CHttpUpload::DelOneTaskHash(const HASH& taskHash, const HASH& childHash)
{
	CAutoLock lock(m_hashonetaskLock);
	std::map<HASH, set<HASH> >::iterator itr=m_mOneTaskHashs.find(taskHash);
	if (itr != m_mOneTaskHashs.end())
	{
		itr->second.erase(childHash);
	}	
}

void CHttpUpload::DelOneTaskSock(const HASH& taskHash)
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

bool CHttpUpload::FindOneTaskHash(const HASH& taskHash, const string& filename, HASH& outHash)
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
