#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "qvod_string.h"
#include "qvod_lock.h"
#define QVODHTTPTRACKER "http://track.qvod.com"
#define KUAIWANHTTPTRACKER "http://track.kuaiwan.com"



#define LIVETRACKER			"qlive.daddymami.net"
#define LIVETRACKERIP		"60.55.34.168"

#define STUNSERVER			"stun.daddymami.net"
#define STUNSERVERIP		"175.6.0.124"

#define DEBUGAUTHSERVER		"debugauth.daddymami.net"
#define DEBUGAUTHSERVERIP	"222.184.91.210"

#define JSONSTCSERVER		"statis.daddymami.net"
#define JSONSTCSERVERIP		"175.6.0.120"

#define URLTRACKER			"urltracker.yunfan.com"
#define URLTRACKERIP		"183.136.236.21"
#define URLTRACKERPORT		8081

#define MINER_TRACKER_URL	"mtracker.yunfan.com"
#define MINER_TRACKER_IP	"183.60.41.76"

#ifdef MONGOTV
#define CLOUDCFGSERVER		"cloudcfg.hunantv.yunfan.com"
#define CLOUDCFGSERVERIP	"218.71.143.156"

#define SHARERATE_SERVER_URL	"stat.yunfan.com"
#define SHARERATE_SERVER_IP		"183.60.41.19"

#define DETECTUPSPEEDSERVER	"hunantv.tspeed.yunfan.com"
#define DETECTUPSPEEDSERVERIP	"175.6.0.44"

#else 
#define CLOUDCFGSERVER		"cloudcfg.hunantv.yunfan.com"
#define CLOUDCFGSERVERIP	"218.71.143.156"


#define SHARERATE_SERVER_URL	"stat.yunfan.com"
#define SHARERATE_SERVER_IP		"183.60.41.19"

#define DETECTUPSPEEDSERVER	"hunantv.tspeed.yunfan.com"
#define DETECTUPSPEEDSERVERIP	"175.6.0.44"
#endif

#ifdef MONGOTV
#define QVODTRACKER "hunantv.cloud.yunfan.com"
#define QVODDEFAULTTRACKERIP "115.231.216.12"
#define QVODAGENT "hunantv.agent.yunfan.com"
#define QVODDEFAULTAGENTIP "183.136.237.22"
#define QVODAGENTURL "http://hunantv.update.yunfan.com/hunantv_agent.cfg"
#define AGENTLIST_NAME	"hunantv_agent.ini"
#else
#define QVODTRACKER "hunantv.cloud.yunfan.com"
#define QVODDEFAULTTRACKERIP "115.231.216.12"
#define QVODAGENT "hunantv.agent.yunfan.com"
#define QVODDEFAULTAGENTIP "183.136.237.22"
#define QVODAGENTURL "http://hunantv.update.yunfan.com/hunantv_agent.cfg"
#define AGENTLIST_NAME	"hunantv_agent.ini"
#endif

#define QVODHTTPTRACKER "http://track.qvod.com"

#define STUNSERVER		"stun.daddymami.net"
#define STUNSERVERIP		"175.6.0.124"

#define STUN1SERVER		"stun1.daddymami.net"
#define STUN1SERVERIP		"60.55.35.155"

#define JSONSTCSERVER		"statis.daddymami.net"
#define JSONSTCSERVERIP		"175.6.0.120"

#define URLTRACKER			"urltracker.yunfan.com"
#define URLTRACKERIP		"183.136.236.21"
#define URLTRACKERPORT		8081

#define MINER_TRACKER_URL	"mtracker.yunfan.com"
#define MINER_TRACKER_IP	"183.60.41.76"

#define NFTRACKERIP		"172.17.6.16"
#define NFTRACKERPORT		5000

#ifdef MONGOTV
#define SHARERATE_SERVER_URL	"stat.yunfan.com"
#define SHARERATE_SERVER_IP		"113.106.27.213"
#endif

#define HTTPTRACKER QVODHTTPTRACKER
#define TRACKER QVODTRACKER
#define AGENT QVODAGENT
#define DEFAULTTRACKERIP QVODDEFAULTTRACKERIP
#define DEFAULTAGENTIP QVODDEFAULTAGENTIP
#define AGENTURL QVODAGENTURL



#define SafeDelete(pObject)				if(pObject) {delete pObject; pObject = NULL;}
#define SafeRelease(pObject)				if(pObject) {pObject->release(); pObject = NULL;}
#define SafeDeleteArray(p)					if(p){delete []p; p = NULL;}

