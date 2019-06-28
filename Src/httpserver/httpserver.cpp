#include "qvod_core.h"
#include "common-utility.h"
#include "TaskMgrInterface.h"
#include "common-commonfun.h"
#include "sha1.h"
#include "httpserver.h"
#include "trackerinterface.h"
#include "global.h"
#include "HttpAgentInterface.h"
#include "common-SpeedCounter.h"
#include "common-callback.h"
#include "qvod_sock.h"
#include "ServerProduct.h"
#include "CacheDataInMem.h"

qvod_thread_event_t		g_GetUrlEvent;
pthread_mutex_t			g_GetUrlEventLock;
extern CHttpServerMgr	g_httpServer;
extern IYfNetPlush*     g_pIYfNetPlush;

bool GetHttpUrl(const char* url,const char* savefilename);

QVOD_THREAD_RETURN GetHttpUrl(void* urlname)
{
	char url[1024]={0};
	char savefilename[1024]={0};
	char* purl = (char*)urlname;
	int n = strlen(purl);
	
	if(n >= 1024)
		return (QVOD_THREAD_RETURN)0;
	
	strcpy(url,purl);
	purl += n+1;
	n = strlen(purl);
	
	if(n >= 1024)
		return (QVOD_THREAD_RETURN)0;
	
	strcpy(savefilename,purl);
	
	QvodSetThreadEvent(&g_GetUrlEvent, &g_GetUrlEventLock);
	GetHttpUrl(url,savefilename);

	return (QVOD_THREAD_RETURN)0;
}


CHttpServer::CHttpServer()
:m_interval(125)
{
	m_nsocket = 0;
	m_sended = 0;
	for(int i=0;i<64;i++)
	{
		m_TimeArray[i] = 0;
		m_FileposArray[i] = 0;
		m_FileEndPosArray[i] = 0;
		m_pushedLen[i] = 0;
		m_NextPushTimeArray[i] = 0;
	}

#ifndef WIN32
	/* init pfds */
	int i;
	for(i = 0; i < 64; ++i)
	{
		pfds[i].fd = QVOD_INVALID_SOCKET;
		pfds[i].events = 0;
		pfds[i].revents = 0;
	}
#endif

	m_listensock = QVOD_INVALID_SOCKET;
	m_uLocalIP = inet_addr("127.0.0.1");
}

CHttpServer::~CHttpServer()
{
	QvodDestroyThreadEvent(&m_hTerminalEvent, &m_hTerminalEventLock);
}

int CHttpServer::run()
{
	m_tickecount = QvodGetTime();
	QvodCreateThread(NULL, (qvod_routine_pt)CHttpServer::Routine, this);
	QvodCreateThreadEvent(&m_hTerminalEvent, &m_hTerminalEventLock);
	return 0;
}

int CHttpServer::stop()
{
	//Ω· ¯Ã◊Ω”◊÷œﬂ≥Ã
	g_bRunning = false;
#ifdef QVODDEBUG
	_OUTPUT(INEED, "CHttpServer::stop()\n");
#endif
	QvodWaitSingleThreadEvent(&m_hTerminalEvent, 100, &m_hTerminalEventLock);

	for(unsigned int i = 0;i< m_nsocket;i++)
	{
		QvodCloseSocket(m_SocketArray[i]);
	}

	/* reset listen socket */
	m_listensock = QVOD_INVALID_SOCKET;

	return 0;
}

bool CHttpServer::OnTimer()
{
	OnTimerEvent();
	return true;
}

