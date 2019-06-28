#ifndef MESSAGE_H
#define MESSAGE_H
//typedef unsigned int UINT; /* silver delete */
//typedef unsigned char BYTE; /* silver delete */
//typedef unsigned long QVOD_DWORD; /* silver delete */
#include "qvod_string.h"

#define TRACKER_LOGIN	        0x301
#define TRACKER_LOGIN_RESP  	0x301

#define TRACKER_TERMINATE     	0x302

#define TRACKER_ENQUIRE	0x303
#define TRACKER_ENQUIRE_RESP	0x303

#define TRACKER_ADDRESOURCE	       0x304
#define TRACKER_ADDRESOURCE_RESP	0x304

#define TRACKER_DELRESOURCE	     0x305
#define TRACKER_DELRESOURCE_RESP	0x305

#define TRACKER_GETPEERS    	0x306
#define TRACKER_GETPEERS_RESP	0x306

#define TRACKER_GETFILESUSERS	0x307
#define TRACKER_GETFILESUSERS_RESP	0x307


#define TRACKER_STUN	       0x311//穿透请求
#define TRACKER_STUN_RESP	   0x311//穿透请求回应
#define TRACKER_STUNNOTIFY  	0x312//穿透通知，

#define TRACKER_AGENT_GETPEERS	0x326//获取资源用户,同时登记自己也在下载该资源

#define CACHE_GETCACHE		0x351
#define CACHE_GETCACHE_RSP	0x351

#define TRACKER_QUERY_ISP_BYIP        0x501//查询ip对应isp

struct peerid
{
	char checksum[4];
	char version[4];
	char random[12];
};

struct msghead
{

	unsigned short len;
	unsigned short command;
	unsigned int sequence;
};

struct msgheadreq
{
	msghead head;
	char Peer_id[20];
};

struct LoginReq
{
	msghead head;
	char Peer_id[20];
	unsigned int ip;
	unsigned short listenport;
	char NAT_Type;	
	char Reserved;
};
struct LoginRsp
{
	msghead head;
	unsigned int Peer_ip;
	unsigned short Peer_port;
	unsigned char Result;
	unsigned char version;

};
struct LogoutReq
{
	msghead head;
	char Peer_id[20];
};

struct HandShakeReq
{
	msghead head;
	char Peer_id[20];
};

struct HandShakeRsp
{
	msghead head;
	unsigned char Result;
	unsigned char reserved1; 
	unsigned short reserved2;
};


struct ShareReq
{
	msghead head;
	char Peer_id [20];
	unsigned char Filehash[20];

};

#pragma pack(1)
struct ShareRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char Result;
	unsigned char reserved1; 
	unsigned short reserved2;
};
#pragma pack()

struct UnShareReq
{
	msghead head;
	char Peer_id [20];
	unsigned char Filehash[20];

};

#pragma pack(1)
struct UnShareRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char Result;
	unsigned char reserved1; 
	unsigned short reserved2;
};
#pragma pack()

struct DownLoadReq
{
	msghead head;
	char Peer_id [20];
	unsigned char FileHash[20];
};

#pragma pack(1)
struct DownLoadRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char Result;
	unsigned char reserved;//超级节点数		
	unsigned char Peer_count;//以后改为UNSIGNED CHAR
	unsigned char reserved2;//种子个数
};
#pragma pack()

#pragma pack(1)
struct AgentGetPeerReq//代理请求结构
{
	msghead head;
	peerid id;
	unsigned char hash[20];
	unsigned int publicip;
	unsigned short listenport;//监听端口
	unsigned char nattype;
	unsigned char reserved;
};
#pragma pack()

struct AgentGetPeerRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned int publicip;
	unsigned char result;//0:成功,1:资源不存在, 4:版本太旧;
	unsigned char version;//0:无新版本,1:有新版本
	unsigned char peers;//peer数
	unsigned char reserved;
};


struct Peer
{
	char random[12];
	unsigned int ip;
	unsigned short port;
	unsigned char nattype;
	unsigned char protocol;//协议版本		
};

