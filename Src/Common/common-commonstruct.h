#ifdef WIN32 /* WIN32 */
#pragma warning(disable:4786)
#else /* posix */
//nothing
#endif /* posix end */

#ifndef _COMMON_STRUCT
#define _COMMON_STRUCT
#include <stdio.h>
#include <string>
#include "qvod_string.h"

using namespace std;
// ¬º˛¿‡–Õ∂®“�?
#define EVENT_TYPE_ACCEPT	1	//±ª∂ØΩ”µΩ“ª∏ˆ¡¨Ω�?
#define EVENT_TYPE_CONNECT	0	//÷˜∂ØΩ®¡¢“ª∏ˆ¡¨Ω�?
#define EVENT_TYPE_CLOSE	3	//πÿ±’¡¨Ω�?
#define EVENT_TYPE_OUTTIME	4	//¡¨Ω”≥¨ ±

enum TaskType
{
	TVDOWNLOADTASK=1,
		BTDOWNLOADTASK,
		TVUPLOADTASK,
		BTUPLOADTASK,
		HIDEDOWNLOADTASK=11,
		HTTPDOWNTASK,
		HTTPP2PDOWNTASK,
		HLSDOWNTASK,
		HLSP2PDOWNTASK,

		HTTPUPTASK = 20,
		HTTPNFUPTASK,
};
enum USERTYPE {NONE=0,QVOD,TORRENT,QLIVE,EMULE};
enum NETTYPE {TCP=1,UDP,TUDP};
enum PROTOCOL {QVODBTPRO=1,QLIVEPRO,EMULEPRO};
enum PEERTYPE {ALLTYPE,NORMAL,MINER};