bool CHttpServer::OnTimerEvent()
{
	CAutoLock lock(m_lock);

	if(m_nsocket < 1)
		return true;

	int nlen = 0;
	int errlen = 0;
	QVOD_DWORD curTime = QvodGetTime();
	unsigned int i;

	for(i = 0;i<m_nsocket;i++)
	{
		//ºÏ≤È≥¨ ±
 		if(m_FileposArray[i] == 0xFFFFFFFF)//√ª”–∑¢ÀÕπ˝«Î«Û,≤ª∏¯∑¢ ˝æ›
		{
 			continue;
		}
		//ºÏ≤È¥ÌŒÛ

// 		errlen = CHttpAgentInterface::Instance()->CopyErrBuf(m_TaskUrlArray[i],errbuf);
// 		if(errlen > 0)
// 		{
// #ifdef QVODDEBUG
// 			_OUTPUT(ERROR1,"\nHttpAgent return error msg: %s\n",errbuf);
// #endif // QVODDEBUG
// 			send(m_SocketArray[i],errbuf,errlen,0);
//  			if(InnerRemoveSocket(m_SocketArray[i]))
//  			{
// 				i--;
// 			}
// 			continue;
// 		}

		if (! CTaskMgrInterFace::Instance()->TaskIsExist(m_TaskArray[i]))		// 任务不存在 删除socket
		{
			if (InnerRemoveSocket(m_SocketArray[i]))
			{
				i--;
			}
			break;
		}

		int iTaskType = CTaskMgrInterFace::Instance()->GetTaskType(m_TaskArray[i]);
        string strFilename = m_strURI[i];
		
		if (HLSDOWNTASK == iTaskType ||
			(HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP))
		{
			int iPos = m_strURI[i].rfind("/");
			if (iPos)
			{
				++iPos;
				strFilename = m_strURI[i].substr(iPos);
			}
		}

		if (!m_bHttpReply[i])
		{
			//¥¥Ω®»ŒŒÒ ±£¨µΩ’‚¿Ô”¶∏√ ’µΩ÷÷◊”¡À£¨‘Ÿªÿ”¶
			QVOD_UINT64 filelen = 0;
			if (HLSDOWNTASK == iTaskType || 
				(HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP))
			{
				filelen = CTaskMgrInterFace::Instance()->GetM3u8TsSize(m_TaskArray[i],strFilename);
				bool bm3u8 = strFilename.find(".m3u8") != string::npos;
				if (0 == filelen && !bm3u8)	// m3u8文件不从HttpAgent里获取数据，否则TaskMgr里面签名后tsurl变化导致播放不了
				{
					filelen = CHttpAgentInterface::Instance()->GetM3u8TsSize(m_TaskArray[i],strFilename);
				}
                if (0 == filelen)
                {
					filelen = CCacheDataInMem::Instance().GetM3u8TsSize(m_TaskArray[i], bm3u8? "playlist.m3u8": strFilename);
                }

				if (0 == filelen)
                {
					// edit by wlj,下载出错信息不通知给播放引擎，方便客户端处理重试逻辑
//					string strErrorInfo;
//					if (CHttpAgentInterface::Instance()->GetM3u8DownError(m_TaskArray[i],  strFilename, strErrorInfo))
//					{
//						// 下载出错了，推流给播放引擎
//						//char szError[200];
//						//sprintf(szError, "HTTP/1.1 %d Not Found\r\n\r\n", err);
//						send(m_SocketArray[i], strErrorInfo.c_str(), strErrorInfo.length(), 0);
//						m_bHttpReply[i] = true;
//#ifdef QVODDEBUG
//						_OUTPUT(INFO, "CHttpServer send head filename=%s\n----------\n%s\n----------\n", strFilename.c_str(), strErrorInfo.c_str());
//#endif // QVODDEBUG
//						if (InnerRemoveSocket(m_SocketArray[i]))
//						{
//							i--;
//						}
//						
//						break;
//					}
					//CTaskMgrInterFace::Instance()->SetTaskStatus(m_TaskArray[i], CTask::RUNNING);
                    continue;
				}
			}
			else
			{
				filelen = CTaskMgrInterFace::Instance()->GetFileSize(m_TaskArray[i]);
				if ( 0 == filelen )
				{
					filelen = CHttpAgentInterface::Instance()->GetFileSize(m_TaskUrlArray[i]);
					if (0 == filelen)
					{
						continue;
					}
				}	
			}

			if (0 == m_FileEndPosArray[i])
			{
				m_FileEndPosArray[i] = filelen - 1;
			}
			string strSuffix = GetPathSuffix(strFilename.c_str());
			string strContentType = "application/octet-stream";
			if (strSuffix == ".mp4")
			{
				strContentType = "video/mp4";
			}
			char httpHeadBuf[1024] = {0};
			QVOD_UINT64 iContentLength = m_FileEndPosArray[i] >= m_FileposArray[i]?m_FileEndPosArray[i]-m_FileposArray[i]+1:0;			
			if (m_bHttpRange[i])
			{
				sprintf(httpHeadBuf,"HTTP/1.1 206 Partial Content\r\nContent-Type: %s\r\nAccept-Ranges: bytes\r\nContent-Range: bytes %llu-%llu/%llu\r\nContent-Length: %llu\r\nServer: Yunfan Ios\r\nConnection: keep-alive\r\n\r\n", strContentType.c_str(), m_FileposArray[i], m_FileEndPosArray[i], filelen,iContentLength);
			}
			else
			{
				sprintf(httpHeadBuf,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nAccept-Ranges: bytes\r\nContent-Length: %lld\r\nServer: Yunfan Ios\r\nConnection: keep-alive\r\n\r\n", strContentType.c_str(), filelen);
			}

			send(m_SocketArray[i],httpHeadBuf,strlen(httpHeadBuf),0);

			m_bHttpReply[i] = true;

#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpServer send head filename=%s\n----------\n%s\n----------\n", strFilename.c_str(), httpHeadBuf);
#endif // QVODDEBUG

		}

		int iFileVideoRate = CTaskMgrInterFace::Instance()->GetPlayFileRate(m_TaskArray[i]);
		if (iFileVideoRate < 100000)
		{
			//小于100Kbps默认1Mbps
			iFileVideoRate = 1048576;
		}
		
		float fPercent = 1.5f;
		if (HLSP2PDOWNTASK == iTaskType)		// HLSP2PDOWNTASK任务以2.5速度推送
		{
			fPercent = 2.5f;
			if  (CTaskMgrInterFace::Instance()->TaskIsFinish(m_TaskArray[i]))
			{
				fPercent = 20.0f;
			}
		}

		iFileVideoRate = (int)(iFileVideoRate*fPercent);
		iFileVideoRate /= 8;
		bool bSendSuccess = false;
		
		do 
		{
			bSendSuccess = false;
			if (m_FileEndPosArray[i] < m_FileposArray[i])
			{
				break;
			}

			QVOD_INT64 readlen = m_FileEndPosArray[i] - m_FileposArray[i] + 1;
			readlen = HTTP_BUFFER_LENGTH > readlen ? readlen:HTTP_BUFFER_LENGTH;
			if(readlen <= 0)
			{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpServer socket %d need readlen %d <=0, endpos %llu, filepos %llu\n", m_SocketArray[i], readlen, m_FileEndPosArray[i], m_FileposArray[i]);
#endif
				break;
			}
			
			if (m_sendlen[i] == m_dataLen[i])
			{
				int iNoLimitSec = 15;
				if (HLSDOWNTASK == iTaskType || 
					(HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP))
				{
					//hls的推送前两个ts不限速，后面连续的ts限速推送
					iNoLimitSec = 1;
				}
				//按照码率推送数据，前15秒的数据不限制，超过15秒的数据以每秒1.5倍码率的量推送
				bool bSend = ((HLSDOWNTASK == iTaskType || (HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP)) && !CTaskMgrInterFace::Instance()->IsPlayTsContinue(m_TaskArray[i])) || 
				iFileVideoRate*iNoLimitSec > m_pushedLen[i] || 
				QvodGetTime() > m_NextPushTimeArray[i];

				if (! bSend)		// 限速此次不推送 不用读取数据
				{
					break;
				}

				if (HLSDOWNTASK == iTaskType || (HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP))
				{
					nlen = CTaskMgrInterFace::Instance()->ReadM3u8Ts(m_TaskArray[i],strFilename,m_databuf[i],m_FileposArray[i],readlen,false);
					//超过文件大小,推送完成退出
					if(nlen == -2)
					{
#ifdef QVODDEBUG
						_OUTPUT(FAIL, "**********CTaskMgr readpiece fail nlen -2 and remove sock************\n");
#endif // QVODDEBUG
						if (InnerRemoveSocket(m_SocketArray[i]))
						{
							i--;
						}
						break;
					}

					if(nlen <= 0)
					{
						bool bM3u8 = strFilename.find(".m3u8") != string::npos;
						if (! bM3u8)
						{
							nlen = CHttpAgentInterface::Instance()->ReadM3u8Ts(m_TaskArray[i],strFilename,m_databuf[i],readlen,m_FileposArray[i]);
							//超过文件大小,推送完成退出
							if(nlen == -2)
							{
								if (InnerRemoveSocket(m_SocketArray[i]))
								{
									i--;
								}
								break;
							}
						}
						if(nlen <= 0)
						{
							//从内存模块中读取数据
#ifdef QVOD_USE_CACHEMEM
							nlen = CCacheDataInMem::Instance().ReadM3u8Data(m_TaskArray[i],bM3u8?"playlist.m3u8":strFilename,m_databuf[i],m_FileposArray[i],readlen);
							if (nlen <= 0)
							{
								break;
							}
#endif
						}
					}
					else
					{
#ifdef QVODDEBUG
						//_OUTPUT(INFO,"DEFINE QVODDEBUG! Socket %d CTaskMgrInterFace start:%llu, len: %d\n", m_SocketArray[i], m_FileposArray[i],nlen);
#endif
					}
				}
				else
				{
					CTaskMgrInterFace::Instance()->SetPlayFilePos(m_TaskArray[i],m_FileposArray[i]);
					
					nlen = CTaskMgrInterFace::Instance()->ReadPiece(m_TaskArray[i],m_databuf[i],readlen,m_FileposArray[i],false);
					
					//超过文件大小,推送完成退出
					if(nlen == -2)
					{
#ifdef QVODDEBUG
						_OUTPUT(FAIL, "**********CTaskMgr readpiece fail nlen -2 and remove sock************\n");
#endif // QVODDEBUG
						if (InnerRemoveSocket(m_SocketArray[i]))
						{
							i--;
						}
						break;
					}

					if(nlen <= 0)
					{
						nlen = CHttpAgentInterface::Instance()->ReadPiece(m_TaskUrlArray[i],m_databuf[i],readlen,m_FileposArray[i]);
						
						//超过文件大小,推送完成退出
						if(nlen == -2)
						{
							if (InnerRemoveSocket(m_SocketArray[i]))
							{
								i--;
							}
							break;
						}

						if(nlen <= 0)
						{
							break;
						}
#ifdef QVODDEBUG
						_OUTPUT(INFO,"CHttpServer Socket %d CHttpAgentInterface start:%llu , len: %d\n", m_SocketArray[i], m_FileposArray[i],nlen);
#endif // QVODDEBUG
					}
					else
					{
#ifdef QVODDEBUG
						//_OUTPUT(INFO,"DEFINE QVODDEBUG! Socket %d CTaskMgrInterFace start:%llu, len: %d\n", m_SocketArray[i], m_FileposArray[i],nlen);
#endif
					}
				}	

				if (nlen > 0)
				{
					m_dataLen[i] = nlen;
					m_sendlen[i] = 0;
				}
			}
           
			bool connClosed = false;
			int sendlen = 0;
            int iNoLimitSec = 15;

			if (HLSDOWNTASK == iTaskType || (HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP))
			{
				//hls的推送前两个ts不限速，后面连续的ts限速推送
				iNoLimitSec = 1;
			}

			//按照码率推送数据，前15秒的数据不限制，超过15秒的数据以每秒1.5倍码率的量推送
			while(m_sendlen[i] < m_dataLen[i] && 
				(((HLSDOWNTASK == iTaskType || (HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP)) && !CTaskMgrInterFace::Instance()->IsPlayTsContinue(m_TaskArray[i])) || 
				iFileVideoRate*iNoLimitSec > m_pushedLen[i] || 
				QvodGetTime() > m_NextPushTimeArray[i]))
			{
				int n = send(m_SocketArray[i],m_databuf[i]+m_sendlen[i],m_dataLen[i]-m_sendlen[i],0);
//#ifdef QVODDEBUG
//				int nPos = strFilename.find("?");
//				string strFile = "tmp";
//				if (nPos != string::npos)
//				{
//					strFile = strFilename.substr(0, nPos);
//				}
//				_OUTPUT(INFO, "CHttpServer send data: filename=%s start=%lld len=%d", strFile.c_str(), m_FileposArray[i], n);
//				if (n > 0)
//				{
//					string strPath = "F:/server/";
//					strPath += strFile;
//					FILE *fp = fopen(strPath.c_str(), "ab+");
//					if (fp)
//					{
//						fseek(fp, m_FileposArray[i], SEEK_SET);
//						fwrite((void*)(m_databuf[i]+m_sendlen[i]), 1, n, fp);
//						fclose(fp);
//					}	
//				}
//#endif // QVODDEBUG
				if(n == QVOD_SOCKET_ERROR )
				{
					int err = QvodNetGetLastError();
#ifdef QVODDEBUG
					//_OUTPUT(INFO, "CHttpServer send error: socket %d err=%d\n", m_SocketArray[i], err);
#endif // QVODDEBUG
					if(err != QVOD_NET_EWOULDBLOCK)	// 无法立即完成一个非阻塞性套接字操作。 
					{
#ifdef QVODDEBUG
						_OUTPUT(INFO, "CHttpServer %s(%d)-%s: socket %d send error %d\n", __FILE__, __LINE__, __FUNCTION__, m_SocketArray[i], errno);
#endif // QVODDEBUG
						if (InnerRemoveSocket(m_SocketArray[i]))//连接异常,主动关闭
						{
							i--;
							connClosed = true;
						}
					}
					break;
				}
				else if (n == 0)
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpServer %s(%d)-%s: socket %d send len == 0\n", __FILE__, __LINE__, __FUNCTION__, m_SocketArray[i]);
#endif // QVODDEBUG
					if (InnerRemoveSocket(m_SocketArray[i]))//连接异常,主动关闭
					{
						i--;
						connClosed = true;
					}
					break;
				}      

				sendlen += n;
				m_sended += n;
				m_sendlen[i] += n;
				m_FileposArray[i] += n;
				m_TimeArray[i] = QvodGetTime();
				m_pushedLen[i] += n;
				m_NextPushTimeArray[i] = QvodGetTime() + n*1000/iFileVideoRate;

#ifdef QVODDEBUG
				if (!((HLSDOWNTASK == iTaskType || (HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP)) && !CTaskMgrInterFace::Instance()->IsPlayTsContinue(m_TaskArray[i]))
					&& iFileVideoRate*iNoLimitSec < m_pushedLen[i] && n*1000/iFileVideoRate > 500)
				{
					_OUTPUT(INFO,"CHttpServer %s(%d)-%s: interval time = %d ms,sendlen = %d\n", __FILE__, __LINE__, __FUNCTION__,
						n*1000/iFileVideoRate,n);
				}
#endif
			}

			if(sendlen > 0)
			{
				g_httpServer.InterPushSize(sendlen);
				CSpeedCounter::Instance()->PushSpeedData(NULL,NULL,sendlen,HttpServerPushCallBack);
				bSendSuccess = true;
			}

			if (connClosed)
			{
				break;
			}

			if (m_FileposArray[i] >= m_FileEndPosArray[i] && !m_keepalive[i])
			{
				if (InnerRemoveSocket(m_SocketArray[i]))
				{
					i--;
				}
				break;
			}
		} while (bSendSuccess);
		
	}

	return true;
}