struct GetUser
{
	msghead head;
	char Peer_id[20];
	unsigned char Filehash[20];

};
struct GetUserRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char result; // 0:成功;1:资源不存在; 2:未登录; 3:消息格式错
	unsigned char supers;//超级节点数
	unsigned char count;//peer数
	unsigned char seeds;//种子个数

};

//以下请求消息不放入队列
struct StunReq
{
	msghead head;
	peerid sourceid;//自身peerid
	peerid targetid;//对方peerid
	unsigned char hash[20];//资源HASH
};
struct StunRsp
{
	msghead head; //返回消息就表示对方不存在
	unsigned char hash[20];//资源HASH
	unsigned char result; // 1:对方不存在;2:未登录; 3:消息格式错
	unsigned char reserved1; 
	unsigned short reserved2;

};
struct StunNotify
{
	msghead head;
	unsigned char hash[20];
	peerid sourceid;//源peerid
	unsigned int ip;//发起方IP
	unsigned short port; //发起方端口
	unsigned char nattype;//对方网络类型
	unsigned char reserved;
};

//查询ISP号
struct GetIspID
{
	msghead header;
	char Peer_id[20];
	unsigned int ip;		//公网ip
};

struct GetIspIDRsp
{
	msghead header;
	unsigned int ip;
	unsigned char isp[4];
};

//从流量币收到的数据,要上报给统计服务器
struct userid
{
	unsigned int len;//后面数据的大小，不包括本身，下同
	unsigned char id ;//协议命令号
	unsigned char reserved[3];
	unsigned int uid; //用户id
};

//以下为BT标准协议
#define CHOKE 0
#define UNCHOKE 1
#define INTERESTED 2
#define NOT_INTRESTED 3
#define HAVE 4
#define BITFIELD 5
#define REQUEST 6
#define PIECE 7
#define CANCEL 8
#define METAFILEREQ 9
#define METAFILERSP 10
#define REQ_TIMEOUT 11
#define VIP_AUTHREQ 12
#define VIP_AUTHRSP 13
#define UNHAVE 14
#define USERID		109

//#define BTTRACKERREFERTO 101
//#define BTTRACKERRSP     102
//#define UDPTRACKERRSP    103

#define CMDDECLARE       0xA0    //160
#define FIXEDCMD         0xA1    //161
#define NOTIFYPEERS      0xA2    //162
#define BCUNKNOWN        0xA3    //163

#define BTGETPEER         20
#define BTTKHTTPCONNECTED 21 
#define BTTKUDPCONNECTED  22
#define BTTKUDPRESPONCE   23

#define CONNECTED	  	  30
#define DISCONNECTED  	  31
#define ACCEPTED		  32


//handshake
//结构：[protocol string length] + [protocol string] + [reserved] + [info_hash] + [peer_id]

//protocol string length：1字节整数，对应其后的protocol string的长度。
//protocol string：协议标识字符串。BitTorrent协议1.0版本中此字符串为
//"BitTorrent protocol"，所以对应的protocol string length为19。
//reserved：8个保留字节。当前所有BitTorrent实现都设置为0。
//info_hash：20字节SHA1 hash值，与种子文件的info_hash一致。
//peer_id：20字节字符串，发送方终端自身的标识。


struct handshake
{
	unsigned char length;
	char protocol[19];//protocol ex标示缓存
	unsigned char checkbit;//第1位用于crc校验,其余7位暂空
	unsigned char isp[4];
	char reserved[3];
	unsigned char info_hash[20];
	char peer_id[20];
};


//keep-alive
//结构：[len=0000]
//没有内容，只有4个字节的整数，值为0。每隔2分钟发一次。
#define keep_alive "\0\0\0\0"