typedef struct _KEY{
public:
	unsigned int ip;//Õ¯¬Á◊÷Ω⁄–�?
	unsigned short port;//Õ¯¬Á◊÷Ω⁄–�?
	unsigned short reserved;
	int sock; /* silver modify, uint --> int */
	_KEY()
	{
		ip = 0;
		port = 0;
		reserved = 0;
		sock = 0;
	}
	_KEY(const _KEY& _A)
	{
		ip = _A.ip;
		port = _A.port;
		sock = _A.sock;
		reserved = 0;
	}
	_KEY& operator = (_KEY const& _A) 
	{
		ip = _A.ip;
		port = _A.port;
		sock = _A.sock;
		reserved = 0;
		return *this;
	}
	bool operator == (_KEY const& _A) const
	{
		if(ip == _A.ip && port == _A.port && sock == _A.sock)
			return true;
		else
			return false;
	}
	bool operator < (_KEY const& _A) const
    {
		//’‚∏ˆ∫Ø ˝÷∏∂®≈≈–Ú≤ﬂ¬�?
		if(ip == _A.ip)
		{
			if(port == _A.port)
			{
				if(sock == _A.sock)
				{
					return false;
				}
				else
				{
					return sock < _A.sock;
				}
			}
			else
			{
				return port < _A.port;
			}
		}
		else
		{
			return ip < _A.ip;
		}
    }
} KEY;
typedef struct _DATA{
	KEY key;
	unsigned int len;//length of the packet
	unsigned char nettype;//tcp udp
	unsigned char protocol;//1:qvod|bt 2:emule
	char* packet;
// 	DATA(){packet=NULL;}
// 	DATA(const DATA& _A)
// 	{
// 		key=_A.key;
// 		len=_A.len;
// 		protocol=_A.protocol;
// 		nettype=_A.nettype;
// 		packet=new char[_A.len];
// 		memcpy(packet,_A.packet,_A.len);
// 	}
// 	DATA& operator = (const DATA const& _A) 
// 	{
// 		key=_A.key;
// 		len=_A.len;
// 		protocol=_A.protocol;
// 		nettype=_A.nettype;
// 		if(packet!=NULL)
// 			delete[] packet;
// 		packet=new char[_A.len];
// 		memcpy(packet,_A.packet,_A.len);
// 		return *this;
// 	}
// 	~DATA()
// 	{
// 		if(packet!=NULL)
// 			delete[] packet;
// 	}
} DATA;
typedef struct _HASH{
public:
	unsigned char data[20];
	_HASH()
	{
		memset(data,0,20);
	}
	_HASH(const char* hash)
	{
		memcpy(data,hash,20);
	}
	_HASH(const unsigned char* hash)
	{
		memcpy(data,hash,20);
	}
	bool operator < (_HASH const& _A) const
	{
		if(memcmp(data, _A.data, 20) < 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool operator == (_HASH const& _A) const
	{
		if(memcmp(data,_A.data,20)==0)
			return true;
		else
			return false;
	}
	bool operator != (_HASH const& _A) const
	{
		if(memcmp(data,_A.data,20)==0)
			return false;
		else
			return true;
	}
	_HASH& operator = (_HASH const& _A) 
	{
		memcpy(data,_A.data,20);
		return *this;
	}
} HASH;

typedef struct _HASHDATA
{
	HASH hash;
	DATA data;
	unsigned char protocol;
	QVOD_DWORD timestamp;
	_HASHDATA& operator = (_HASHDATA const& _A) 
	{
		memcpy(hash.data,_A.hash.data,20);
		data=_A.data;
		protocol = _A.protocol;
		timestamp = _A.timestamp;
		return *this;
	}
} HASHDATA;

struct SPEEDATA
{
	HASH hash;
	KEY key;
	int size;
	QVOD_DWORD timestamp;
	void* callback;//ªÿµ˜∫Ø ˝÷∏’�?
};

struct TRACKDATA
{
	KEY key;
	char hash[20];
	char trackadr[55];
	QVOD_DWORD timestamp;
	USERTYPE type;
	bool valid;
	unsigned char contime;
	bool operator == (TRACKDATA const& _A) const
	{
		if(memcmp(hash,_A.hash,20)==0&&
			strcmp(trackadr,_A.trackadr)==0&&
			_A.type==type)
			return true;
		else
			return false;
	}
};

struct TIMEOUTREQ 
{
	KEY key;
	unsigned char protocol;
	QVOD_DWORD timestamp;	
};

//ª∫¥Ê“Ú∞Îø™¡¨Ω” ˝œﬁ÷∆µƒpeer
struct PEER
{
	HASH hash;
	char random[12];
	unsigned int ip;
	unsigned short port;
	unsigned char protocol;//–≠“È∞Ê±æ	
	unsigned char type;		//0Œ™qvod peer,1Œ™qlive peer
	PEERTYPE peertype;	
};

//ª∫¥Êø’œ–µƒpeer
struct KEYTIME
{
	KEY key;
	mutable unsigned long timestamp;
	bool operator < (KEYTIME const& _A) const
	{
		if(key == _A.key)
		{
			return false;
		}
		else
		{
			return key < _A.key;
		}
	}
};

struct Hash_Addr
{
	unsigned char hash[20];
	std::string	addr;
};

struct fragment 
{
	KEY key;
	unsigned char netevent;
	unsigned int len;
	char* buf;
};


struct httpiecereq
{
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	QVOD_UINT64 start;
	QVOD_UINT64 length;
};

struct YfHttpPieceReq
{
	YfHttpPieceReq()
	{
		start = 0;
		length = 0;
	}
	string strOrlurl;
	string strReferer;
	QVOD_UINT64 start;
	unsigned int length;
};

struct linkservershare
{
	char hash[40];
	char url[1024];
	char refer[1024];
	char web[1024];
	char title[255];
	char type[10];
	int size;
};

struct agenttaskinfo
{
	char acchash[20];
	unsigned int downloadspeed;
	unsigned int downloadlen;
	unsigned int filelen;
	char filename[512];
};

struct SQueryUrltrackerResult
{
	HASH hash;
	QVOD_INT64 size;
	string strCdn;
	string strOrgCdn;
};

struct SM3u8TsReq
{
    SM3u8TsReq()
    {
        start = 0;
        uFilePos = -1;
    }
	string strHash;
	string strOrlurl;
	string strReferer;
	QVOD_UINT64 start;
    QVOD_UINT64 uFilePos;   // 相对于文件的位置（仅m3u8文件有签名大小的ts才有效）
    unsigned int length;
    unsigned char type;		//0Œ™m3u8£¨1Œ™ts
};

struct HttpPostReq
{
	int id;
	string strHash;
	string strOrlurl;
	string strReferer;

	string strHeadAdd;
	string strSendData;
};

#endif