bool CHttpServer::ReInitListen(unsigned short port)
{
	if (QVOD_INVALID_SOCKET != m_listensock) 
	{
        CAutoLock lock(m_lock);

        int err = 0;
        qvod_socklen_t len = sizeof(err);
        if(getsockopt(m_listensock, SOL_SOCKET, SO_ERROR, (char*)&err, &len) == 0) 
        {
            if(err == 0) 
            {
 #ifdef QVODDEBUG
                 _OUTPUT(INFO, "httpserver listen socket no error. No need reinit.\n");
 #endif
                return true;
            }
            else
            {
#ifdef QVODDEBUG
                _OUTPUT(FAIL, "httpserver listen socket error %d, m_listensock %d\n", err, m_listensock);
#endif
            }
        }

		InnerRemoveSocket(m_listensock);
		m_listensock = QVOD_INVALID_SOCKET;
        for (int i = 0; i < m_nsocket; i++) 
        {
            if(InnerRemoveSocket(m_SocketArray[i]))
            {
                i--;
            }
        }

		if(InitListen(port))
		{
			return true;
		}

#ifdef QVODDEBUG
		_OUTPUT(INEED, "re-init httpserver failed\n");
#endif
	}
	return false;
}

bool CHttpServer::InitListen(unsigned short port)
{
	qvod_sock_t s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(QVOD_INVALID_SOCKET == s)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "httpserver init create listen socket fail %d\n", errno);