//choke
//结构：[len=0001] + [id=0]
//表示线路阻塞，无法响应下载请求。
//直接发送不定结构
#pragma pack(1)
struct encodehead
{
	unsigned int key;//用于解密后面部分
	unsigned int key2;
	unsigned char paddinglen;//padding的长度，0-15
	//unsigned char padding[] //长度不固定
	//unsigned int checkkey;//解密后等于key
	//unsigned int checkkey2;//解密后等于key2
	//packet
};
#pragma pack()

#pragma pack(1)
typedef struct _head
{
	unsigned int len;//后面数据的大小，不包括本身，下同
	unsigned char id ;//协议命令号
}stdhead,agentmsghead,interested,not_interested,choke,unchoke;
#pragma pack()


// struct btpeers
// {
// 	stdhead head;
// 	unsigned char hash[20];
// };
#pragma pack(1)
typedef struct 
{
	stdhead head;
	unsigned char status;
}connected,accepted,disconnected;
#pragma pack()

#pragma pack(1)
struct estdhead
{
	unsigned char sign;
	unsigned int len;
	unsigned char id ;
};
#pragma pack()

#pragma pack(1)
struct econnected
{
	estdhead head;
	unsigned char status;
};
#pragma pack()
//unchoke
//结构：[len=0001] + [id=1]
//表示线路通畅，可响应下载请求。
//直接发送不定结构


//interested
//结构：[len=0001] + [id=2]
//表示对对端共享的数据感兴趣。


//not_interested
//结构：[len=0001] + [id=3]
//与interested相反。


//have
//结构：[len=0005] + [id=4] + [piece index]
//告诉其它对端，自己拥有某个分片。
#pragma pack(1)
struct vipauthreq
{
	unsigned int len;
	unsigned char id ;
	QVOD_INT64 sessionid;
	char username[16];
};
#pragma pack()

#pragma pack(1)
struct vipauthrsp
{
	unsigned int len;
	unsigned char id ;
	unsigned char result;//返回结果,0成功,1认证失败,2流量不足,3服务器连接上线
};
#pragma pack()

#pragma pack(1)
struct have
{
	unsigned int len;//后面数据的大小，不包括本身，下同
	unsigned char id ;//协议命令号
	unsigned int piece_index;
};
#pragma pack()

#pragma pack(1)
struct unhave
{
	unsigned int len;//后面数据的大小，不包括本身，下同
	unsigned char id ;//协议命令号
	unsigned int piece_index;
};
#pragma pack()
//bitfield
//结构：[len=0001+X] + [id=5] + [bitfield]
//当握手完成之后，此消息即应该被发送。但是如果终端没有任何分片，可不必发送此消息。
//每个bit位代表一个分片，所以一个字节对应8个分片。
//第一个字节最高bit位代表第0个分片，依此类推。如果对应bit为1，表示有该分片，0则表示没有。
//如果bitfield末尾有多余的bits，置为0。bitfield的字节长度与分片的多少相关

#pragma pack(1)
struct bitfield
{
	unsigned int len;//后面数据的大小，不包括本身，下同
	unsigned char id ;//协议命令号
	QVOD_BYTE bit[1];
};
#pragma pack()

//request
//结构：[len=0013] + [id=6] + [index] + [begin] + [length]
//用于请求一个分片中的一个数据块。index指定一个分片；begin指定起始于该分片的第几个byte位置；
//length指定请求多长的数据。这里的length必须小于分片的大小。
//典型的分片大小为256K，这里length必须小于该值，一般指定为32K。
//index,begin,length都是4个字节整数。

#pragma pack(1)
struct request
{
	unsigned int len;//后面数据的大小，不包括本身，下同
	unsigned char id ;//协议命令号
	unsigned int index;
	unsigned int begin;
	unsigned int length;
};
#pragma pack()

//piece
//结构：[len=0009+X] + [id=7] + [index] + [begin] + [block]
//用于发送一个数据块。index,begin,length都是4个字节整数。index，begin含义与request对应。
//block是文件的一块数据，显然此数据块是index分片的一部分。

#pragma pack(1)
struct piece
{
	unsigned int len;
	unsigned char id ;
	unsigned int index;
	unsigned int begin;

};
#pragma pack()

