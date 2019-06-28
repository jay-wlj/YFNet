#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H

#include <iostream>
#include <vector>
#include "qvod_event.h"
#include "qvod_thread.h"
#include "qvod_lock.h"
#include "qvod_sock.h"
#include "global.h"

class CTask;

enum CLIENTTYPE
{
	IE = 1,
	REALPLAYER = 2,
	WMPLAYER = 3,
	QVODPLAYER = 4,
	QVODPLAYER2 = 5,
	OTHER
};

struct RMChunk
{
	char Name[4];
	QVOD_UINT Len;
};

struct RMProperties
{
	unsigned int                  object_id;
	unsigned int                  size;
	unsigned short                object_version;
	unsigned int                  max_bit_rate;
	unsigned int                  avg_bit_rate;
	unsigned int                  max_packet_size;
	unsigned int                  avg_packet_size;
	unsigned int                  num_packets;
	unsigned int                  duration;
	unsigned int                  preroll;
	unsigned int                  index_offset;
	unsigned int                  data_offset;
	unsigned short                num_streams;
	unsigned short                flags;
};

struct Header
{
	QVOD_GUID guid;
	QVOD_UINT64 Obj_Size;
	QVOD_UINT Obj_Num;
	QVOD_UCHAR Reserved1;
	QVOD_UCHAR Reserved2;
};

struct FileProperties
{
	QVOD_GUID guid;
	QVOD_UINT64 Obj_Size;
	QVOD_GUID File_ID;
	QVOD_UINT64 File_Size;
	QVOD_UINT64 File_Date;
	QVOD_UINT64 Packets_Count;
	QVOD_UINT64 Play_Duration;
	QVOD_UINT64 Send_Duration;
	QVOD_UINT64 Preroll;
	QVOD_UINT Flags;
	QVOD_UINT Max_Packet_Size;
	QVOD_UINT Min_Packet_Size;
	QVOD_UINT max_Bitrate;
};

class CHttpServer //:public CControl
{	

public:
	CHttpServer();
	virtual ~CHttpServer();

	virtual int		run();
	virtual int		stop();

	bool			GetHttpUrlFromBuffer(char* buf,char* url,int urllen);
	int				GetStartFromUrl(char* url);
	void			GetFileNameFromUrl(char* url,char* filename);

	bool			OnTimer();
	bool			OnTimerEvent();
	qvod_sock_t		GetSocket(int i){return m_SocketArray[i];}

#ifdef WIN32
	WSAEVENT*		GetEventPtr(int i){return m_EventArray + i;}
#endif

	int				GetSocketNum(){return m_nsocket;}
	virtual bool	InnerAddSocket(qvod_sock_t s,QVOD_UINT ip);
	bool			InnerRemoveSocket(qvod_sock_t s);
	bool			ReInitListen(unsigned short port);
	bool			InitListen(unsigned short port);
	int				ProcessYfHttpReq(char* buf, int len, int i);

	static QVOD_THREAD_RETURN Routine(void* pvoid);

public:
	QVOD_DWORD				m_tickecount;
	QVOD_UINT				m_interval;

	qvod_thread_event_t		m_hTerminalEvent;
	pthread_mutex_t			m_hTerminalEventLock;
	QVOD_DWORD				m_TimeArray[64];
	/*
	 * no need to connect, only listen.
	 */
	
#ifdef WIN32
	WSAEVENT m_EventArray[64];
#else
	struct pollfd			pfds[64];
#endif	
	
	qvod_sock_t				m_listensock; /* only for recording the value of listen socket */

	QVOD_UINT				m_uLocalIP; // 127.0.0.1本地代理IP
	qvod_sock_t				m_SocketArray[64]; /* including listen sockets */
	QVOD_UINT				m_SocketIP[64];
	QVOD_UINT64				m_FileposArray[64];
	QVOD_UINT64				m_FileEndPosArray[64];
	HASH					m_TaskArray[64];
	HASH					m_TaskUrlArray[64];
	QVOD_UINT				m_nsocket;//套接字数目，最多只能4个
	QVOD_UINT				m_sended;
	char					m_databuf[64][HTTP_BUFFER_LENGTH];
	QVOD_UINT				m_dataLen[64];
	QVOD_UINT				m_sendlen[64];
	bool					m_keepalive[64];
	bool					m_bHttpRange[64];
	bool					m_bHttpReply[64];				//是否已经回复http头
	QVOD_DWORD				m_NextPushTimeArray[64];	//下次推送数据的时间
	QVOD_UINT				m_pushedLen[64];			//已推送的数据量
	string					m_strURI[64];
	CLock					m_lock;
};

class CHttpServerMgr: public CHttpServer
{
public:
	CHttpServerMgr();
	virtual ~CHttpServerMgr();

	virtual int			run();
	virtual int			stop();
	void				init(unsigned short port);
	bool				ReInit();
	virtual bool		InnerAddSocket(qvod_sock_t s,QVOD_UINT ip);
	void				InterPushSize(int size);
	unsigned int		GetPushSpeed();
	
private:
	std::vector<CHttpServer*>	m_vctServer;
	unsigned short				m_listenport;
	unsigned int				m_pushBufLen;
    CLock						m_lockPushBufLen;
};
#endif