#endif
		return false;
	}

	qvod_sockaddr_t addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;

	addr.sin_addr.s_addr = QVOD_INADDR_ANY;

	/* reuse addr */
	long reuseaddr = 1;
	int res = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr, sizeof(reuseaddr));
	
	if(res != 0) 
	{
		/* error */
		return false;
	}

	do 
	{
		addr.sin_port = htons(g_httpport);

		if(0 != ::bind(s,(sockaddr*)&addr,sizeof(addr)))
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "Httpserver bind port %d fail %d\n", g_httpport, errno);
#endif
			//换个端口，至少10000及以上端口
			g_httpport = ++g_httpport < 10000?10000:g_httpport;

			continue;
		}
		else
		{
			break;
		}
	} 
	while (true);

	CallBackToUI(4,g_httpport,NULL,0);

    CAutoLock lock(m_lock);
#ifdef QVODDEBUG
	_OUTPUT(INFO, "+ bind port %d success +, listen sock %d, i %d\n", g_httpport, s, m_nsocket);
#endif

	listen(s,5);

#ifdef WIN32
	WSAEVENT event = WSACreateEvent();
	m_EventArray[m_nsocket] = event;	
	WSAEventSelect(s,event,FD_ACCEPT);
#else
	/* add listen socket to event */
	QvodSetNonblocking(s);
	pfds[m_nsocket].fd = s;
	pfds[m_nsocket].events = POLLIN;
#endif
	/* add listen socket to the array, and then set m_listensock */
	m_SocketArray[m_nsocket] = s;
	memset(&m_TaskArray[m_nsocket],0,sizeof(m_TaskArray[m_nsocket]));
	memset(&m_TaskUrlArray[m_nsocket],0,sizeof(m_TaskUrlArray[m_nsocket]));
	m_TimeArray[m_nsocket] = 0;
	m_pushedLen[m_nsocket] = 0;
	m_NextPushTimeArray[m_nsocket] = 0;
	m_FileposArray[m_nsocket] = 0xFFFFFFFF;

	/* set value of m_listensock */
	m_listensock = s;
	m_nsocket++;

	return true;
}

bool CHttpServer::InnerAddSocket(qvod_sock_t s,QVOD_UINT ip)
{
	CAutoLock lock(m_lock);

	if(m_nsocket >= 64)
	{
		return false;
	}

	int sendbuf = 1024*1024;		// 设置发送缓冲区为1M
	int b = setsockopt(s,SOL_SOCKET,SO_SNDBUF,(char*)&sendbuf,sizeof(int));

#ifdef WIN32
	WSAEVENT event = WSACreateEvent(); 
	if (WSA_INVALID_EVENT == event )
	{
		return false;
	}
	m_EventArray[m_nsocket] = event;
	WSAEventSelect(s,event,FD_READ|FD_CLOSE);

#else
#ifdef MACOSX
	int opt = SO_NOSIGPIPE;
#else
    int opt = MSG_NOSIGNAL;
#endif
 	socklen_t optval = 1;
	setsockopt(s, SOL_SOCKET, opt, (char*)&optval, sizeof(socklen_t) );

	/* add socket to event */
	QvodSetNonblocking(s);
	pfds[m_nsocket].fd = s;
	pfds[m_nsocket].events |= (POLLIN | POLLERR | POLLHUP | POLLNVAL);
#endif

	m_SocketArray[m_nsocket] = s;
	m_SocketIP[m_nsocket] = ip;
	m_FileposArray[m_nsocket] = 0xFFFFFFFF;
	m_FileEndPosArray[m_nsocket] = 0;
	m_dataLen[m_nsocket] = 0;
	m_sendlen[m_nsocket] = 0;
	m_keepalive[m_nsocket] = false;
	memset(&m_TaskArray[m_nsocket],0,sizeof(m_TaskArray[m_nsocket]));
	memset(&m_TaskUrlArray[m_nsocket],0,sizeof(m_TaskUrlArray[m_nsocket]));
	m_TimeArray[m_nsocket] = QvodGetTime();
	m_pushedLen[m_nsocket] = 0;
	m_NextPushTimeArray[m_nsocket] = 0;

	m_nsocket++;

	return true;		
}