//cancel
//结构：[len=0013] + [id=8] + [index] + [begin] + [length]
//与request消息相对，告诉对端放弃某个数据块的下载。index，begin, length必须对应已经发送的request消息。

#pragma pack(1)
struct cancel
{
	unsigned int len;
	unsigned char id ;
	unsigned int index;
	unsigned int begin;
	unsigned int length;
};
#pragma pack()

//扩展协议
//request metafile
//结构：[len=0021] + [id=9] + [info_hash]
//客户端之间，使用此扩展消息请求获取metafile文件。

#pragma pack(1)
struct metafilereq
{
	unsigned int len;
	unsigned char id ;
	char info_hash[20];
};
#pragma pack()

//metafile
//结构：[len=0001+X] + [id=10] + [content]
//回复request metafile，发送文件内容。若无此文件，内容为空。
#pragma pack(1)
struct metafilersp
{
	unsigned int len;
	unsigned char id ;
	char content[1];
};
#pragma pack()

#pragma pack(1)
struct requesttimeout
{
	unsigned int len;
	unsigned char id ;
	request req;

};
#pragma pack()

//直播协议
#define LIVEHANDSHAKE 201
#define LIVEBITFIELD  202
#define LIVEREQUEST   203
#define LIVEPIECE     204
#define LIVETIMEOUT   205

#pragma pack(1)
struct livehandshake//size 52
{
	stdhead header;
	char protocol[19];//qlive protocl...
	unsigned char hash[20];
	char peerid[20];//QLVE0064XXXXXXXXXXXX
	unsigned char reserved[8];
};
#pragma pack()

#pragma pack(1)
struct livebitfield
{
	stdhead header;
	unsigned int first;
	unsigned int curpos;
	unsigned int last;
	unsigned int orginstarttime;
	//char* bit;
};
#pragma pack()

#pragma pack(1)
struct liverequest
{
	stdhead header;
	unsigned int index;
};
#pragma pack()

#pragma pack(1)
struct livepiece
{
	stdhead header;
	unsigned int index;
	char sha1[20];
	//char* data;
};
#pragma pack()

#pragma pack(1)
struct livetimeout
{
	stdhead header;
	unsigned int index;
};
#pragma pack()

//bitcomet扩展协议
#pragma pack(1)
struct cmddeclare
{
	unsigned int len;
	unsigned char id;
	unsigned char code[12]; 
};
#pragma pack()

#pragma pack(1)
struct fixedcmd
{
	unsigned int len;
	unsigned char id;
	unsigned int fixed1;//为0
	unsigned int fixed2;//0x1C000000
	unsigned int fixed3;//为0
};
#pragma pack()

#pragma pack(1)
struct btpeerinfo
{
	char peerid[20];
	unsigned int ip;//0为自身
	unsigned short port;
	unsigned short reserved;//为0
};
#pragma pack()

#pragma pack(1)
struct notifypeers
{
	unsigned int len;
	unsigned char id;
	unsigned int fixed1;//为0
	unsigned int fixed2;//0x1C000000
	char peerid[20];
	unsigned int ip;//0为自身
	unsigned short port;
	unsigned short reserved;//为0
};
#pragma pack()

#define CONNECT 0 
#define ANNOUNCE 1
#define SCRAPE 2
#define TRACKERERROR 3

typedef struct _btconnect 
{
	QVOD_INT64 connid;
	QVOD_UINT action;
	QVOD_UINT transid;
}bthead,btconnect;
struct btconnectrsp
{
	QVOD_UINT action;
	QVOD_UINT transid;
	QVOD_INT64 connid;
};

#pragma pack(1)
struct btannounce
{
	bthead head;
	unsigned char hash[20];
	char peerid[20];
	QVOD_INT64 downloaded ;
	QVOD_INT64 left;
	QVOD_INT64 uploaded;
	int event;
	int ip; 
	int key; 
	int num_want; 
	short port;  
};
#pragma pack()