#define DECODEJSONINTVALUE(ITEM,NAME,INFO)\
	if (##ITEM.isObject() &&  ##ITEM.isMember(#NAME) && ##ITEM[#NAME].isInt() )\
	{\
		INFO = ##ITEM[#NAME].asInt();\
	}


#ifdef KUAIWAN
#define HTTPTRACKER KUAIWANHTTPTRACKER
#else
#define HTTPTRACKER QVODHTTPTRACKER
#endif

#define QVODPLAYER_NAME "YfNet"
#define QVODTERMINAL_VERSION 0x0505

#ifdef _DEBUG
#define QVOD_DEBUG_SCREEN 1
#define QVOD_DEBUG_FILE 1
#define QVODDEBUG
#else
#define QVOD_DEBUG_SCREEN 0
#define QVOD_DEBUG_FILE 1
#endif

#define QVOD_USE_CACHEMEM									// 是否支持写入内存(仅当前播放任务)
#define SUPPORT_HTTPS												// 是否支持https访问

//用户级定义
#define HTTP_BUFFER_LENGTH 16384
#define ACC_LIMIT_SIZE (512*1024)
#define SPCINTERVALTIME 10//interval time for speed count,10sec
#define BLOCK_SIZE 8192//16384
#define REQUEST_NUM 2
#ifdef SUPERNODE
#define PIECE_TIMEOUT 32000
#else
#define PIECE_TIMEOUT 8000
#define PIECE_EMERG_TIMEOUT 5000
#endif


#define HTTP_MEMORY_BLOCK 4194304 //http data block

#define MEMORY_SIZE (20*1024*1024)   //内存版数据空间
#define MEMORY_INDEX_SIZE (2*1024*1024) //内存版索引空间大小

//#define MAX_DOWN_DATATIME_FROM_CDN	25	//从cdn一次http请求最大的数据时长，单位秒
//qlive
#define PIECE_LEN (8*1024)
#define MAX_PIECE_SIZE 1200
#define BLOCK_LEN (188*43)
#define MAX_BITFIELD_SIZE (MAX_PIECE_SIZE/8)
#define PIECE_DATA_LENGTH (8*1024)
#define MAX_REQUEST_NUMS 4

#define MAX_DELAY_TIME (5*1000)
#define SUPERNODE_HEART_BEAT_TIME (25*1000)
#define SUPERNODE_HEART_BEAT_NOTFULL_TIME (5*1000)
#define PEER_HEART_BEAT_TIME (2*1000)
#define UP_FORWORD_PIECES 200


#define MAX_RECV_BUF_LEN (1024*10)

#define SOURCE_SERVER 0x01
#define CACHE_SERVER 0x02
#define NORMAL_PEER 0x00
#define PIECE_LEN	(8*1024)
#define SHA1_LEN    20
#define BITS_LEN    (MAX_PIECE_SIZE/8)
//qlive end

enum QVODNatType
{
	NatTypeOpen = 0,
	NatTypeConeNat,
	NatTypeRestrictedNat,
	NatTypePortRestrictedNat,
	NatTypeSymNat,
	NatTypeSymFirewall,
	NatTypeBlocked,
	NatTypeUnknown,
	NatTypeFailure,
};

enum WindowsVersionCode
{
	WINDOWS_2000 = 1,
	WINDOWS_XP,
	WINDOWS_XP_SP1,
	WINDOWS_XP_SP2,
	WINDOWS_XP_SP3,
	WINDOWS_2003,
	WINDOWS_VISTA,
	WINDOWS_VISTA_SP1,
	WINDOWS_VISTA_SP2,
	WINDOWS_7,
	WINDOWS_7_SP1,
	WINDOWS_8
};
extern long g_ConnectingSock;
extern long g_connectinglmt;
extern QVOD_DWORD g_dwWindowsMajorVersion;
extern QVOD_DWORD g_dwWindowsMinorVersion;
extern QVOD_DWORD g_dwPacketVersion;
extern unsigned char g_WindowsVersionCode;
extern unsigned short g_TerminalMinVersion;
extern unsigned short g_TerminalProtocol;

extern QVOD_UINT g_uHostIP;
extern QVOD_USHORT g_listenPort;
extern int g_NATtype;


int  InitQvod();
void GetOperationVersion();
QVOD_UINT GetLocalIP();
int  GetNatType();
bool IsLANIP(QVOD_DWORD nIP);
bool Qvod_Gethostbyname(QVOD_IN char* name,QVOD_OUT unsigned long& value);
void Enc2(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key2);
void Dec2(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key2);
void Enc3(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key);
void Dec3(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key);
void Enc4(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key);
void Dec4(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key);

extern "C"
{
	//获取视频的时长，单位为秒
	int GetVideoTime(const char* pHash,QVOD_INT64 iFileSize);
	//ffmpeg回调读取视频数据
	int ReadYfFile(void* pFile,char* pBuf,int len);

	typedef struct FilePos
	{
		char szHash[40];
		QVOD_INT64 iStartPos;
		QVOD_INT64	iFileSize;
	}FilePos;
};


#endif