bool CHttpServer::InnerRemoveSocket(qvod_sock_t s)
{
	CAutoLock lock(m_lock);

	QVOD_UINT i;

	if(s == m_listensock)
	{
#ifdef QVODDEBUG
		_OUTPUT(INEED, "remove listen socket %d\n", s);
#endif
	}

	for (i = 0;i<m_nsocket;i++)
	{
		if (m_SocketArray[i] == s)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpServer remove socket %d, i %d\n", s, i);
#endif
			int iTaskType = CTaskMgrInterFace::Instance()->GetTaskType(m_TaskArray[i]);
			if ( (HLSDOWNTASK == iTaskType || (HLSP2PDOWNTASK == iTaskType && m_SocketIP[i]==m_uLocalIP)))
			{
				if (m_FileposArray[i] < m_FileEndPosArray[i])
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO,"CHttpServer httpserver push data not finish,%lld : %lld\n",m_FileposArray[i],m_FileEndPosArray[i]);
#endif
					//m3u8任务，ts未推送完就断开了，认为是seek，清空记录播放ts的队列
					CTaskMgrInterFace::Instance()->ClearContinuePlayTs(m_TaskArray[i]);
				}
				string strTsName = m_strURI[i];
				int iPos = strTsName.rfind("/");
				if (iPos)
				{
					++iPos;
					strTsName = strTsName.substr(iPos);
				}
				CTaskMgrInterFace::Instance()->ClosePush(m_TaskArray[i], strTsName);
			}

			QvodCloseSocket(s);
			m_SocketArray[i] = m_SocketArray[--m_nsocket];

#ifdef WIN32
			WSACloseEvent(m_EventArray[i]);
			m_EventArray[i] = m_EventArray[m_nsocket];
#else
			pfds[i] = pfds[m_nsocket];
			/* reset pfds[m_nsocket] */
			pfds[m_nsocket].fd = -1;
			pfds[m_nsocket].events = 0;
			pfds[m_nsocket].revents = 0;
#endif
			m_SocketIP[i] = m_SocketIP[m_nsocket];
			m_FileposArray[i] = m_FileposArray[m_nsocket];
			m_TaskArray[i] = m_TaskArray[m_nsocket];
			m_TaskUrlArray[i] = m_TaskUrlArray[m_nsocket];
			m_TimeArray[i] = m_TimeArray[m_nsocket];
			m_FileEndPosArray[i] = m_FileEndPosArray[m_nsocket];
			m_dataLen[i] = m_dataLen[m_nsocket];
			m_sendlen[i] = m_sendlen[m_nsocket];
			m_keepalive[i] = m_keepalive[m_nsocket];
			m_pushedLen[i] = m_pushedLen[m_nsocket];
			m_NextPushTimeArray[i] = m_NextPushTimeArray[m_nsocket];
			m_bHttpReply[i] = m_bHttpReply[m_nsocket];
			m_bHttpRange[i] = m_bHttpRange[m_nsocket];
			m_strURI[i] = m_strURI[m_nsocket];

			if (m_dataLen[i]>m_sendlen[i])
			{
				memcpy(m_databuf[i]+m_sendlen[i], m_databuf[m_nsocket]+m_sendlen[i], m_dataLen[i]-m_sendlen[i]);
			}
			
			return true;
		}
	}
	return false;
}

bool CHttpServer::GetHttpUrlFromBuffer(char* buf,char* url,int urllen)
{
	char* start = NULL;
	char* end = NULL;

	start = strstr(buf,"GET ");
	if (start)
	{
		start += 4;
	}
	else 
	{
		start = strstr(buf, "HEAD ");
		if (start)
		{
			start += strlen("HEAD ");
		}
	}
	if(start)
	{
		end = strstr(start," HTTP/1.");
		if(end)
		{
			int length = end - start;
			if(length < urllen)
			{
				memcpy(url,start,length);
				url[length] = '\0';

				return true;
			}
		}
	}

	return false;
}

int CHttpServer::GetStartFromUrl(char* url)
{
	char* pos = NULL;

	pos = strstr(url,"start=");

	if(pos)
	{
		return atoi(pos+6);
	}
	
	pos = strstr(url,"tflvbegin=");

	if(pos)
	{
		return atoi(pos+10);
	}

	return 0;
}

void CHttpServer::GetFileNameFromUrl(char* url,char* filename)
{
	char* pos = strrchr(url,'/');
	strcpy(filename,pos+1);

	pos = strstr(filename,"?");

	if(pos)
	{
		*pos = '\0';
	}
}