struct btannouncersp
{
	QVOD_UINT action;
	QVOD_UINT transid;
	QVOD_UINT interval;
	QVOD_UINT leechers;
	QVOD_UINT seeds;
	//	20 + 6 * n  32-bit integer  IP address  
	//	24 + 6 * n  16-bit integer  TCP port  

};

struct bterror
{
	QVOD_UINT action;
	QVOD_UINT transid;
	//	8  string  message  

};

#define AGENTSHAREFILE			0xA3
#define AGENTREQLINKRSP			0xA4
#define AGENTSHARELINKRSP		0xA5
#define AGENTREQUESTPIECE		0xA5
#define AGENTREQUESTBLOCK		0xA6
#define AGENTREMOVEFILE			0xA7
#define AGENTREQUESTBLOCKINFO   0xA8
#define AGENTREQUESTMETA	    0xA9
#define AGENTREQUESTCACHE		0xAA
#define AGENTRANGE				0xAB
#define AGENTM3U8				0xAC
#define AGENTM3U8TS				0xAD
#define AGENTUPLOAD				0xAE

#pragma pack(1)
typedef struct _agentmsgshare
{
	agentmsghead head;
	unsigned char result;//0,1,2
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	unsigned char hash[20];
	char fileurl[1024];
	char filepath[255];
	QVOD_UINT64 start;
	QVOD_UINT64 filelen;
	QVOD_UINT64 downloadlen;
}agentmsgshare;
#pragma pack()

#pragma pack(1)
typedef struct _agentreqblock
{
	agentmsghead head;
	unsigned char result;//0,1,2
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	unsigned char hash[20];
	char* pbuf;
	QVOD_UINT64 pos;
	QVOD_UINT64 len;
	unsigned char type; //1:block;2:index
}agentreqblock;
#pragma pack()

#pragma pack(1)
struct agentreqlinkrsp
{
	agentmsghead head;
	unsigned char hash[20];
	char acchash[20];
	char url[1024];
	unsigned int filelen;
};
#pragma pack()

#pragma pack(1)
struct agentrequestpiece
{
	agentmsghead head;
	unsigned char result;//0,1
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	int start;
	int length;
	int piecelen;
	char buf[65536];
};
#pragma pack()

#pragma pack(1)
struct agentrequestcache
{
	agentmsghead head;
	unsigned char result;//0,1
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	unsigned int piecelen;
	char hash[40];
	unsigned int start;
	unsigned int downlen;
	char* buf;
};
#pragma pack()

#pragma pack(1)
struct agentremovefile
{
	agentmsghead head;
	unsigned char hash[20];
};

struct agentreqblockinfo
{
	agentmsghead head;
	unsigned int ip;
	unsigned short port;
	unsigned char type;
	unsigned char reserved;
	unsigned int sock;
	unsigned char hash[20];
};
#pragma pack()

#pragma pack(1)
struct agentreqmeta
{
	agentmsghead head;
	unsigned char hash[20];
	unsigned int metalen;
	char* metabuf;
};
#pragma pack()

#pragma pack(1)
struct agentrange
{
	agentmsghead head;
	int id;
	unsigned char result;			// 0-complete, 1-partial, 2-no data, 3-http返回不是200或重定向，链接失效了 4-超过重试次数
	unsigned  short errcode;	// 错误码
	unsigned char hash[20];
	char *buf;
	QVOD_INT64 start;
	QVOD_INT64 len;
	QVOD_INT64 filesize;
};
#pragma pack()

#pragma pack(1)
struct agentm3u8
{
	agentmsghead head;
	unsigned char result;
	unsigned short errcode;
	unsigned char hash[20];
	char *buf;
	QVOD_INT64 len;
};
#pragma pack()

#pragma pack(1)
struct agentm3u8ts
{
	agentmsghead head;
	unsigned char result;
	unsigned short errcode;
	unsigned char hash[20];
	std::string filename;
	char *buf;
	QVOD_INT64 start;
	QVOD_INT64 len;
	QVOD_INT64 filesize;
};
#pragma pack()
#endif