int CHttpServer::ProcessYfHttpReq( char* buf, int len, int i )
{
    //请求的格式为/yfhttpagent/40字节hash.格式名，因为ios播放器需要后缀名。。。
    //m3u8请求格式为/yfhttpagent/40字节hash/文件名
	std::string strError = "HTTP/1.1 404 Not Found\r\n\r\n";
	string strOrgUrl;
	char* pos = strstr(buf,"GET ");
	if (pos)
	{
		pos += strlen("GET ");
	}
	else 
	{
		pos = strstr(buf, "HEAD ");
		if (pos)
		{
			pos += strlen("HEAD ");
		}
	}
	if (pos)
	{
		char* endPos = strstr(pos,"\r\n");
		if (endPos)
		{
			*endPos = '\0';
			char* urlEndPos = strstr(pos," ");
			if (urlEndPos)
			{
				strOrgUrl = string(pos,urlEndPos-pos);
				strOrgUrl = URLDecode(strOrgUrl.c_str(), strOrgUrl.length());
			}
			*endPos = '\r';
		}
	}

    //判断是否云帆的请求及取出hash
    HASH hash;
    int iPos = strOrgUrl.find("/yfhttpagent/");
    bool bYunfanProxy = (string::npos != iPos);
	if (bYunfanProxy)
    {
		string strHash;
        iPos += strlen("/yfhttpagent/");
        if (strOrgUrl.size() >= iPos + 40)
        {
			strHash.assign(strOrgUrl.c_str()+iPos, 40);
            Char2Hash(strOrgUrl.c_str()+iPos, hash.data);
        }
		if (strOrgUrl.find(".m3u8") != string::npos)    // 启动下载任务
		{
			g_pIYfNetPlush->SetTaskPlaying(strHash.c_str(), true);		// 设置为播放状态
			//g_pIYfNetPlush->RunTask(strHash.c_str());
		}
    }
    else
    {
        int nPos = strOrgUrl.find(":80/");   //去掉默认80端口
        if (nPos != -1)
        {
            strOrgUrl.erase(nPos, 3);
        }
        bool bExist = CTaskMgrInterFace::Instance()->FindHashByUrl(strOrgUrl, hash);
        if (!bExist && g_pIYfNetPlush)
        {
            if (strOrgUrl.find(".m3u8") != string::npos)    // 启动下载任务
            {
                char szHash[41] = {0};
                char szHttpProxyUrl[200] = {0};
                if (E_NET_OK == g_pIYfNetPlush->CreateHlsTask(strOrgUrl.c_str(), strOrgUrl.c_str(), false, szHash, szHttpProxyUrl))
                {
                    Char2Hash(szHash, hash.data);
                    g_pIYfNetPlush->RunTask(szHash);
                }

            }
        }
		if (strOrgUrl.find(".m3u8") != string::npos)
		{
			g_pIYfNetPlush->SetTaskPlaying(Hash2Char(hash.data).c_str(), true);		// 设置为播放状态
			//CServerProduct::SetTaskStatus(hash,CTask::RUNNING);	
		}
    }

#ifdef QVODDEBUG
	struct	in_addr addr;
	addr.s_addr = m_SocketIP[i];
    _OUTPUT(INFO,"CHttpServer::ProcessYfHttpReq ip=%s url=%s [%s]\n", inet_ntoa(addr), strOrgUrl.c_str(), Hash2Char(hash.data).c_str());
#endif

    if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
    {
        send(m_SocketArray[i], strError.c_str(), strError.size(), 0);
		return 0;
    }

	//º«¬º∏√¡¨Ω”µƒ«Î«Û–≈œ¢
	m_bHttpReply[i] = false;
	m_bHttpRange[i] = false;
	m_TaskArray[i] = hash;
	m_TaskUrlArray[i] = hash;
	m_strURI[i] = strOrgUrl;
    m_FileposArray[i] = 0;
    m_FileEndPosArray[i] = 0;
	
	char* pPlayPos = strstr(buf,"Range: bytes=");
	
	if(pPlayPos)
	{
		pPlayPos += strlen("Range: bytes=");
		m_FileposArray[i] = QVOD_ATOI64(pPlayPos);
	}
	else
	{
		pPlayPos = strstr(buf,"Range:bytes=");
		if(pPlayPos)
		{
			pPlayPos += strlen("Range:bytes=");
			m_FileposArray[i] = QVOD_ATOI64(pPlayPos);
		}
		else 
		{
			m_FileposArray[i] = 0;
		}
	}

	if (pPlayPos)
	{
		m_bHttpRange[i] = true;
		char* pEndPos = strstr(pPlayPos, "\r\n");
		if (pEndPos)
		{
			char* pTmpEndPos = pEndPos;
			*pTmpEndPos = '\0';
			pEndPos = strstr(pPlayPos, "-");
			if (pEndPos)
			{
				pEndPos++;
				m_FileEndPosArray[i] = QVOD_ATOI64(pEndPos);
			}
			*pTmpEndPos = '\r';
		}
	}

    if (strstr(buf, "Connection: Keep-Alive"))
    {
        m_keepalive[i] = true;
    }

//#ifdef QVODDEBUG
//	_OUTPUT(INFO,"CHttpServer::ProcessYfHttpReq orgurl = %s,beginpos = %lld,endpos = %lld\n",strOrgUrl.c_str(),
//		m_FileposArray[i],m_FileEndPosArray[i]);
//#endif

	int type = CTaskMgrInterFace::Instance()->GetTaskType(hash);

	if (HLSDOWNTASK == type ||
		((HLSP2PDOWNTASK == type && m_SocketIP[i]==m_uLocalIP) && 0 == m_FileposArray[i]) )		// 判断HLSP2PDOWNTASK类型是否走ts片读取
	{
        string strTsName = strOrgUrl;

        int iPos = strOrgUrl.rfind("/");
        if (iPos)
        {
            ++iPos;
            strTsName = strOrgUrl.substr(iPos);
        }
		CTaskMgrInterFace::Instance()->SetM3u8PlayingPos(hash,strTsName,m_FileposArray[i]);
	}
	else
	{
		CTaskMgrInterFace::Instance()->SetPlayFilePos(hash,m_FileposArray[i]);
	}	

// 	CServerProduct::SetTaskStatus(hash,CTask::RUNNING);	
	//¥¥Ω®»ŒŒÒªÚseek¡À£¨∏¸–¬œ¬‘ÿƒ£ Ω£¨¬Ì…œœ¬‘ÿ–Ë“™µƒ ˝æ›
	CTaskMgrInterFace::Instance()->UpdateDownloadMode(hash);
	if (HLSDOWNTASK != type && !(HLSP2PDOWNTASK == type && m_SocketIP[i]==m_uLocalIP))
	{
		CTaskMgrInterFace::Instance()->DownEmergencyData(hash);	
	}

	return 1;
}

#ifdef WIN32
void CHttpServer::Routine(void* pvoid)
{
	CHttpServer* psvr = (CHttpServer*)pvoid;
	UINT index;
	WSANETWORKEVENTS NetWorkEvents;
	while (g_bRunning)
	{
		psvr->OnTimer();
		if(psvr->m_nsocket < 1)
		{
			Sleep(16);
			continue;
		}
		//开始处理事件
		WSAEVENT* pevent = psvr->m_EventArray;
		index = WSAWaitForMultipleEvents(psvr->m_nsocket,pevent,FALSE,2,FALSE);
		if (index == WSA_WAIT_FAILED )
		{
#ifdef QVODDEBUG
			_OUTPUT(FAIL,"CHttpServer::Routine WSAWaitForMultipleEvents WSA_WAIT_FAILED" ) ; 
#endif
			continue;
		}
		if(index == WSA_WAIT_TIMEOUT)
		{
			continue;
		}
		index = index - WSA_WAIT_EVENT_0;

		//遍历后面所有事件,看后面事件是否被传信
		for(UINT i = index;i<psvr->m_nsocket;i++)
		{
			index = WSAWaitForMultipleEvents(1,psvr->GetEventPtr(i),TRUE,0,FALSE);
			
			if (index == WSA_WAIT_FAILED || index ==WSA_WAIT_TIMEOUT)
			{
				continue;
			}

			ResetEvent(psvr->m_EventArray[i]);
			WSAEnumNetworkEvents(psvr->GetSocket(i),*psvr->GetEventPtr(i),&NetWorkEvents);
			
			//FD_READ
			if(NetWorkEvents.lNetworkEvents & FD_READ)
			{
				SOCKET s = psvr->GetSocket(i);
				if (NetWorkEvents.iErrorCode[FD_READ_BIT])
				{
					if (psvr->InnerRemoveSocket(s))
					{
						i--;
					}
					continue;
				}
				else
				{
					char buf[4096] = {0};

					int nlen = recv(s,buf,4095,0);

					do 
					{
						if(nlen <= 0)
						{
							break;
						}

						char *pFinishLine = strstr(buf, "\r\n\r\n") ;
						if ( pFinishLine )
						{
							break;
						}

						QVOD_SLEEP(10) ;
						int nRevlen = recv(s,buf+nlen,4095-nlen,0);

						if(nRevlen <= 0)
						{
							break;
						}

						nlen += nRevlen ;
					} 
					while (true);

					if(nlen <= 0)
					{
						if (psvr->InnerRemoveSocket(s))
						{
							i--;
						}
						continue;
					}

					buf[nlen] = '\0';

					int ilen = 0;
					char requrl[1024]={0};

					psvr->GetHttpUrlFromBuffer(buf,requrl,1024);
					ilen = psvr->ProcessYfHttpReq(buf,nlen,i);

					if(ilen == 0)
					{
						if (psvr->InnerRemoveSocket(s))
						{
							i--;
						}
						continue;
					}
				}
			}
			if(NetWorkEvents.lNetworkEvents & FD_ACCEPT)
			{
				SOCKET s = psvr->GetSocket(i);
				if (NetWorkEvents.iErrorCode[FD_ACCEPT_BIT])
				{
					if (psvr->InnerRemoveSocket(s))
					{
						i--;
					}
					continue;
				}
				else
				{
					SOCKADDR_IN addr;
					int len = sizeof(addr);
					SOCKET ns = accept(s,(sockaddr*)&addr,&len);
					if(ns == -1)  
					{
						if (psvr->InnerRemoveSocket(s))
						{
							i--;
						}
						continue;
					}
#ifdef SUPERNODE
					if(!g_IPRules.CanAccept(addr.sin_addr.S_un.S_addr))
					{
						closesocket(ns);
						continue;
					}
#endif
					if (!IsLANIP(addr.sin_addr.s_addr))
					{
						closesocket(ns);
						continue;
					}								
					if(!g_httpServer.InnerAddSocket(ns,addr.sin_addr.S_un.S_addr))
					{
						closesocket(ns);
					}
				}

			}
			//FD_CLOSE
			if(NetWorkEvents.lNetworkEvents & FD_CLOSE )
			{
				SOCKET s = psvr->GetSocket(i);
				if (psvr->InnerRemoveSocket(s))
				{
					i--;
				}
				continue;
			}
		}
	}

	//删除线程对象
	SetEvent(psvr->m_hTerminalEvent);
}
#else
QVOD_THREAD_RETURN CHttpServer::Routine(void* pvoid)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "thread CHttpServer::Routine start %d\n", pthread_self());
#endif // QVODDEBUG

	CHttpServer* psvr = (CHttpServer*)pvoid;
	if (NULL == psvr)
	{
		return  (QVOD_THREAD_RETURN)0;
	}
	while(g_bRunning)
	{
		psvr->OnTimer();

		int SocketNum = psvr->m_nsocket;
		if(SocketNum < 1)
		{
			QVOD_SLEEP(10);
			continue;
		}

		/* start poll */
		int result = poll(psvr->pfds, SocketNum, 20);

		if(result == -1)
		{
			/* error */

			/*
			 * if error occurs, poll will not wait,
			 * so we must sleep for decreasing cpu time.
			 */
			QVOD_SLEEP(10);

			qvod_err_t err = QvodGetLastError();
			if(err == EINTR) 
			{
				/* a singal was caught */
				continue;
			}

			if(err == EBADF) 
			{
				/* maybe already closed */
#ifdef QVODDEBUG
				_OUTPUT(ERROR1, "httpserver poll error, maybe some socket has closed\n");
#endif
				continue;
			}
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"httpserver poll error, and errno = %d\n", err);
#endif
			continue;
		}

		if(result == 0)
		{
			/* timeout */
			continue;
		}

		/* check socket event */
		int i;
		for(i = 0; i < psvr->m_nsocket; ++i)
		{

			qvod_sock_t s = psvr->GetSocket(i);
			
			//_OUTPUT(INFO, "HttpServer do poll sock %d, i %d, events %d, errno %d, m_socket %d\n", s, i, psvr->pfds[i].revents, errno, psvr->m_nsocket);

			if(psvr->pfds[i].revents & POLLIN)
			{
				/* socket have read event */
				if(s == psvr->m_listensock)
				{
					while(true)
					{
						/* is m_listensock */
						qvod_sockaddr_t addr;
						qvod_socklen_t len = sizeof(addr);
						qvod_sock_t ns = accept(s, (sockaddr*)&addr, &len);
#ifdef QVODDEBUG
						_OUTPUT(INFO, "------ httpserver accept new socket %d errno %d\n", ns, errno);
#endif
						if(ns == -1)
						{
							//_OUTPUT(INEED, "%s(%d)-%s: first accept error: %d, m_nsocket %d\n", __FILE__, __LINE__, __FUNCTION__, errno, psvr->m_nsocket);
							//psvr->InnerRemoveSocket(s);
							//i--;
							//continue;

							if(EAGAIN == errno || EMFILE == errno || errno == EINTR)//EMFILE:too many open files
							{
								/* recv a signal */
								//_OUTPUT(INEED, "%s(%d)-%s: first accept error: %d, m_nsocket %d, ns %d\n", __FILE__, __LINE__, __FUNCTION__, errno, psvr->m_nsocket, ns);
								break;
							}

							/* the listen socket is a blocking socket */
#ifdef QVODDEBUG
							_OUTPUT(INFO, "%s(%d)-%s: unknown accept error: %d, exit\n", __FILE__, __LINE__, __FUNCTION__, errno);
#endif
							break;
						}

						if(!g_httpServer.InnerAddSocket(ns,addr.sin_addr.s_addr)) 
						{
							QvodCloseSocket(ns);
							continue;
					    }
					}

					continue;
				}

				/* common socket have read event */
				char buf[4096] = {0};
				int nlen = recv(s,buf,4095,0);

				do 
				{
					if(nlen <= 0)
					{
						break;
					}

					char *pFinishLine = strstr(buf, "\r\n\r\n") ;
					if ( pFinishLine )
					{
						break;
					}

					//QVOD_SLEEP(10) ;
					int nRevlen = recv(s,buf+nlen,4095-nlen,0);

					if(nRevlen <= 0)
					{
                        int err = QvodNetGetLastError();
                        if (QVOD_NET_EWOULDBLOCK==err || err == EAGAIN)
                        {
                            continue;
                        }
						break;
					}

					nlen += nRevlen ;
				} 
				while (true);

				//while (true)
				//{
				//	int iRecv = recv(s, buf+nlen, 4095-nlen, 0);
				//	if (iRecv <= 0)
				//	{
				//		break;
				//	}
				//	nlen += iRecv;
				//}
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CHttpServer socket %d recv %d:\n----------\n%s\n----------\n", s, nlen, buf);
#endif
				if(nlen <= 0)
				{
					/* error */
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CHttpServer remove socket %d, i %d, socks %d, errno %d\n", s, i, psvr->m_nsocket, errno);
#endif
					if(psvr->InnerRemoveSocket(s))
					{
						i--;
					}
					continue;
				}
	
				buf[nlen] = '\0';
				int ilen = 0;

				char requrl[1024]={0};
				psvr->GetHttpUrlFromBuffer(buf,requrl,1024);
				ilen = psvr->ProcessYfHttpReq(buf,nlen,i);
				
				if(ilen == 0)
				{
					if(psvr->InnerRemoveSocket(s))
					{
						i--;
					}
					continue;
				}

			} /* POLLIN event */

			if(psvr->pfds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "+++++ httpserver socket %d have except event %d\n", s, errno);
#endif
				if(psvr->InnerRemoveSocket(s))
				{
					i--;
                    if (s == psvr->m_listensock) 
                    {
                        g_httpServer.ReInit();
                    }
				}
				continue;
			}

		} /* for */

	} /* while */

	/* destroy thread object, omit */

	QvodSetThreadEvent(&psvr->m_hTerminalEvent, &psvr->m_hTerminalEventLock);

#ifdef QVODDEBUG
	_OUTPUT(INEED, "thread CHttpServer::Routine stop %lu\n", pthread_self());
#endif

	return (QVOD_THREAD_RETURN)0;
}
#endif

CHttpServerMgr::CHttpServerMgr()
{
	m_pushBufLen = 0;
}

CHttpServerMgr::~CHttpServerMgr()
{
}

int CHttpServerMgr::run()
{
	QvodCreateThreadEvent(&g_GetUrlEvent, &g_GetUrlEventLock);
	CHttpServer* pServer =new CHttpServer();
	
	if (NULL == pServer)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CHttpServerMgr::run() NULL == pServer");
#endif
		return 2;
	}

	if(!pServer->InitListen(m_listenport))
	{
		delete pServer;
		return 2;
	}

	pServer->run();
	m_vctServer.push_back(pServer);

	return 0;
}

int CHttpServerMgr::stop()
{
	std::vector<CHttpServer*>::iterator it;
	CHttpServer* pServer = NULL;

#ifdef QVODDEBUG
	_OUTPUT(INFO, "%s(%d)-%s: CHttpServerMgr::total stop()\n", __FILE__, __LINE__, __FUNCTION__);
#endif

	for(it = m_vctServer.begin();it!= m_vctServer.end();it++)
	{
		pServer = *it;
		pServer->stop();
#ifdef QVODDEBUG
		_OUTPUT(INFO, "%s(%d)-%s: CHttpServerMgr::stop()\n", __FILE__, __LINE__, __FUNCTION__);
#endif
		delete pServer;
	}

	m_vctServer.clear();

	return 0;

}

bool CHttpServerMgr::InnerAddSocket(qvod_sock_t s,QVOD_UINT ip)
{
	std::vector<CHttpServer*>::iterator it;
	CHttpServer* pServer = NULL;

	for(it = m_vctServer.begin();it!= m_vctServer.end();it++)
	{
		pServer = *it;
        if (NULL == pServer) 
        {
            continue;
        }

		if(pServer->GetSocketNum() < 64)
			return pServer->InnerAddSocket(s,ip);
	}

	pServer = new CHttpServer();
    if (NULL == pServer) 
    {
        return false;
    }

	pServer->InnerAddSocket(s,ip);
	pServer->run();
	m_vctServer.push_back(pServer);

	return true;
}

void CHttpServerMgr::InterPushSize(int size)
{
	CAutoLock lock(m_lockPushBufLen);
	m_pushBufLen += size;
}

unsigned int CHttpServerMgr::GetPushSpeed()
{
	return m_pushBufLen/SPCINTERVALTIME;
}

void CHttpServerMgr::init(unsigned short port)
{
	m_listenport = port;
}

bool CHttpServerMgr::ReInit()
{
	std::vector<CHttpServer*>::iterator it;
	CHttpServer* pServer = NULL;

	for(it = m_vctServer.begin();it!= m_vctServer.end();it++)
	{
		pServer = *it;
		if(QVOD_INVALID_SOCKET != pServer->m_listensock)
			return pServer->ReInitListen(g_httpport);
	}

	return false;
}


